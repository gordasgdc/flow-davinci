// SPDX-License-Identifier: MIT
#pragma once
#include <QFileSystemWatcher>
#include <QSaveFile>
#include "panelcontrols.h"
#include "interactiondiagnostics.h"
#include "activationbridge.h"
#include "responsiveness.h"
#include "clipcolorprobe.h"

namespace Unbound {
class ControlService : public QObject
{
public:
    explicit ControlService(QObject *parent) : QObject(parent), watcher(this)
    {
        setObjectName(QStringLiteral("unboundControlService"));
        new InteractionDiagnostics(this);
        new ResolveActivationBridge(this);
        new ResponsivenessProbe(this);
        new ClipColorProbe(this);
        directory = qEnvironmentVariable("UNBOUND_LAUNCH_DIRECTORY",
            QDir::homePath() + QStringLiteral("/Library/Logs/Unbound"));
        QDir().mkpath(directory);
        watcher.addPath(directory);
        connect(&watcher, &QFileSystemWatcher::directoryChanged, this, [this] {
            QFile request(directory + QStringLiteral("/show-controls.request"));
            if (!request.open(QIODevice::ReadOnly)) return;
            const QByteArray token = request.readAll();
            if (token.isEmpty() || token == lastRequest) return;
            lastRequest = token;
            logEvent(QStringLiteral("controls.launch.request"), {{"request", QString::fromUtf8(token)}});
            showControls();
        });
        const QString logDirectory = qEnvironmentVariable("UNBOUND_DIAGNOSTICS_DIR",
            QDir::homePath() + QStringLiteral("/Library/Logs/Unbound"));
        QDir().mkpath(logDirectory);
        const QString markerPath = logDirectory + QStringLiteral("/active-session.json");
        QFile previous(markerPath);
        if (previous.open(QIODevice::ReadOnly)) {
            const auto session = QJsonDocument::fromJson(previous.readAll()).object();
            logEvent(QStringLiteral("helper.previous-session.unclean"), {{"previous", session},
                {"meaning", "No clean shutdown was recorded; this does not establish crash cause."}});
            previous.close();
        }
        QSaveFile marker(markerPath);
        if (marker.open(QIODevice::WriteOnly)) {
            marker.write(QJsonDocument(QJsonObject{{"component", "Unbound"}, {"build", UNBOUND_BUILD_ID},
                {"pid", QCoreApplication::applicationPid()},
                {"started", QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs)}}).toJson());
            marker.commit();
        }
        logEvent(QStringLiteral("helper.start"), {{"executable", QCoreApplication::applicationFilePath()}});
        connect(qApp, &QCoreApplication::aboutToQuit, this, [markerPath] {
            logEvent(QStringLiteral("helper.stop")); QFile::remove(markerPath);
        });
        showControls();
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
    QFileSystemWatcher watcher;
    QPointer<PanelControls> controls;
    QString directory;
    QByteArray lastRequest;
};
}
