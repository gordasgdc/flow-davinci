// SPDX-License-Identifier: MIT
#pragma once
#include "diagnostics.h"
#include "nativepanelposition.h"
#include <QScrollArea>
#include <QScrollBar>

namespace Unbound {
// Attach real native content to an owner's layout while preserving its original
// slot. Qt parentage makes it travel with the owner; no geometry-follow timer.
class PanelAccessoryLease : public QObject {
public:
    explicit PanelAccessoryLease(QObject *owner) : QObject(owner) {}
    ~PanelAccessoryLease() override { release(); }
    bool active() const { return position.active() && viewport; }
    QWidget *ownerSurface() const { return destination; }
    QString attach(QWidget *accessory,QWidget *nativeRoot,QWidget *owner, Qt::Edge edge=Qt::BottomEdge)
    {
        if (active()) return destination==owner ? QString() : QStringLiteral("Detach the accessory before changing its owner.");
        if (viewport || position.content()) return QStringLiteral("The previous accessory position changed; restoration is required.");
        auto layout=owner ? qobject_cast<QBoxLayout *>(owner->layout()) : nullptr;
        if ((edge!=Qt::TopEdge && edge!=Qt::BottomEdge) || !layout || layout->direction()!=QBoxLayout::TopToBottom || !accessory || accessory==owner || accessory->isAncestorOf(owner))
            return QStringLiteral("This panel does not have a supported reserved-band layout.");
        const int height=qMax(32,accessory->sizeHint().height());
        const auto error=position.acquire(accessory,nativeRoot);
        if (!error.isEmpty()) return error;
        content=accessory; destination=owner; destinationLayout=layout;
        originalFill=accessory->autoFillBackground();
        position.surface()->hide();
        viewport=new QScrollArea(owner); viewport->setObjectName("unboundAttachedAccessory_"+accessory->objectName());
        viewport->setFrameShape(QFrame::NoFrame); viewport->setWidgetResizable(false);
        viewport->setMinimumWidth(0); viewport->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        viewport->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        viewport->setWidget(accessory); accessory->show(); if(edge==Qt::TopEdge)layout->insertWidget(0,viewport,0);else layout->addWidget(viewport,0);
        rowHeight=height;
        connect(viewport->horizontalScrollBar(),&QScrollBar::rangeChanged,this,[this](int,int) { updateHeight(); });
        ownerDestroyed=connect(owner,&QObject::destroyed,this,[this] { releaseImpl(true); });
        updateHeight(); viewport->show();
        logEvent("accessory.attach",{{"panel",accessory->objectName()},{"owner",owner->objectName()},{"edge",edge==Qt::TopEdge?"top":"bottom"}});
        return {};
    }
    bool release() { return releaseImpl(false); }
private:
    bool releaseImpl(bool ownerDying)
    {
        if (!active()) return !viewport && !position.content();
        if (!content || viewport->widget()!=content || content->parentWidget()!=viewport->viewport() ||
            (!ownerDying && (!destination || viewport->parentWidget()!=destination || !destinationLayout || destinationLayout->indexOf(viewport)<0))) {
            logEvent("accessory.release.rejected",{{"reason","attachment-ownership-changed"}}); return false;
        }
        auto widget=viewport->takeWidget(); widget->setParent(position.contentParking()); widget->hide();
        if (!position.restore()) { viewport->setWidget(widget); widget->show(); return false; }
        widget->setAutoFillBackground(originalFill);
        disconnect(ownerDestroyed); if (destinationLayout) destinationLayout->removeWidget(viewport);
        delete viewport.data(); viewport=nullptr; destination=nullptr; destinationLayout=nullptr; content=nullptr;
        logEvent("accessory.release",{{"panel",widget->objectName()}});
        return true;
    }
public:
    void setHidden(bool hidden) { if (viewport) viewport->setVisible(!hidden); }
    bool hidden() const { return viewport && viewport->isHidden(); }
private:
    void updateHeight()
    {
        if (!viewport) return;
        const auto bar=viewport->horizontalScrollBar();
        viewport->setFixedHeight(rowHeight+(bar->maximum()>bar->minimum() ? bar->sizeHint().height() : 0));
    }
    NativePanelPosition position;
    QPointer<QWidget> content,destination;
    QPointer<QBoxLayout> destinationLayout;
    QPointer<QScrollArea> viewport;
    QMetaObject::Connection ownerDestroyed;
    bool originalFill=false;
    int rowHeight=32;
};
}
