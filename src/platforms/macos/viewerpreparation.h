// SPDX-License-Identifier: MIT
#pragma once
#include <QAction>
#include <QWidget>
#include "diagnostics.h"
namespace Unbound {
inline bool prepareSourceViewer(QWidget *page, QWidget *source)
{
    if (!page || !source || source->isVisible()) return false;
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
