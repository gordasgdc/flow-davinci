// SPDX-License-Identifier: MIT
#pragma once
#include <QHBoxLayout>
#include <QMenu>
#include <QToolButton>
#include <functional>
namespace Unbound {
inline void populatePanelScaleMenu(QMenu *menu,QObject *owner,std::function<double()> value,std::function<void(double)> set) {
    menu->setProperty("unboundOwnMenu",true);
    const QPair<const char *,double> changes[]={{"Smaller",-0.1},{"Larger",0.1},{"Reset to 100%",0.0}};
    for (const auto &change:changes) {
        auto action=menu->addAction(QString::fromLatin1(change.first));
        QObject::connect(action,&QAction::triggered,owner,[value,set,delta=change.second] {
            set(delta==0.0 ? 1.0 : qBound(0.7,qRound((value()+delta)*10.0)/10.0,1.5));
        });
    }
    menu->addSeparator();
    for (int percent:{70,80,90,100,110,120,130,140,150}) {
        auto action=menu->addAction(QString::number(percent)+"%"); action->setCheckable(true);
        action->setChecked(qAbs(value()-percent/100.)<0.001);
        QObject::connect(action,&QAction::triggered,owner,[set,percent] { set(percent/100.); });
    }
}
class PanelScaleControls : public QWidget {
public:
    PanelScaleControls(QWidget *parent,std::function<double()> value,std::function<void(double)> set,
                       std::function<bool()> available=[] { return true; }) : QWidget(parent) {
        setObjectName("openGatePanelScale"); setProperty("openGateChrome",true);
        auto row=new QHBoxLayout(this); row->setContentsMargins(0,0,0,0); row->setSpacing(0);
        auto minus=new QToolButton(this),plus=new QToolButton(this),reset=new QToolButton(this);
        minus->setText(QString::fromUtf8("−")); plus->setText("+"); reset->setText("UI");
        minus->setToolTip("Smaller panel interface"); plus->setToolTip("Larger panel interface"); reset->setToolTip("Panel interface size (experimental); reset to 100%");
        for (auto button:{minus,reset,plus}) { button->setAutoRaise(true); button->setFocusPolicy(Qt::NoFocus); row->addWidget(button); }
        connect(minus,&QToolButton::clicked,this,[value,set,available] { if (available()) set(qMax(0.7,qRound((value()-0.1)*10)/10.)); });
        connect(plus,&QToolButton::clicked,this,[value,set,available] { if (available()) set(qMin(1.5,qRound((value()+0.1)*10)/10.)); });
        connect(reset,&QToolButton::clicked,this,[set,available] { if (available()) set(1.0); });
        auto menu=new QMenu(reset); menu->setProperty("unboundOwnMenu",true);
        connect(menu,&QMenu::aboutToShow,this,[menu,this,value,set,available] {
            menu->clear(); populatePanelScaleMenu(menu,this,value,[set,available](double factor) { if (available()) set(factor); });
        });
        reset->setMenu(menu); reset->setPopupMode(QToolButton::MenuButtonPopup);
    }
};
}
