// SPDX-License-Identifier: MIT
#pragma once
#include <QFileSystemWatcher>
#include "panelcontrols.h"

namespace Unbound {
class ControlService : public QObject
{
public:
    explicit ControlService(QObject *parent) : QObject(parent), watcher(this)
    {
        setObjectName(QStringLiteral("unboundControlService"));
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
        logEvent(QStringLiteral("helper.start"));
        connect(qApp, &QCoreApplication::aboutToQuit, this, [] { logEvent(QStringLiteral("helper.stop")); });
        showControls();
    }
    ~ControlService() override { delete controls.data(); }
    void showControls()
    {
        if (!controls) controls = new PanelControls;
        controls->showNormal();
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
