// SPDX-License-Identifier: MIT
#pragma once
#include <QJsonObject>
#include <QString>
namespace Unbound {
inline QString workspacePage(const QJsonObject &entry)
{
    return entry.value("page").toString(entry.value("workspace").toObject().value("page").toString("edit"));
}
inline QString workspacePageLabel(const QString &page)
{
    if (page == "media") return "Media";
    if (page == "cut") return "Cut";
    if (page == "edit") return "Edit";
    if (page == "fusion") return "Fusion";
    if (page == "color") return "Color";
    if (page == "fairlight") return "Fairlight";
    if (page == "deliver") return "Deliver";
    if (page == "photo") return "Photo";
    return {};
}
}
