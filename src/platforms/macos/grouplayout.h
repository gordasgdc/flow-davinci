// SPDX-License-Identifier: MIT
#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

namespace Unbound {
inline bool groupTreeNames(const QJsonObject &node, QStringList &names, int depth = 0)
{
    if (depth > 24 || names.size() > 32) return false;
    if (node.contains("panel")) {
        const auto name = node["panel"].toString();
        if (name.isEmpty() || names.contains(name)) return false;
        names.append(name); return true;
    }
    const auto children = node["children"].toArray();
    if ((node["orientation"] != "horizontal" && node["orientation"] != "vertical") || children.size() < 2 || children.size() > 32) return false;
    for (auto child : children) if (!groupTreeNames(child.toObject(), names, depth + 1)) return false;
    return true;
}
inline QJsonObject splitGroupTree(const QJsonObject &a, const QJsonObject &b, Qt::Orientation orientation, bool before = false)
{
    return {{"orientation", orientation == Qt::Horizontal ? "horizontal" : "vertical"},
        {"children", before ? QJsonArray{b, a} : QJsonArray{a, b}}, {"sizes", QJsonArray{1, 1}}};
}
inline QJsonObject insertGroupTree(QJsonObject node, const QJsonObject &addition, const QString &target, Qt::Orientation orientation, bool before)
{
    if (target.isEmpty() || node.value("panel") == target) return splitGroupTree(node, addition, orientation, before);
    if (node.contains("panel")) return node;
    auto children = node["children"].toArray();
    for (int i = 0; i < children.size(); ++i) children[i] = insertGroupTree(children[i].toObject(), addition, target, orientation, before);
    node["children"] = children; return node;
}
inline QJsonObject removeGroupLeaf(QJsonObject node, const QString &name)
{
    if (node.contains("panel")) return node["panel"] == name ? QJsonObject() : node;
    QJsonArray children, sizes;
    const auto old = node["children"].toArray(), oldSizes = node["sizes"].toArray();
    for (int i = 0; i < old.size(); ++i) {
        const auto child = removeGroupLeaf(old[i].toObject(), name);
        if (!child.isEmpty()) { children.append(child); sizes.append(i < oldSizes.size() ? oldSizes[i] : QJsonValue(1)); }
    }
    if (children.isEmpty()) return {};
    if (children.size() == 1) return children[0].toObject();
    node["children"] = children; node["sizes"] = sizes; return node;
}
}
