// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QAbstractButton>
#include <QMouseEvent>
#include <QPointer>
#include <functional>
#include <utility>
namespace Unbound {
// Shared click/drag distinction, with no platform window APIs or layout policy.
// Native clicks pass through; only a held left-button threshold starts a drag.
class PanelTitleGesture : public QObject {
public:
    PanelTitleGesture(QWidget *title,QObject *owner,std::function<void(QPoint)> move)
        :QObject(owner),title(title),move(std::move(move)) {title->installEventFilter(this);}
    ~PanelTitleGesture() override {if(title)title->removeEventFilter(this);}
protected:
    bool eventFilter(QObject *,QEvent *event) override {
        if(event->type()==QEvent::MouseButtonPress) {
            auto mouse=static_cast<QMouseEvent *>(event);armed=mouse->button()==Qt::LeftButton;dragged=false;press=mouse->globalPos();
        } else if(event->type()==QEvent::MouseMove && armed) {
            auto mouse=static_cast<QMouseEvent *>(event);
            if(mouse->buttons().testFlag(Qt::LeftButton) && (mouse->globalPos()-press).manhattanLength()>=QApplication::startDragDistance()) {
                armed=false;dragged=true;
                if(auto button=qobject_cast<QAbstractButton *>(title.data()))button->setDown(false);
                if(QWidget::mouseGrabber()==title)title->releaseMouse();
                if(move)move(mouse->globalPos());return true;
            }
        } else if(event->type()==QEvent::MouseButtonRelease) {armed=false;const bool consume=dragged;dragged=false;return consume;}
        else if(event->type()==QEvent::Hide || event->type()==QEvent::UngrabMouse)armed=false;
        return false;
    }
private:
    QPointer<QWidget> title;std::function<void(QPoint)> move;QPoint press;bool armed=false,dragged=false;
};
}
