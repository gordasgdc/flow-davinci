// SPDX-License-Identifier: MIT
#pragma once
#include <QFileSystemWatcher>
#include <QSaveFile>
#include "panelcontrols.h"
#include "crashsession.h"
#include "interactiondiagnostics.h"
#include "activationbridge.h"
#include "responsiveness.h"
#include "surfacetrace.h"
#include "clipcolorprobe.h"
#include "keyboardprobe.h"
#include "boundarydiagnostics.h"
#include "workflowcommandbridge.h"
#include "timelinetabscrolling.h"

namespace Unbound {
class ControlService : public QObject
{
public:
    explicit ControlService(QObject *parent) : QObject(parent), watcher(this)
    {
        setObjectName(QStringLiteral("unboundControlService"));
        journal=new CrashSession(this);
        new InteractionDiagnostics(this);
        new ResolveActivationBridge(this);
        new ResponsivenessProbe(this);
        new SurfaceTrace(this);
        new ClipColorProbe(this);
        new KeyboardProbe(this);
        if(qEnvironmentVariableIntValue("OPENGATE_TIMELINE_TAB_SCROLLING")==1)new TimelineTabScrolling(this);
        new BoundaryDiagnostics(this);
        directory = qEnvironmentVariable("UNBOUND_LAUNCH_DIRECTORY",
            QDir::homePath() + QStringLiteral("/Library/Logs/Unbound"));
        QDir().mkpath(directory);
        watcher.addPath(directory);
        connect(&watcher, &QFileSystemWatcher::directoryChanged, this, [this] { consumeLaunchRequest(false); });
        #ifdef UNBOUND_FEATURE_MANIFEST
        logEvent("helper.feature-manifest", {{"manifest", QJsonDocument::fromJson(QByteArray(UNBOUND_FEATURE_MANIFEST)).object()}});
#endif
        logEvent(QStringLiteral("helper.start"), {{"executable", QCoreApplication::applicationFilePath()}});
        // Initialize menus, native decorations and saved workspace restoration
        // without putting a second UI in front of Resolve on every launch.
        controls = new PanelControls;
        logEvent("controls.ready",{{"visible",controls->isVisible()},{"startup","native-workspace"}});
        new WorkflowCommandBridge(this,directory,[this](QString id) {
            return controls && controls->invokeWorkflowCommand(id);
        });
        consumeLaunchRequest(true);
        connect(qApp,&QCoreApplication::aboutToQuit,this,[this] { journal->markClean(); });
    }
    ~ControlService() override { delete controls.data(); }
    void showControls()
    {
        if (!controls) controls = new PanelControls;
        controls->showNormal();
        keepOnDisplay(controls);
        controls->raise();
        controls->activateWindow();
        logEvent(QStringLiteral("controls.show"));
    }
private:
    void consumeLaunchRequest(bool startup)
    {
        QFile request(directory+QStringLiteral("/show-controls.request"));
        if (!request.open(QIODevice::ReadOnly)) return;
        const auto age=QFileInfo(request).lastModified().msecsTo(QDateTime::currentDateTimeUtc());
        const auto token=request.readAll(); request.close();
        // Consume the single-use request, including stale files from an older
        // Resolve session. A recent explicit launch may precede helper startup.
        request.remove();
        if (token.isEmpty() || token==lastRequest) return;
        lastRequest=token;
        if (startup && (age<0 || age>15000)) {
            logEvent("controls.launch.stale",{{"age_ms",double(age)}}); return;
        }
        logEvent("controls.launch.request",{{"request",QString::fromUtf8(token)},{"startup",startup}});
        showControls();
    }
    CrashSession *journal=nullptr;
    QFileSystemWatcher watcher;
    QPointer<PanelControls> controls;
    QString directory;
    QByteArray lastRequest;
};
}
