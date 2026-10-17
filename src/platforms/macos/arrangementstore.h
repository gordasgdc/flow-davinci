// SPDX-License-Identifier: MIT
#pragma once
#include <QJsonArray>
#include <QJsonParseError>
#include <QSet>
#include <QUuid>
#include "workspacesettings.h"

namespace Unbound {
class ArrangementStore
{
public:
    ArrangementStore() : path(qEnvironmentVariable("UNBOUND_ARRANGEMENTS_FILE",
        QDir::homePath() + QStringLiteral("/Library/Application Support/Unbound/Arrangements.json")))
    {
        QFile file(path);
        if (!file.exists()) return;
        QJsonParseError error;
        if (!file.open(QIODevice::ReadOnly)) { failure = "Cannot read saved arrangements."; return; }
        const auto doc = QJsonDocument::fromJson(file.readAll(), &error);
        if (error.error != QJsonParseError::NoError || !doc.isObject() ||
            doc.object()["version"].toInt() != 1 || !doc.object()["arrangements"].isArray()) {
            failure = "Saved arrangements are invalid or from an unsupported version."; return;
        }
        entries = doc.object()["arrangements"].toArray();
        QSet<QString> ids, names;
        for (const auto &value : entries) {
            const auto entry = value.toObject();
            const auto id = entry["id"].toString(), name = entry["name"].toString().trimmed().toCaseFolded();
            if (id.isEmpty() || name.isEmpty() || ids.contains(id) || names.contains(name) || !entry["workspace"].isObject()) {
                failure = "Saved arrangements contain invalid or duplicate entries."; return;
            }
            ids.insert(id); names.insert(name);
        }
    }
    QJsonArray list() const { return entries; }
    QString error() const { return failure; }
    QJsonObject find(const QString &id) const
    { for (const auto &entry : entries) if (entry.toObject()["id"] == id) return entry.toObject(); return {}; }
    QString create(const QString &name, const QJsonObject &workspace, QString &id)
    {
        const QString problem = validName(name);
        if (!problem.isEmpty()) return problem;
        id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        auto next = entries;
        next.append(QJsonObject{{"id", id}, {"name", name.trimmed()}, {"workspace", workspace}});
        return write(next);
    }
    QString update(const QString &id, const QJsonObject &workspace)
    { return change(id, QString(), workspace, false); }
    QString rename(const QString &id, const QString &name)
    {
        const auto problem = validName(name, id);
        return problem.isEmpty() ? change(id, name.trimmed(), {}, false) : problem;
    }
    QString remove(const QString &id) { return change(id, {}, {}, true); }
    QString reorder(const QStringList &order)
    {
        if (order.size() != entries.size()) return "Arrangement order does not match saved presets.";
        QJsonArray next; QSet<QString> seen;
        for (const auto &id : order) {
            const auto entry = find(id);
            if (seen.contains(id) || entry.isEmpty()) return "Arrangement order contains missing or duplicate presets.";
            seen.insert(id); next.append(entry);
        }
        return write(next);
    }
private:
    QString validName(const QString &name, const QString &except = {}) const
    {
        if (name.trimmed().isEmpty()) return "Enter an arrangement name.";
        for (auto value : entries) {
            auto entry = value.toObject();
            if (entry["id"] != except && entry["name"].toString().compare(name.trimmed(), Qt::CaseInsensitive) == 0)
                return "An arrangement already has that name.";
        }
        return {};
    }
    QString change(const QString &id, const QString &name, const QJsonObject &workspace, bool erase)
    {
        auto next = entries;
        for (int i = 0; i < next.size(); ++i) {
            auto entry = next[i].toObject();
            if (entry["id"] != id) continue;
            if (erase) next.removeAt(i);
            else {
                if (!name.isNull()) entry["name"] = name;
                if (!workspace.isEmpty()) entry["workspace"] = workspace;
                next[i] = entry;
            }
            return write(next);
        }
        return "Arrangement no longer exists.";
    }
    QString write(const QJsonArray &next)
    {
        // Preserve invalid files for recovery instead of overwriting them.
        if (!failure.isEmpty()) return failure;
        if (!QDir().mkpath(QFileInfo(path).absolutePath())) return "Cannot create arrangements folder.";
        QSaveFile file(path);
        const auto bytes = QJsonDocument(QJsonObject{{"version", 1}, {"arrangements", next}}).toJson();
        if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit())
            return "Cannot save arrangements.";
        entries = next; return {};
    }
    QString path, failure;
    QJsonArray entries;
};
}
