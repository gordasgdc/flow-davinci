// SPDX-License-Identifier: MIT
#pragma once
#include <QPointer>
#include <QWidget>
namespace Unbound {
// Layout ownership changes remain synchronous. Paint only the completed tree,
// then return to the caller's exact update policy, including nested batches.
class LayoutUpdateBatch {
public:
    explicit LayoutUpdateBatch(QWidget *widget) : widget(widget), enabled(widget && widget->updatesEnabled()) {
        if (enabled) widget->setUpdatesEnabled(false);
    }
    ~LayoutUpdateBatch() { if (widget && enabled) widget->setUpdatesEnabled(true); }
    LayoutUpdateBatch(const LayoutUpdateBatch &) = delete;
    LayoutUpdateBatch &operator=(const LayoutUpdateBatch &) = delete;
private:
    QPointer<QWidget> widget;
    bool enabled;
};
}
