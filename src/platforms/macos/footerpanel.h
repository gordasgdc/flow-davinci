// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QAction>
#include <QBoxLayout>
#include <QJsonObject>
#include <QMenu>
#include <QPointer>
#include <QSet>
#include <QScopedValueRollback>
#include <memory>
#include <QTimer>
#include "diagnostics.h"
#include "pagecontext.h"
#include "surfacepagenavigation.h"
#include "panelaccessorylease.h"
namespace Unbound {
// Global visibility is Resolve's command. Surface placement is OpenGate state.
// The original complete footer occupies the primary surface through a reversible
// bottom-band lease; secondary surfaces invoke the original native buttons.
class FooterPanel : public QObject {
public:
    FooterPanel(QObject *owner,std::function<QWidget *()> mainWindow,QJsonObject saved)
        :QObject(owner),nativeLease(this),root(std::move(mainWindow)),state(std::move(saved)) {
        timer.setSingleShot(true);timer.setInterval(40);connect(&timer,&QTimer::timeout,this,[this]{refresh();});
        pendingVisibility=state.contains("hidden");qApp->installEventFilter(this);schedule();
    }
    ~FooterPanel() override {shutdown();}
    std::function<void()> changed;
    std::function<QWidget *()> primarySurface;
    std::function<void(bool,bool)> placementChanged;
    std::function<QList<QWidget *>()> surfaceHosts;
    void shutdown() {
        stopped=true;timer.stop();qApp->removeEventFilter(this);nativeLease.release();
        for(auto view:views)if(view)delete view;views.clear();
        if(nativeFooter){QScopedValueRollback<bool> guard(applyingPlacement,true);nativeFooter->setVisible(nativeVisibility?nativeVisibility->isChecked():shown);}
    }
    bool releaseNative(){return nativeLease.release();}
    void refreshSurfaces(){refresh();}
    void restore(){refresh();}
    void wireMenu(){refresh();}
    QString pinPrimarySurface(bool enabled) {
        auto host=primarySurface?primarySurface():nullptr;
        if(!host)return QStringLiteral("No workspace surface is available yet.");
        setPinned(host,enabled);return {};
    }
    QString showPanel(){return requestVisibility(true);}
    QString hidePanel(){return requestVisibility(false);}
    QString toggleVisibility(){refresh();return requestVisibility(!shown);}
    QJsonObject snapshot() const {
        auto value=state;value["model"]="surface-v1";value["floating"]=false;value["attachment"]="surfaces";value["hidden"]=!shown;return value;
    }
    void setState(const QJsonObject &value) {
        state=value;pendingVisibility=value.contains("hidden");refresh();
    }
    bool pinned(QWidget *host) const {
        if(!host)return false;const auto choices=state["surfaces"].toObject();
        return choices.contains(host->objectName())?choices[host->objectName()].toBool():
            host==(root?root():nullptr) || (primarySurface && primarySurface()==host);
    }
    void setPinned(QWidget *host,bool enabled) {
        if(!host || host->objectName().isEmpty())return;
        logEvent("footer.placement.request",{{"surface",host->objectName()},{"pinned",enabled}});
        auto choices=state["surfaces"].toObject();choices[host->objectName()]=enabled;state["surfaces"]=choices;refresh();publish();
    }
    void populatePlacementMenu(QMenu *menu,QWidget *host) {
        auto placement=menu->addMenu("Page Navigation Placement");placement->setProperty("unboundOwnMenu",true);
        auto pin=placement->addAction("Pin to This Surface");pin->setCheckable(true);pin->setChecked(pinned(host));
        QPointer<QWidget> retained(host);connect(pin,&QAction::triggered,this,[this,retained](bool value){defer([this,retained,value]{if(retained)setPinned(retained,value);});});
    }
    void populateSurfaceMenu(QMenu *menu,QWidget *host) {
        auto navigation=menu->addMenu("Page Navigation");navigation->setProperty("unboundOwnMenu",true);
        auto visibility=navigation->addAction("Show Page Navigation");visibility->setProperty("openGateNavigationProxy",true);visibility->setCheckable(true);visibility->setChecked(shown);
        connect(visibility,&QAction::triggered,this,[this](bool value){defer([this,value]{requestVisibility(value);});});
        auto placement=navigation->addAction("Pin to This Surface");placement->setToolTip("Reserve a bottom band for Page Navigation on this surface");placement->setCheckable(true);placement->setChecked(pinned(host));placement->setEnabled(host && !host->property("unboundToolbarStrip").toBool());
        QPointer<QWidget> retained(host);connect(placement,&QAction::triggered,this,[this,retained](bool value){defer([this,retained,value]{if(retained)setPinned(retained,value);});});
    }
protected:
    bool eventFilter(QObject *object,QEvent *event) override {
        const auto type=event->type();
        if(type==QEvent::Show || type==QEvent::Hide || type==QEvent::Resize || type==QEvent::LayoutRequest) {
            auto widget=qobject_cast<QWidget *>(object);
            if(widget && widget==nativeFooter && !nativeLease.active() && !applyingPlacement && !nativeVisibility && initialized && (type==QEvent::Show || type==QEvent::Hide)) {
                shown=!nativeFooter->isHidden();publish();
            }
            if(widget && (widget==nativeFooter || widget->objectName()=="m_PageSwitcherContainer" || widget->objectName()=="frameControlPanelMiddle" || widget->isWindow() || widget->objectName().startsWith("WStackPage_") || widget->objectName()=="workspaceShowPanel"))schedule();
        }
        return false;
    }
private:
    void schedule(){if(!timer.isActive())timer.start();}
    void publish(){state["hidden"]=!shown;if(changed)changed();}
    void defer(std::function<void()> operation) {
        auto wait=new QTimer(this);wait->setSingleShot(true);auto attempts=std::make_shared<int>(0);
        connect(wait,&QTimer::timeout,this,[wait,attempts,operation] {
            if(QApplication::activePopupWidget() && ++*attempts<600){wait->start(25);return;}
            if(!QApplication::activePopupWidget())operation();wait->deleteLater();
        });wait->start(0);
    }
    QString requestVisibility(bool value) {
        logEvent("footer.visibility.request",{{"visible",value}});
        discover();
        if(nativeVisibility) {
            if(nativeVisibility->isChecked()!=value) {
                if(!nativeVisibility->isEnabled()){logEvent("footer.visibility.failed",{{"reason","native-command-disabled"},{"visible",value}});return QStringLiteral("Resolve's Show Page Navigation command is disabled.");}
                if(QApplication::activePopupWidget()){defer([this,value]{requestVisibility(value);});return {};}
                nativeVisibility->trigger();
            }
            shown=nativeVisibility->isChecked();
        } else if(nativeFooter) {
            // Fallback for early construction: this is still the real native
            // footer, in its original slot. A later native action takes authority.
            nativeFooter->setVisible(value);shown=value;
        } else {logEvent("footer.visibility.failed",{{"reason","native-footer-unavailable"},{"visible",value}});return QStringLiteral("Native Page Navigation is not available yet.");}
        pendingVisibility=false;publish();refresh();
        logEvent("footer.visibility.complete",{{"requested",value},{"visible",shown}});return {};
    }
    void discover() {
        auto main=root?root():nullptr;if(!main)return;
        if(auto footer=main->findChild<QWidget *>("m_PageSwitcherContainer"))nativeFooter=footer;
        QSet<QAction *> matches;
        for(auto action:main->findChildren<QAction *>()) {
            QString text=action->text();text.remove('&');
            if(action->isCheckable() && text.section('\t',0,0).trimmed()=="Show Page Navigation" && !action->property("openGateNavigationProxy").toBool())matches.insert(action);
        }
        if(matches.size()==1 && nativeVisibility!=*matches.begin()) {
            nativeVisibility=*matches.begin();
            connect(nativeVisibility,&QAction::toggled,this,[this](bool value){shown=value;pendingVisibility=false;publish();schedule();});
        }
        // Original page buttons are read-only models and executable targets.
        pages.clear();if(!nativeFooter)return;
        const QPair<const char *,const char *> definitions[]={{"media","buttonBrowsePanel"},{"cut","buttonFirstCutPanel"},{"edit","buttonConformPanel"},{"fusion","buttonFusionPanel"},{"color","buttonColorPanel"},{"photo","buttonPhotoPanel"},{"fairlight","buttonAudioPanel"},{"deliver","buttonDeliverPanel"},{"home","buttonHome"},{"settings","buttonConfig"}};
        const auto active=currentWorkspacePage(main);
        for(const auto &definition:definitions) {
            QList<QAbstractButton *> buttons;
            for(auto button:nativeFooter->findChildren<QAbstractButton *>(definition.second)) {
                bool hidden=button->isHidden();
                for(auto parent=button->parentWidget();parent && parent!=nativeFooter;parent=parent->parentWidget())hidden|=parent->isHidden();
                if(!hidden)buttons.append(button);
            }
            if(buttons.size()!=1)continue;
            auto target=buttons.first();
            const QString title=QString(definition.first)=="home"?QStringLiteral("Home"):QString(definition.first)=="settings"?QStringLiteral("Project Settings"):workspacePageLabel(definition.first);
            pages.append({definition.first,title,target,active==definition.first || (active.isEmpty() && target->isCheckable() && target->isChecked())});
        }
    }
    void activatePage(const QString &id) {
        discover();for(const auto &page:pages)if(page.id==id && page.target && page.target->isEnabled()) {
            // Qt's original clicked signal reaches Resolve's real page owner.
            QPointer<QAbstractButton> target=page.target;
            logEvent("footer.page.request",{{"page",id},{"route","native-page-button"}});
            target->click();schedule();
            logEvent("footer.page.dispatched",{{"page",id},{"target_alive",bool(target)}});return;
        }
        logEvent("footer.page.unavailable",{{"page",id}});
    }
    void refresh() {
        if(stopped || refreshing)return;QScopedValueRollback<bool> guard(refreshing,true);discover();
        if(nativeVisibility){pendingVisibility=false;shown=nativeVisibility->isChecked();}
        else if(pendingVisibility && nativeFooter)requestVisibility(!state["hidden"].toBool());
        else if(nativeFooter && !initialized)shown=!nativeFooter->isHidden();
        initialized=true;
        QPointer<QWidget> destination=primarySurface?primarySurface():nullptr;
        const auto nativeMain=root?root():nullptr;
        if(nativeFooter) {
            QScopedValueRollback<bool> placement(applyingPlacement,true);
            const bool custom=destination && destination!=nativeMain && pinned(destination);
            if(nativeLease.active() && (!custom || nativeLease.ownerSurface()!=destination)) {
                if(!nativeLease.release()){logEvent("footer.native.release.failed",{{"reason","native-slot-ownership-changed"}});return;}
            }
            if(custom && shown && !nativeLease.active()) {
                nativeFooter->show();
                const auto error=nativeLease.attach(nativeFooter,nativeMain,destination,Qt::BottomEdge,true);
                if(!error.isEmpty())logEvent("footer.native.mount.failed",{{"error",error}});
            }
            if(nativeLease.active()){nativeLease.setHidden(!shown);nativeFooter->setVisible(shown);}
            else nativeFooter->setVisible(shown && pinned(nativeMain));
        }
        // Native main window already has the exact native footer; leave it
        // visible without an overlay or replacement artwork. Only other surfaces
        // need an owned navigation view.
        auto hosts=surfaceHosts?surfaceHosts():QList<QWidget *>{};
        if(auto primary=primarySurface?primarySurface():nullptr)if(!hosts.contains(primary))hosts.append(primary);
        for(auto host:hosts) {
            if(!host || host==(root?root():nullptr) || host->property("unboundToolbarStrip").toBool())continue;
            auto layout=qobject_cast<QBoxLayout *>(host->layout());if(!layout || layout->direction()!=QBoxLayout::TopToBottom)continue;
            auto view=views.value(host);
            if(nativeLease.ownerSurface()==host){if(view)view->hide();continue;}
            if(!view) {
                view=new SurfacePageNavigation(host);views[host]=view;layout->addWidget(view,0);
                view->activate=[this](QString id){activatePage(id);};
                connect(host,&QObject::destroyed,this,[this,host]{views.remove(host);});
            }
            view->updatePages(pages);view->setVisible(shown && pinned(host) && nativeLease.ownerSurface()!=host && !pages.isEmpty());
        }
        for(auto it=views.begin();it!=views.end();++it)if(it.value() && !hosts.contains(it.key()))it.value()->hide();
        for(auto widget:QApplication::allWidgets())if(auto menu=qobject_cast<QMenu *>(widget))if(menu->objectName()=="workspaceShowPanel")wireCaller(menu);
        auto primary=primarySurface?primarySurface():nullptr;
        if(placementChanged)placementChanged(bool(primary),pinned(primary));
    }
    void wireCaller(QMenu *menu) {
        auto entry=menu->findChild<QAction *>("unboundPageNavigation",Qt::FindDirectChildrenOnly);
        if(!entry) {
            entry=menu->addAction("Page Navigation");entry->setObjectName("unboundPageNavigation");entry->setProperty("openGateNavigationProxy",true);entry->setCheckable(true);
            connect(entry,&QAction::triggered,this,[this](bool value){defer([this,value]{requestVisibility(value);});});
            connect(menu,&QMenu::aboutToShow,this,[this]{refresh();});
        }
        entry->setChecked(shown);
    }
    PanelAccessoryLease nativeLease;
    std::function<QWidget *()> root;QJsonObject state;QTimer timer;
    QPointer<QWidget> nativeFooter;QPointer<QAction> nativeVisibility;
    QList<NavigationPage> pages;QHash<QWidget *,QPointer<SurfacePageNavigation>> views;
    bool shown=true,pendingVisibility=true,initialized=false,refreshing=false,applyingPlacement=false,stopped=false;
};
}
