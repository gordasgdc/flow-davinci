// SPDX-License-Identifier: MIT
#pragma once
#include <QObject>
#include <QPointer>
#include <QVBoxLayout>
#include "diagnostics.h"
namespace Unbound {
// Lease a page's layout as one unit, keeping its native slot graph intact for
// restoration. Only a custom arrangement occupies the page while the lease is
// active. This does not create, decorate or hide the main native window.
class NativeRootLayout : public QObject {
public:
    explicit NativeRootLayout(QObject *owner=nullptr) : QObject(owner) {}
    ~NativeRootLayout() override { restore(); }
    bool active() const { return page && nativeLayout && customLayout; }
    static bool contains(QWidget *nativePage,QWidget *container) {
        if (!nativePage || !container || container==nativePage) return false;
        for (auto ancestor=container; ancestor && ancestor!=nativePage; ancestor=ancestor->parentWidget())
            if (ancestor->parentWidget()==nativePage && ancestor->property("unboundNativeArrangementRoot").toBool())
                return nativePage->layout() && nativePage->layout()->indexOf(ancestor)>=0;
        return false;
    }
    QString mount(QWidget *nativePage,QWidget *arrangement) {
        if (page || !nativePage || !nativePage->layout() || !arrangement || arrangement->isWindow() ||
            arrangement==nativePage || arrangement->isAncestorOf(nativePage))
            return QStringLiteral("The native workspace layout is not available.");
        page=nativePage; surface=arrangement; main=page->window(); nativeLayout=page->layout();
        pageDestroyed=connect(page,&QObject::destroyed,this,[this] {
            if (stash) stash->deleteLater();
            clear(); logEvent("workspace.native-root.owner-destroyed");
        });
        minimum=page->minimumSize(); policy=page->sizePolicy();
        const auto margins=nativeLayout->contentsMargins();
        stash=new QWidget(main); stash->setObjectName("unboundNativeLayoutStash"); stash->hide();
        stash->setLayout(nativeLayout);
        if (page->layout() || stash->layout()!=nativeLayout) {
            if (!page->layout() && stash->layout()==nativeLayout) page->setLayout(nativeLayout);
            // A native callback may have installed a different page layout.
            // Retain our parked native tree rather than deleting its controls.
            if (page->layout()==nativeLayout) { delete stash.data(); clear(); }
            else logEvent("workspace.native-root.mount-rejected",{{"reason","native-layout-changed-during-transfer"}});
            return QStringLiteral("Could not preserve the native workspace layout.");
        }
        // Preserve visibility of unmanaged direct children too; native custom
        // widgets are not necessarily represented by a QLayoutItem.
        for (auto child : page->findChildren<QWidget *>(QString(),Qt::FindDirectChildrenOnly))
            if (child!=surface && !child->isWindow()) { visibility.append({child,child->isHidden()}); child->hide(); }
        customLayout=new QVBoxLayout(page); customLayout->setContentsMargins(margins); customLayout->setSpacing(0);
        customLayout->addWidget(surface);
        rootProperty=surface->property("unboundNativeArrangementRoot");
        surface->setProperty("unboundNativeArrangementRoot",true);
        page->setMinimumSize(0,0); surface->show();
        logEvent("workspace.native-root.mount",{{"page",page->objectName()},{"native_layout",nativeLayout->metaObject()->className()}});
        return {};
    }
    bool restore() {
        if (!page) return true;
        if (!nativeLayout || !stash || stash->layout()!=nativeLayout || page->layout()!=customLayout) {
            logEvent("workspace.native-root.restore-rejected",{{"reason","native-or-custom-layout-replaced"}});
            return false;
        }
        // Park our tree before returning the original layout. Never delete a
        // native control merely because its presentation container is retiring.
        auto parking=new QWidget(main); parking->hide(); parking->setLayout(customLayout);
        page->setLayout(nativeLayout);
        if (page->layout()!=nativeLayout) {
            if (!page->layout()) page->setLayout(customLayout);
            if (surface && parking->isAncestorOf(surface)) {
                if (parking->layout()) parking->layout()->removeWidget(surface);
                surface->setProperty("unboundNativeArrangementRoot",rootProperty);
                surface->setParent(main); surface->hide();
            }
            delete parking;
            logEvent("workspace.native-root.restore-rejected",{{"reason","native-layout-transfer-failed"}});
            return false;
        }
        if (surface) {
            if (parking->layout()) parking->layout()->removeWidget(surface);
            surface->setProperty("unboundNativeArrangementRoot",rootProperty);
            surface->setParent(main); surface->hide();
        }
        page->setMinimumSize(minimum); page->setSizePolicy(policy);
        for (const auto &item : visibility)
            if (item.first && item.first->parentWidget()==page) item.first->setVisible(!item.second);
        disconnect(pageDestroyed);
        delete parking; delete stash.data();
        logEvent("workspace.native-root.restore",{{"page",page->objectName()},{"native_layout_restored",true}});
        clear(); return true;
    }
private:
    void clear() {
        disconnect(pageDestroyed); pageDestroyed={};
        page=nullptr; main=nullptr; surface=nullptr; stash=nullptr; nativeLayout=nullptr; customLayout=nullptr; visibility.clear();
    }
    QPointer<QWidget> page,main,surface,stash;
    QPointer<QLayout> nativeLayout,customLayout;
    QList<QPair<QPointer<QWidget>,bool>> visibility;
    QMetaObject::Connection pageDestroyed;
    QSize minimum; QSizePolicy policy;
    QVariant rootProperty;
};
}
