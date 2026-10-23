// SPDX-License-Identifier: MIT
#pragma once
#include <QLayout>
#include <QWidget>
#include <QVariant>
namespace Unbound {
// The workspace already lays out its visible bands. Use its actual content
// rectangle rather than reconstructing screen-wide gates or native constraints.
inline QRect workspaceViewportBounds(QWidget *owner,QWidget *nativeBody=nullptr) {
    if(!owner || !owner->isVisible())return {};
    if(nativeBody && nativeBody->window()==owner && nativeBody->isVisible()) {
        const auto content=workspaceViewportBounds(nativeBody);
        return content.isEmpty()?QRect(nativeBody->mapToGlobal(QPoint()),nativeBody->size()):content;
    }
    // A native controller can remain between the page and its managed body.
    // Its outer rectangle includes the reserved native strip; maximize uses the
    // actual arranged viewport, just as it does for a direct page-mounted body.
    for (auto body:owner->findChildren<QWidget *>("unboundNativeArrangement"))
        if (body->property("unboundNativeArrangementRoot").toBool() &&
            body->property("unboundNativeArrangementPage").value<QObject *>()==owner && body->isVisibleTo(owner))
            return QRect(body->mapToGlobal(QPoint()),body->size());
    auto layout=owner->layout();if(!layout)return {};
    QRect content;
    for(int i=0;i<layout->count();++i) {
        auto item=layout->itemAt(i);auto widget=item->widget();
        if(widget) {
            const auto name=widget->objectName();
            if(!widget->isVisibleTo(owner) || name.startsWith("unboundAttachedAccessory_") ||
                name=="openGateNativeFooterMirror")continue;
        } else if(!item->layout())continue;
        content=content.united(item->geometry());
    }
    content=content.intersected(owner->contentsRect());
    return content.isEmpty()?QRect():QRect(owner->mapToGlobal(content.topLeft()),content.size());
}
}
