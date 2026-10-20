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
namespace Unbound {
// Global visibility is Resolve's command. Surface placement is OpenGate state.
// No lease, reparenting, floating session or attachment to a panel's contents.
class FooterPanel : public QObject {
public:
    FooterPanel(QObject *owner,std::function<QWidget *()> mainWindow,QJsonObject saved)
        :QObject(owner),root(std::move(mainWindow)),state(std::move(saved)) {
        timer.setSingleShot(true);timer.setInterval(40);connect(&timer,&QTimer::timeout,this,[this]{refresh();});
        pendingVisibility=state.contains("hidden");qApp->installEventFilter(this);schedule();
    }
    ~FooterPanel() override {qApp->removeEventFilter(this);releaseAttachment();}
    std::function<void()> changed;
    std::function<QWidget *()> attachmentOwner;
    std::function<void()> revealAttachmentOwner;
    std::function<QList<QWidget *>()> surfaceHosts;
    QWidget *floatingWindow() const {return nullptr;}
    bool releaseAttachment() {
        for(auto view:views)if(view)delete view;views.clear();delete nativeView.data();nativeView=nullptr;
        return true;
    }
    void syncAttachment(){refresh();}
    void restore(){refresh();}
    void wireMenu(){refresh();}
    // Compatibility entry points migrate saved floating/attached layouts to a
    // pinned surface footer. They never move Resolve's original widget.
    QString attachToWorkspace() {
        auto host=attachmentOwner?attachmentOwner():nullptr;
        if(!host)return QStringLiteral("No custom workspace surface is available yet.");
        setPinned(host,true);return showPanel();
    }
    QString attachToTimeline(){return attachToWorkspace();}
    QString floatPanel(){return attachToWorkspace();}
    void dock(){refresh();}
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
        return choices.contains(host->objectName())?choices[host->objectName()].toBool():attachmentOwner && attachmentOwner()==host;
    }
    void setPinned(QWidget *host,bool enabled) {
        if(!host || host->objectName().isEmpty())return;
        auto choices=state["surfaces"].toObject();choices[host->objectName()]=enabled;state["surfaces"]=choices;refresh();publish();
    }
    void populateSurfaceMenu(QMenu *menu,QWidget *host) {
        auto navigation=menu->addMenu("Page Navigation");navigation->setProperty("unboundOwnMenu",true);
        auto visibility=navigation->addAction("Show Page Navigation");visibility->setProperty("openGateNavigationProxy",true);visibility->setCheckable(true);visibility->setChecked(shown);
        connect(visibility,&QAction::triggered,this,[this](bool value){defer([this,value]{requestVisibility(value);});});
        auto placement=navigation->addAction("Pin to This Surface");placement->setCheckable(true);placement->setChecked(pinned(host));placement->setEnabled(host && !host->property("unboundToolbarStrip").toBool());
        QPointer<QWidget> retained(host);connect(placement,&QAction::triggered,this,[this,retained](bool value){defer([this,retained,value]{if(retained)setPinned(retained,value);});});
    }
protected:
    bool eventFilter(QObject *object,QEvent *event) override {
        const auto type=event->type();
        if(type==QEvent::Show || type==QEvent::Hide || type==QEvent::Resize || type==QEvent::LayoutRequest) {
            auto widget=qobject_cast<QWidget *>(object);
            if(widget==nativeFooter && !nativeVisibility && initialized && (type==QEvent::Show || type==QEvent::Hide)) {
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
        discover();
        if(nativeVisibility) {
            if(nativeVisibility->isChecked()!=value) {
                if(!nativeVisibility->isEnabled())return QStringLiteral("Resolve's Show Page Navigation command is disabled.");
                if(QApplication::activePopupWidget()){defer([this,value]{requestVisibility(value);});return {};}
                nativeVisibility->trigger();
            }
            shown=nativeVisibility->isChecked();
        } else if(nativeFooter) {
            // Fallback for early construction: this is still the real native
            // footer, in its original slot. A later native action takes authority.
            nativeFooter->setVisible(value);shown=value;
        } else return QStringLiteral("Native Page Navigation is not available yet.");
        pendingVisibility=false;publish();refresh();return {};
    }
    void discover() {
        auto main=root?root():nullptr;if(!main)return;
        nativeFooter=main->findChild<QWidget *>("m_PageSwitcherContainer");
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
        const QPair<const char *,const char *> definitions[]={{"media","buttonBrowsePanel"},{"cut","buttonFirstCutPanel"},{"edit","buttonConformPanel"},{"fusion","buttonFusionPanel"},{"color","buttonColorPanel"},{"photo","buttonPhotoPanel"},{"fairlight","buttonAudioPanel"},{"deliver","buttonDeliverPanel"}};
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
            pages.append({definition.first,workspacePageLabel(definition.first),target,active==definition.first || (active.isEmpty() && target->isCheckable() && target->isChecked())});
        }
    }
    void activatePage(const QString &id) {
        discover();for(const auto &page:pages)if(page.id==id && page.target && page.target->isEnabled()) {
            // Qt's original clicked signal reaches Resolve's real page owner.
            QPointer<QAbstractButton> target=page.target;target->click();schedule();
            logEvent("footer.page.request",{{"page",id},{"route","native-page-button"}});return;
        }
        logEvent("footer.page.unavailable",{{"page",id}});
    }
    void refresh() {
        if(refreshing)return;QScopedValueRollback<bool> guard(refreshing,true);discover();
        if(nativeVisibility){pendingVisibility=false;shown=nativeVisibility->isChecked();}
        else if(pendingVisibility && nativeFooter)requestVisibility(!state["hidden"].toBool());
        else if(nativeFooter && !initialized)shown=!nativeFooter->isHidden();
        initialized=true;
        if(nativeFooter && !pages.isEmpty()) {
            QWidget *slot=nativeFooter;
            for(auto parent=pages.first().target?pages.first().target->parentWidget():nullptr;parent && parent!=nativeFooter;parent=parent->parentWidget())
                if(parent->objectName()=="frameControlPanelMiddle"){slot=parent;break;}
            if(!nativeView || nativeView->parentWidget()!=slot) {
                delete nativeView.data();nativeView=new SurfacePageNavigation(slot,true);
                nativeView->activate=[this](QString id){activatePage(id);};
            }
            // Overlay only the native page-button region. Home/Settings and
            // adjacent native footer controls remain untouched and reachable.
            const QRect inFooter(slot->mapTo(nativeFooter,QPoint()),slot->size());
            const auto visible=inFooter.intersected(nativeFooter->rect());
            const QRect local(slot->mapFrom(nativeFooter,visible.topLeft()),visible.size());
            nativeView->updatePages(pages);nativeView->setNativeSlotGeometry(local);nativeView->show();nativeView->raise();
        }
        auto hosts=surfaceHosts?surfaceHosts():QList<QWidget *>{};
        if(auto primary=attachmentOwner?attachmentOwner():nullptr)if(!hosts.contains(primary))hosts.append(primary);
        for(auto host:hosts) {
            if(!host || host==(root?root():nullptr) || host->property("unboundToolbarStrip").toBool())continue;
            auto layout=qobject_cast<QBoxLayout *>(host->layout());if(!layout || layout->direction()!=QBoxLayout::TopToBottom)continue;
            auto view=views.value(host);
            if(!view) {
                view=new SurfacePageNavigation(host);views[host]=view;layout->addWidget(view,0);
                view->activate=[this](QString id){activatePage(id);};
                connect(host,&QObject::destroyed,this,[this,host]{views.remove(host);});
            }
            view->updatePages(pages);view->setVisible(shown && pinned(host) && !pages.isEmpty());
        }
        for(auto it=views.begin();it!=views.end();++it)if(it.value() && !hosts.contains(it.key()))it.value()->hide();
        for(auto widget:QApplication::allWidgets())if(auto menu=qobject_cast<QMenu *>(widget))if(menu->objectName()=="workspaceShowPanel")wireCaller(menu);
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
    std::function<QWidget *()> root;QJsonObject state;QTimer timer;
    QPointer<QWidget> nativeFooter;QPointer<QAction> nativeVisibility;
    QPointer<SurfacePageNavigation> nativeView;
    QList<NavigationPage> pages;QHash<QWidget *,QPointer<SurfacePageNavigation>> views;
    bool shown=true,pendingVisibility=true,initialized=false,refreshing=false;
};
}
