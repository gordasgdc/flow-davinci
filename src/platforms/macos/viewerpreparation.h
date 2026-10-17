// SPDX-License-Identifier: MIT
#pragma once
#include <QAction>
#include <QWidget>
#include <QStackedWidget>
#include "diagnostics.h"
namespace Unbound {
inline QWidget *sourceViewerSurface(QWidget *source)
{
    if (!source) return nullptr;
    auto stack = qobject_cast<QStackedWidget *>(source->parentWidget());
    return stack && stack->objectName() == QLatin1String("stackedEditSourceOfflinePlayers") ? stack : source;
}
inline bool prepareSourceViewer(QWidget *page, QWidget *source)
{
    if (!page || !source || sourceViewerSurface(source)->isVisible()) return false;
    for (auto action : page->window()->findChildren<QAction *>()) {
        QString text = action->text(); text.remove('&');
        if (text.trimmed() != QLatin1String("Single Viewer Mode") || !action->isCheckable() ||
            !action->isChecked() || !action->isEnabled()) continue;
        logEvent(QStringLiteral("source.prepare.dual-viewer"), {{"action", action->objectName()}});
        action->trigger();
        return true;
    }
    logEvent(QStringLiteral("source.prepare.unavailable"), {{"source", widgetState(source)}});
    return false;
}
}
