// SPDX-License-Identifier: MIT
#pragma once
#include <QAbstractButton>
#include <QHBoxLayout>
#include <QIconEngine>
#include <QPainter>
#include <QPointer>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QToolButton>
#include <functional>
#include "workspacepages.h"
namespace Unbound {
struct NavigationPage { QString id,title;QPointer<QAbstractButton> target;bool current=false; };
class NavigationGlyph : public QIconEngine {
public:
    explicit NavigationGlyph(QString page):page(std::move(page)){}
    QIconEngine *clone() const override{return new NavigationGlyph(page);}
    void paint(QPainter *p,const QRect &r,QIcon::Mode mode,QIcon::State) override {
        p->save();p->setRenderHint(QPainter::Antialiasing);p->translate(r.topLeft());p->scale(r.width()/20.,r.height()/20.);
        p->setPen(QPen(mode==QIcon::Disabled?QColor(95,95,95):QColor(190,190,190),1.4));
        if(page=="Media") {p->drawRect(QRectF(2,3,16,14));p->drawLine(3,14,8,8);p->drawLine(8,8,12,13);p->drawLine(12,13,16,9);}
        else if(page=="Edit" || page=="Cut") {for(int y:{4,9,14})p->drawRect(QRectF(2,y,y==9?11:16,3));if(page=="Cut")p->drawLine(10,1,10,19);}
        else if(page=="Fusion") {p->drawLine(5,5,15,10);p->drawLine(5,15,15,10);for(auto c:{QPoint(5,5),QPoint(5,15),QPoint(15,10)})p->drawEllipse(c,2,2);}
        else if(page=="Color") {p->drawEllipse(QRectF(4,2,10,10));p->drawEllipse(QRectF(1,8,10,10));p->drawEllipse(QRectF(9,8,10,10));}
        else if(page=="Photo") {p->drawRect(QRectF(2,5,16,12));p->drawEllipse(QRectF(7,8,6,6));p->drawLine(5,3,11,3);}
        else if(page=="Fairlight") {p->drawLine(8,4,8,14);p->drawLine(8,4,16,2);p->drawLine(16,2,16,12);p->drawEllipse(QRectF(3,13,5,4));p->drawEllipse(QRectF(11,11,5,4));}
        else {p->drawLine(3,12,3,17);p->drawLine(3,17,17,17);p->drawLine(17,17,17,12);p->drawLine(10,3,10,13);p->drawLine(6,7,10,3);p->drawLine(10,3,14,7);}
        p->restore();
    }
    QPixmap pixmap(const QSize &s,QIcon::Mode m,QIcon::State state) override {QPixmap out(s);out.fill(Qt::transparent);QPainter p(&out);paint(&p,QRect(QPoint(),s),m,state);return out;}
private:QString page;
};
// OpenGate controls mirror native page buttons. The native footer never moves.
class SurfacePageNavigation : public QScrollArea {
public:
    explicit SurfacePageNavigation(QWidget *host,bool nativeSlot=false):QScrollArea(host),nativeSlot(nativeSlot) {
        setObjectName("openGateSurfacePageNavigation");setFrameShape(QFrame::NoFrame);setMinimumWidth(0);
        setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Fixed);setWidgetResizable(true);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        content=new QWidget;row=new QHBoxLayout(content);row->setContentsMargins(4,0,4,0);row->setSpacing(2);setWidget(content);
        timer.setSingleShot(true);connect(&timer,&QTimer::timeout,this,[this]{fit();});
        connect(horizontalScrollBar(),&QScrollBar::rangeChanged,this,[this]{schedule();});
        QString style="QToolButton { border: 0px; border-bottom: 2px solid transparent; border-radius: 0px; background: transparent; padding: 2px 4px; } QToolButton:checked { border-bottom: 2px solid #e53b35; } QToolButton:hover { background: rgba(180,180,180,24); }";
        if(nativeSlot)style+="QScrollBar:horizontal { height: 6px; } QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }";
        setStyleSheet(style);
        setToolTip("Page Navigation — scroll horizontally if all pages do not fit");
    }
    std::function<void(QString)> activate;
    void setNativeSlotGeometry(const QRect &rect){slotHeight=rect.height();setGeometry(rect);schedule();}
    void updatePages(const QList<NavigationPage> &pages) {
        QStringList next;for(const auto &page:pages)next.append(page.id);
        if(next!=ids) {
            while(auto item=row->takeAt(0)){delete item->widget();delete item;}buttons.clear();ids=next;
            for(const auto &page:pages) {
                auto button=new QToolButton(content);button->setObjectName("openGatePage_"+page.id);button->setAutoRaise(true);button->setCheckable(true);button->setFocusPolicy(Qt::NoFocus);button->setIconSize(QSize(18,18));
                row->addWidget(button);buttons.append(button);
                connect(button,&QToolButton::clicked,this,[this,id=page.id]{if(activate)activate(id);});
            }
        }
        for(int i=0;i<pages.size();++i) {
            const auto &page=pages[i];auto button=buttons[i];button->setText(page.title);button->setToolTip(page.title);button->setAccessibleName(page.title);
            button->setEnabled(page.target && page.target->isEnabled());button->setChecked(page.current);
            const auto nativeIcon=page.target?page.target->icon():QIcon();
            const auto key=nativeIcon.isNull()?qulonglong(0):qulonglong(nativeIcon.cacheKey());
            if(!button->property("openGateNativeIconKey").isValid() || button->property("openGateNativeIconKey").toULongLong()!=key) {
                button->setIcon(nativeIcon.isNull()?QIcon(new NavigationGlyph(page.title)):nativeIcon);
                button->setProperty("openGateNativeIconKey",QVariant::fromValue(key));
            }
        }
        schedule();
    }
protected:
    void resizeEvent(QResizeEvent *event) override {QScrollArea::resizeEvent(event);schedule();}
    void changeEvent(QEvent *event) override {QScrollArea::changeEvent(event);if(event->type()==QEvent::FontChange || event->type()==QEvent::StyleChange)schedule();}
private:
    void schedule(){if(!timer.isActive())timer.start(0);}
    void fit() {
        int labeled=8;for(auto button:buttons)labeled+=qMax(36,button->fontMetrics().horizontalAdvance(button->text())+12)+row->spacing();
        const bool labels=(!nativeSlot || slotHeight>=48) && viewport()->width()>=labeled+(compact?24:0);compact=!labels;
        int width=8,height=nativeSlot?0:30;
        for(auto button:buttons) {
            button->setToolButtonStyle(labels?Qt::ToolButtonTextUnderIcon:Qt::ToolButtonIconOnly);
            const auto hint=button->sizeHint();const int buttonHeight=nativeSlot?qMax(12,slotHeight-6):qMax(30,hint.height());
            if(nativeSlot)button->setIconSize(QSize(qMin(18,buttonHeight-4),qMin(18,buttonHeight-4)));
            button->setFixedSize(qMax(labels?button->fontMetrics().horizontalAdvance(button->text())+12:32,hint.width()),buttonHeight);
            width+=button->width()+row->spacing();height=qMax(height,button->height());
        }
        // Explicit content extent keeps every button reachable; Qt must scroll
        // rather than compress native labels or hide the final page.
        content->setMinimumSize(width,height);row->setAlignment(Qt::AlignCenter);
        const int scroll=width>viewport()->width()?horizontalScrollBar()->sizeHint().height():0;
        const int desired=nativeSlot?slotHeight:height+scroll;if(minimumHeight()!=desired || maximumHeight()!=desired)setFixedHeight(desired);
    }
    QWidget *content;QHBoxLayout *row;QList<QToolButton *> buttons;QStringList ids;QTimer timer;bool compact=false,nativeSlot=false;int slotHeight=32;
};
}
