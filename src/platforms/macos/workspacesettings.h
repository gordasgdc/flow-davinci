// SPDX-License-Identifier: MIT
#pragma once
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

namespace Unbound {
class WorkspaceSettings
{
public:
    WorkspaceSettings() : path(qEnvironmentVariable("UNBOUND_WORKSPACE_FILE",
        QDir::homePath() + QStringLiteral("/Library/Application Support/Unbound/Workspace.json")))
    {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly)) data = QJsonDocument::fromJson(file.readAll()).object();
    }
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
