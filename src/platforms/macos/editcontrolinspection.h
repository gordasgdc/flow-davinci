// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractButton>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsObject>
#include <QGraphicsProxyWidget>
#include <QMenu>
#include <QWidgetAction>
#include "diagnostics.h"
namespace Unbound {
// Read only, bounded inspection of the two native command surfaces. Never log
// editor contents or inspect unrelated menus/project controls.
inline QJsonObject editControlIdentity(QWidget *widget) {
    QJsonObject result{{"name",widget->objectName()},{"class",widget->metaObject()->className()},
        {"enabled",widget->isEnabled()},{"visible",widget->isVisible()}};
    if (auto label=qobject_cast<QLabel *>(widget)) result["label"]=label->text();
    if (auto button=qobject_cast<QAbstractButton *>(widget)) result["label"]=button->text();
    return result;
}
inline void inspectTimelineOptions(QMenu *menu) {
    if (!menu || !menu->parentWidget() || menu->parentWidget()->objectName()!="btnEditToolbarPreferences") return;
    QJsonArray actions;
    for (auto action:menu->actions()) {
        if (actions.size()>=64) break;
        QJsonObject record{{"name",action->objectName()},{"label",action->text()},
            {"class",action->metaObject()->className()},{"enabled",action->isEnabled()},
            {"checkable",action->isCheckable()},{"checked",action->isChecked()}};
        if (auto custom=qobject_cast<QWidgetAction *>(action)) if (auto contents=custom->defaultWidget()) {
            record["contents"]=editControlIdentity(contents); QJsonArray children;
            for (auto child:contents->findChildren<QWidget *>()) {
                if (children.size()>=32) break;
                children.append(editControlIdentity(child));
            }
            record["children"]=children;
        }
        actions.append(record);
    }
    logEvent("command.edit-target.timeline-options",{{"actions",actions}});
}
inline void inspectStabilizationControls() {
    QJsonArray candidates;
    for (auto widget:QApplication::allWidgets()) {
        auto button=qobject_cast<QAbstractButton *>(widget);
        if (!button || (!button->text().contains("stabil",Qt::CaseInsensitive) &&
            !button->objectName().contains("stabil",Qt::CaseInsensitive))) continue;
        if (candidates.size()>=32) break;
        QJsonArray ancestors;
        for (auto parent=button->parentWidget();parent && ancestors.size()<16;parent=parent->parentWidget())
            ancestors.append(QJsonObject{{"name",parent->objectName()},{"class",parent->metaObject()->className()}});
        auto identity=editControlIdentity(button); identity["ancestors"]=ancestors;
        candidates.append(identity);
    }
    logEvent("command.edit-target.stabilization-controls",{{"candidates",candidates}});
    QJsonArray surfaces;
    for (auto widget:QApplication::allWidgets()) {
        if (widget->objectName()!="widgetEditInspector") continue;
        for (auto view:widget->findChildren<QGraphicsView *>()) {
            if (!view->scene()) continue;
            QJsonArray items;
            for (auto item:view->scene()->items()) {
                if (items.size()>=128) break;
                auto object=item->toGraphicsObject();
                if (!object) continue;
                QJsonObject record{{"class",object->metaObject()->className()},
                    {"name",object->objectName()},{"visible",item->isVisible()},{"enabled",item->isEnabled()}};
                if (auto proxy=qobject_cast<QGraphicsProxyWidget *>(object)) if (proxy->widget())
                    record["widget"]=editControlIdentity(proxy->widget());
                items.append(record);
            }
            surfaces.append(QJsonObject{{"view",editControlIdentity(view)},{"items",items}});
        }
    }
    logEvent("command.edit-target.inspector-graphics",{{"surfaces",surfaces}});
}
}
