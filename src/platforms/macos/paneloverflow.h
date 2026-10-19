// SPDX-License-Identifier: MIT
#pragma once
#include <QLayout>
#include <QPointer>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include "diagnostics.h"
namespace Unbound {
// A presentation wrapper, not a scale transform. Native control constraints stay
// intact; narrow floating sidebars can scroll to controls outside their width.
class PanelOverflow {
public:
    QWidget *wrap(QWidget *content,QWidget *owner,bool horizontalOnly=false) {
        if (!content || !owner || area) return nullptr;
        area=new QScrollArea(owner); area->setObjectName("unboundPanelOverflow_"+content->objectName());
        area->setFrameShape(QFrame::NoFrame); area->setWidgetResizable(true);
        if (horizontalOnly) area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        area->setMinimumSize(0,0); area->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);
        const bool fill=content->autoFillBackground();
        area->setWidget(content); content->setAutoFillBackground(fill); widget=content;
        const auto id=content->objectName();
        for (auto bar : {area->horizontalScrollBar(),area->verticalScrollBar()}) {
            QObject::connect(bar,&QScrollBar::valueChanged,area,[id,bar](int offset) {
                logEvent("panel.overflow.scroll",{{"panel",id},{"axis",bar->orientation()==Qt::Horizontal ? "horizontal" : "vertical"},{"offset",offset}});
            });
            QObject::connect(bar,&QScrollBar::rangeChanged,area,[id,bar](int minimum,int maximum) {
                logEvent("panel.overflow.range",{{"panel",id},{"axis",bar->orientation()==Qt::Horizontal ? "horizontal" : "vertical"},
                    {"minimum",minimum},{"maximum",maximum},{"overflow",maximum>minimum}});
            });
        }
        logEvent("panel.overflow.attach",{{"panel",id},{"native_minimum_width",content->minimumWidth()}});
        return area;
    }
    bool owns(QWidget *content) const { return content && area && widget==content && area->widget()==content && content->parentWidget()==area->viewport(); }
    QPoint offset() const { return area ? QPoint(area->horizontalScrollBar()->value(),area->verticalScrollBar()->value()) : QPoint(); }
    void restoreOffset(QPoint position) {
        if (!area) return;
        QPointer<QScrollArea> alive=area;
        QTimer::singleShot(0,area,[alive,position] {
            if (!alive) return;
            alive->horizontalScrollBar()->setValue(position.x()); alive->verticalScrollBar()->setValue(position.y());
        });
    }
    bool reclaim(QWidget *content) {
        if (!content || !area || widget!=content) return false;
        if (area->widget()==content) area->takeWidget();
        const bool fill=content->autoFillBackground();
        area->setWidget(content); content->setAutoFillBackground(fill);
        // Reparenting can leave the real control layout at the old native
        // parent's size even after QScrollArea resizes its content widget.
        // Reconcile once at this ownership boundary, not on every drag frame.
        if (content->layout()) { content->layout()->invalidate(); content->layout()->activate(); }
        logEvent("panel.overflow.recover",{{"panel",content->objectName()}});
        return true;
    }
    void releaseInto(QWidget *owner) {
        if (!area || !owner || !owns(widget)) return;
        auto retired=area.data(); auto content=widget.data();
        // takeWidget() reparents to nullptr, briefly creating a top-level native
        // Inspector window and recreating its observed NSWindow. Transfer the
        // real child directly, then destroy the emptied wrapper before it can
        // deliver another layout/scroll event against that child. Dock commands
        // call this outside scroll-area event dispatch; the moved controls live.
        retired->hide(); content->removeEventFilter(retired);
        content->setParent(owner);
        if (auto parent=retired->parentWidget()) if (parent->layout()) parent->layout()->removeWidget(retired);
        if (owner->layout()) owner->layout()->addWidget(content);
        logEvent("panel.overflow.release",{{"panel",content ? content->objectName() : QString()}});
        forget(); delete retired;
    }
    void forget() { area=nullptr; widget=nullptr; }
private:
    QPointer<QScrollArea> area;
    QPointer<QWidget> widget;
};
}
