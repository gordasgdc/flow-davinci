// SPDX-License-Identifier: MIT
#pragma once
#include "panelshortcuteditor.h"
#include <QPointer>

namespace Unbound {
// Owns command identity, shortcut context and editor lifetime. Panel transitions
// are callbacks; this module never reaches into the manager or panel sessions.
class PanelCommandCatalog : public PanelCommands {
public:
    PanelCommandCatalog(QWidget *owner, std::function<bool()> running)
        : PanelCommands(owner), editorOwner(owner)
    {
        context=[owner,running] {
            auto window=QApplication::activeWindow();
            return running() && window && (window==owner || window->objectName()=="UiMainWindow" ||
                window->objectName().startsWith("unboundFloating_"));
        };
    }
    void addPanel(const QString &name,const QString &label,std::function<bool()> available,
                  std::function<void()> toggle,std::function<void()> expand)
    {
        add({"panel.toggle."+name,"Show / Hide "+label,available,std::move(toggle)});
        add({"panel.expand."+name,"Maximize / Restore "+label,std::move(available),std::move(expand)});
    }
    void addNavigation(std::function<bool()> available,std::function<QString()> toggle)
    {
        add({"panel.toggle.page-navigation","Show / Hide Page Navigation",std::move(available),[toggle] {
            const auto error=toggle();
            logEvent("command.footer.result",{{"success",error.isEmpty()},{"error",error}});
        }});
    }
    void showEditor()
    {
        if (!editor) editor=createPanelShortcutEditor(this,editorOwner);
        editor->show(); editor->raise(); editor->activateWindow();
    }
private:
    QPointer<QWidget> editorOwner;
    QPointer<QDialog> editor;
};
}
