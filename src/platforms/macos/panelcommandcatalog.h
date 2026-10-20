// SPDX-License-Identifier: MIT
#pragma once
#include "panelshortcuteditor.h"
#include "nativekeyboardnavigation.h"
#include "nativepresetbindings.h"
#include "pagecontext.h"
#include <QPointer>

namespace Unbound {
// Owns command identity, shortcut context and editor lifetime. Panel transitions
// are callbacks; this module never reaches into the manager or panel sessions.
class PanelCommandCatalog : public PanelCommands {
public:
    PanelCommandCatalog(QWidget *owner, std::function<bool()> running)
        : PanelCommands(owner), nativeEditor(this)
    {
        // Explicit opt-in for acceptance testing; never silently enable a new
        // interpreter in an existing installation.
        if (qEnvironmentVariableIntValue("OPENGATE_NATIVE_PRESET_SHORTCUTS")==1) {
            preset=new NativePresetBindings(this,qEnvironmentVariable("OPENGATE_PRESET_DIRECTORY",
                QDir::homePath()+"/Library/Preferences/Blackmagic Design/DaVinci Resolve"));
            nativeMatch=[this](const QKeySequence &pressed) { return matchPreset(pressed); };
            legacyBacktickAllowed=[this] {return preset->snapshot().commands.value("OpenGate - Maximize-Restore Hovered Panel").isEmpty();};
        }
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
        if (!nativeEditor.open("OpenGate"))
            logEvent("keyboard.native.unavailable",{{"origin","opengate-command-menu"},{"duplicate_editor_opened",false}});
    }
private:
    NativeMatch matchPreset(const QKeySequence &pressed) const {
        if(!preset || currentWorkspacePage(resolveMainWindow())!="edit" || !preset->snapshot().error.isEmpty())return {};
        const auto &snapshot=preset->snapshot();
        QString id,label;
        for(const auto &command:entries()) {
            auto nativeLabel=command.label;
            if(command.id=="edit.inspector.stabilize")nativeLabel="Edit - Stabilize";
            else if(command.id=="edit.timeline.fixed-playhead")nativeLabel="Edit - Toggle Fixed Playhead";
            else if(command.id=="panel.expand.hovered")nativeLabel="Maximize-Restore Hovered Panel";
            else nativeLabel.replace(" / ","-");
            nativeLabel="OpenGate - "+nativeLabel;
            if(!snapshot.commands.value(nativeLabel).contains(pressed) || !command.available || !command.available())continue;
            if(!id.isEmpty() && id!=command.id)return {{},"ambiguous-opengate-assignment"};
            id=command.id;label=nativeLabel;
        }
        if(id.isEmpty())return {};
        if(pressed.count()!=1)return {{},"multi-chord-not-supported"};
        for(const auto &other:snapshot.owners.value(pressed.toString(QKeySequence::PortableText))) {
            if(other==label)continue;
            // This known Fairlight-scoped command cannot run on the Edit page.
            if(other.startsWith("FairlightTimeline."))continue;
            return {{},"native-preset-conflict: "+other};
        }
        return {id,{}};
    }
    NativePresetBindings *preset=nullptr;
    NativeKeyboardNavigation nativeEditor;
};
}
