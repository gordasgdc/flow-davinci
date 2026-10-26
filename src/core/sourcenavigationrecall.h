// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractItemView>
#include <QMouseEvent>
#include <QPointer>
#include <QTimer>
#include <functional>
#include "diagnostics.h"

namespace Unbound {
// Recognize native Media Pool navigation and request the existing Source door
// after native dispatch. Owns no placement, visibility or native command state.
class SourceNavigationRecall : public QObject {
public:
    struct Context {
        QString blocked;
        QPointer<QWidget> page,pool;
        QPointer<QObject> source;
        unsigned version=0;
        bool shown=false;
    };
    explicit SourceNavigationRecall(QObject *owner):QObject(owner) {}
    std::function<Context()> context;
    std::function<void(QObject *)> reveal;
    void observe(QObject *object,QMouseEvent *event) {
        auto viewport=qobject_cast<QWidget *>(object);
        auto parent=viewport ? viewport->parentWidget() : nullptr;
        if(!parent || parent->objectName()!="mediaPoolClipView")return;
        auto reject=[&](const QString &reason) {
            logEvent("source.navigation.skipped",{{"reason",reason},{"view_class",parent->metaObject()->className()}});
        };
        if(event->button()!=Qt::LeftButton){reject("not-left-button");return;}
        const auto before=context();
        if(!before.blocked.isEmpty()){reject(before.blocked);return;}
        auto view=qobject_cast<QAbstractItemView *>(parent);
        if(!view){reject("not-abstract-item-view");return;}
        if(view->viewport()!=viewport){reject("not-clip-viewport");return;}
        if(!view->indexAt(event->pos()).isValid()){reject("no-item-at-pointer");return;}
        if(!before.source){reject("source-not-leased");return;}
        if(!before.pool || !before.pool->isAncestorOf(view)){reject("pool-outside-managed-owner");return;}
        if(before.shown){reject("source-already-presented");return;}
        logEvent("source.navigation.queued");
        QTimer::singleShot(0,this,[this,before] {
            const auto now=context();
            QString reason=now.blocked;
            if(reason.isEmpty() && (!before.page || before.page!=now.page || before.version!=now.version ||
               !before.source || before.source!=now.source))reason="ownership-or-transaction-changed";
            if(reason.isEmpty() && now.shown)reason="source-already-presented";
            if(!reason.isEmpty()){logEvent("source.navigation.skipped",{{"reason",reason},{"phase","after-native-dispatch"}});return;}
            reveal(now.source);
            logEvent("source.navigation.recall",{{"reason","media-pool-double-click"}});
        });
    }
};
}
