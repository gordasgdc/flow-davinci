// SPDX-License-Identifier: MIT
#pragma once
#include "settingsprofile.h"
#include "../../core/pageworkspacestate.h"
#include "starterworkspacegeometry.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QRect>

namespace Unbound {
class WorkspaceSettings
{
public:
    WorkspaceSettings() : path(qEnvironmentVariable("UNBOUND_WORKSPACE_FILE",
        settingsProfilePath("Workspace.json")))
    {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) data = QJsonDocument::fromJson(file.readAll()).object();
        data=fitStarterWorkspace(data);
        // Recover the last complete page snapshot, including if an earlier
        // process ended during an intermediate native restoration save.
        if(data["page_workspaces"].toObject().contains("edit")) {
            const auto pages=data["page_workspaces"].toObject();
            const auto edit=PageWorkspaceState::view(data,"edit");
            if(!edit.isEmpty()){data=edit;data["page_workspaces"]=pages;}
        }
    }
    QJsonObject pageView(const QString &page) const {return PageWorkspaceState::view(data,page);}
    QString recordPage(const QString &page,const QJsonObject &value) {return PageWorkspaceState::record(data,page,value);}
    bool save() const
    {
        if (!QDir().mkpath(QFileInfo(path).absolutePath())) return false;
        QSaveFile file(path);
        if (!file.open(QIODevice::WriteOnly)) return false;
        const QByteArray bytes = QJsonDocument(data).toJson();
        return file.write(bytes) == bytes.size() && file.commit();
    }
    static QJsonObject rectangle(const QRect &r)
    {
        return {{"x", r.x()}, {"y", r.y()}, {"width", r.width()}, {"height", r.height()}};
    }
    static QRect rectangle(const QJsonObject &object)
    {
        return {object["x"].toInt(), object["y"].toInt(), object["width"].toInt(), object["height"].toInt()};
    }
    QJsonObject data;
private:
    QString path;
};
}
