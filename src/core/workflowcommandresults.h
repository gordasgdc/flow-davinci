// SPDX-License-Identifier: MIT
#pragma once
#include <QFileSystemWatcher>
#include <QTimer>
#include "diagnostics.h"
namespace Unbound {
// Script completion is diagnostic data only. It can never dispatch a command.
class WorkflowCommandResults : public QObject {
public:
    WorkflowCommandResults(QObject *owner,const QString &launchDirectory)
        :QObject(owner),directory(launchDirectory+"/command-results"),watcher(this) {
        QDir().mkpath(directory);watcher.addPath(directory);
        connect(&watcher,&QFileSystemWatcher::directoryChanged,this,[this] {queue();});
        queue();
    }
private:
    QString directory;QFileSystemWatcher watcher;bool queued=false;
    void queue() {
        if(queued)return;queued=true;
        QTimer::singleShot(0,this,[this] {queued=false;consume();});
    }
    void consume() {
        int count=0;
        for(const auto &info:QDir(directory).entryInfoList({"*.json"},QDir::Files|QDir::NoSymLinks,QDir::Time|QDir::Reversed)) {
            if(++count>16){queue();break;}
            QFile file(info.absoluteFilePath());if(!file.open(QIODevice::ReadOnly))continue;
            const auto bytes=file.read(4097);file.close();if(!file.remove())continue;
            const auto result=QJsonDocument::fromJson(bytes).object();
            const auto age=QDateTime::currentMSecsSinceEpoch()-result["issued_ms"].toDouble(-1);
            if(bytes.size()>4096 || result["command"]!="edit.inspector.stabilize" ||
               !result["success"].isBool() || !qIsFinite(age) || age<0 || age>15000)continue;
            logEvent("command.edit-target.result",{{"command","stabilize"},{"success",result["success"].toBool()},
                {"completion","selected-timeline-api-returned"},{"method",result["method"].toString().left(64)},
                {"reason",result["reason"].toString().left(128)},{"error_type",result["error_type"].toString().left(64)},
                {"attempted",result["attempted"].toInt()},{"completed",result["completed"].toInt()}});
        }
    }
};
}
