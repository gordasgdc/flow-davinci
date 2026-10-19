// SPDX-License-Identifier: MIT
#pragma once
#include <QMenu>
#include <QPointer>
#include <QTimer>
#include <functional>
#include "diagnostics.h"
#include "workspacepages.h"
namespace Unbound {
// Page-grouped, show-only callers. Commands never toggle an already visible
// panel off, change its saved placement or silently switch Resolve pages.
class PanelCallerMenu : public QObject {
public:
    struct Panel {
        QString id,label,page;
        std::function<bool()> available,visible;
        std::function<void()> show;
    };
    explicit PanelCallerMenu(QObject *owner=nullptr) : QObject(owner) {}
    std::function<QString()> currentPage=[] { return QString(); };
    void add(Panel panel) { panels.append(std::move(panel)); }
    QMenu *wire(QMenu *root) {
        auto menu=root->findChild<QMenu *>("openGateShowPanel",Qt::FindDirectChildrenOnly);
        if (!menu) {
            menu=new QMenu("Show Panel in Workspace",root); menu->setObjectName("openGateShowPanel"); menu->setProperty("unboundOwnMenu",true);
            root->addMenu(menu);
        }
        for (int i=0;i<panels.size();++i) {
            const auto &panel=panels[i];
            auto page=menu->findChild<QMenu *>("openGatePanelPage_"+panel.page,Qt::FindDirectChildrenOnly);
            if (!page) {
                page=new QMenu(workspacePageLabel(panel.page),menu); page->setObjectName("openGatePanelPage_"+panel.page);
                page->setProperty("unboundOwnMenu",true); menu->addMenu(page);
            }
            auto action=page->findChild<QAction *>("openGateShowPanel_"+panel.id,Qt::FindDirectChildrenOnly);
            if (!action) {
                action=page->addAction(panel.label); action->setObjectName("openGateShowPanel_"+panel.id); action->setCheckable(true);
                connect(action,&QAction::triggered,this,[this,i] {
                    QTimer::singleShot(0,this,[this,i] {
                        const auto &panel=panels[i]; const bool ready=currentPage()==panel.page && panel.available && panel.available();
                        logEvent("workspace.panel.show",{{"panel",panel.id},{"page",panel.page},{"accepted",ready},{"origin","opengate-ui-menu"}});
                        if (ready && panel.show) panel.show();
                    });
                });
            }
            action->setEnabled(currentPage()==panel.page && panel.available && panel.available());
            action->setChecked(panel.visible && panel.visible());
            action->setToolTip("Show "+panel.label+" in its current arrangement");
        }
        if (!watched.contains(menu)) {
            watched.append(menu); connect(menu,&QMenu::aboutToShow,this,[this,root] { wire(root); });
            for (auto page:menu->findChildren<QMenu *>(QString(),Qt::FindDirectChildrenOnly))
                connect(page,&QMenu::aboutToShow,this,[this,root] { wire(root); });
        }
        return menu;
    }
private:
    QList<Panel> panels; QList<QPointer<QMenu>> watched;
};
}
