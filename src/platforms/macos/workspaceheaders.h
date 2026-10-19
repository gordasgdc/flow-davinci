// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QPointer>
#include <QSet>
#include <QTimer>
#include "diagnostics.h"
#include "nativechrome.h"
namespace Unbound {
// Existing native title-bar accessories only: no extra row, window, or copy of
// Resolve's toolbar. All actions delegate to the existing panel/group owners.
class WorkspaceHeaders : public QObject {
public:
    struct Panel { QString id,title;std::function<QWidget *()> host;std::function<bool()> shown;std::function<void(bool)> setShown; };
    struct Window { QString title;std::function<bool()> shown;std::function<void(bool)> setShown; };
    explicit WorkspaceHeaders(QObject *owner):QObject(owner) {
        timer.setSingleShot(true);timer.setInterval(50);
        connect(&timer,&QTimer::timeout,this,[this] { refresh(); });qApp->installEventFilter(this);
    }
    ~WorkspaceHeaders() override {
        qApp->removeEventFilter(this);
        for (auto host:known) if (host) setNativeHeaderMenu(host,false,{});
    }
    void add(Panel panel) { panels.append(std::move(panel)); }
    std::function<QWidget *()> primary,projectSurface;
    std::function<QMenu *()> allPanels;
    std::function<QList<Window>()> windows;
    void schedule() { if (!timer.isActive()) timer.start(); }
    void populate(QMenu *menu,QWidget *host) {
        menu->clear();menu->addSection("This Window");
        for (const auto &panel:panels) if (panel.host && panel.host()==host) {
            auto action=menu->addAction(panel.title);action->setCheckable(true);action->setChecked(panel.shown && panel.shown());
            connect(action,&QAction::triggered,this,[this,panel](bool shown) {
                QTimer::singleShot(0,this,[panel,shown] {
                    logEvent("header.panel.visibility",{{"panel",panel.id},{"requested",shown}});
                    if (panel.setShown) panel.setShown(shown);
                });
            });
        }
        const bool global=primary && primary()==host;
        if (global) {
            if (auto callers=allPanels ? allPanels() : nullptr) {
                menu->addSeparator();auto all=menu->addMenu("All Panels");all->setProperty("unboundOwnMenu",true);all->addActions(callers->actions());
            }
            if (windows) {
                auto groups=menu->addMenu("Floating Windows");
                for (const auto &entry:windows()) {
                    auto action=groups->addAction(entry.title);action->setCheckable(true);action->setChecked(entry.shown && entry.shown());
                    connect(action,&QAction::triggered,this,[this,entry](bool shown) {
                        QTimer::singleShot(0,this,[entry,shown] {
                            logEvent("header.window.visibility",{{"window",entry.title},{"requested",shown}});
                            if (entry.setShown) entry.setShown(shown);
                        });
                    });
                }
            }
        }
        logEvent("header.menu.open",{{"scope",global ? "workspace" : "window"},{"host",host->objectName()}});
    }
protected:
    bool eventFilter(QObject *object,QEvent *event) override {
        if (object==projectLabel && (event->type()==QEvent::Paint || event->type()==QEvent::LayoutRequest)) {
            if (object->property("text").toString().trimmed()!=lastProject) schedule();return false;
        }
        if (event->type()!=QEvent::Show && event->type()!=QEvent::WindowActivate && event->type()!=QEvent::Resize && event->type()!=QEvent::WindowTitleChange) return false;
        auto widget=qobject_cast<QWidget *>(object);
        if (widget && widget->isWindow() && widget->objectName().startsWith("unboundFloating_")) schedule();
        return false;
    }
private:
    QString projectName() {
        auto surface=projectSurface ? projectSurface() : nullptr;
        if (!surface) return {};
        if (!projectLabel || !surface->isAncestorOf(projectLabel) || !projectLabel->isVisibleTo(surface)) {
            projectLabel=nullptr;
            for (auto label:surface->findChildren<QLabel *>()) if (label->objectName()=="labelTitle" &&
                label->parentWidget() && label->parentWidget()->objectName()=="centerFrame" && label->isVisibleTo(surface)) {
                projectLabel=label;break;
            }
        }
        return projectLabel ? projectLabel->property("text").toString().trimmed() : QString();
    }
    void refresh() {
        const auto project=projectName();
        if (!projectScanned || project!=lastProject) logEvent("header.project.source",{{"resolved",!project.isEmpty()},{"source",widgetState(projectLabel)}});
        projectScanned=true;lastProject=project;QSet<QWidget *> current;
        for (const auto &panel:panels) if (auto host=panel.host ? panel.host() : nullptr)
            if (host->objectName().startsWith("unboundFloating_")) current.insert(host);
        for (auto host:current) {
            if (!host->property("unboundHeaderOriginalTitle").isValid()) host->setProperty("unboundHeaderOriginalTitle",host->windowTitle());
            if (!project.isEmpty()) {
                QString title=project;
                if (!primary || primary()!=host) {
                    QStringList names;for (const auto &panel:panels) if (panel.host && panel.host()==host) names.append(panel.title);
                    if (!names.isEmpty()) title+=" — "+names.first()+(names.size()>1 ? QString(" +%1").arg(names.size()-1) : QString());
                }
                if (host->windowTitle()!=title) { host->setWindowTitle(title);logEvent("header.project.updated",{{"project",project},{"host",host->objectName()}}); }
            }
            QPointer<WorkspaceHeaders> alive(this);QPointer<QWidget> window(host);
            setNativeHeaderMenu(host,true,[alive,window] {
                if (!alive || !window) return;
                auto menu=window->findChild<QMenu *>("openGateWindowPanels",Qt::FindDirectChildrenOnly);
                if (!menu) { menu=new QMenu(window);menu->setObjectName("openGateWindowPanels");menu->setProperty("unboundOwnMenu",true); }
                alive->populate(menu,window);menu->popup(window->mapToGlobal(QPoint(qMax(0,window->width()-240),0)));
            });
            if (!known.contains(host)) known.append(host);
        }
        for (int i=known.size()-1;i>=0;--i) if (!known[i]) known.removeAt(i);
        else if (!current.contains(known[i])) { setNativeHeaderMenu(known[i],false,{});known.removeAt(i); }
    }
    QList<Panel> panels;QList<QPointer<QWidget>> known;QPointer<QWidget> projectLabel;
    QString lastProject;QTimer timer;bool projectScanned=false;
};
}
