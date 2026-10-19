// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractButton>
#include <QLabel>
#include <QMenu>
#include <QWidgetAction>
namespace Unbound {
inline QString nativeEditLabel(QString value) {
    value=value.section('\t',0,0); value.remove('&'); return value.trimmed();
}
inline bool nativeEditActionLabel(QAction *action,const QString &expected) {
    if (nativeEditLabel(action->text())==expected) return true;
    auto custom=qobject_cast<QWidgetAction *>(action);
    auto contents=custom ? custom->defaultWidget() : nullptr;
    if (!contents) return false;
    QList<QWidget *> widgets{contents}; widgets.append(contents->findChildren<QWidget *>());
    for (auto widget:widgets) {
        if (auto text=qobject_cast<QLabel *>(widget)) if (nativeEditLabel(text->text())==expected) return true;
        if (auto button=qobject_cast<QAbstractButton *>(widget)) if (nativeEditLabel(button->text())==expected) return true;
    }
    return false;
}
inline bool nativeTimelineOptionsMenu(QMenu *menu) {
    if (menu->property("unboundOwnMenu").toBool()) return false;
    // The native popup's owner is proven by the Resolve click trace. Its rows
    // may be QWidgetActions with empty action text and embedded labels.
    if (menu->parentWidget() && menu->parentWidget()->objectName()=="btnEditToolbarPreferences") return true;
    for (auto action:menu->actions()) if (nativeEditActionLabel(action,"Display Stacked Timelines")) return true;
    return false;
}
}
