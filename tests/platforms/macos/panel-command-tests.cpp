// SPDX-License-Identifier: MIT
#include <QTemporaryDir>
#include <QLineEdit>
#include <cstdio>
#include "panelshortcuteditor.h"
int main(int argc,char **argv) {
    qputenv("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM","1");
    QApplication app(argc,argv); app.setQuitOnLastWindowClosed(false);
    QTemporaryDir files; qputenv("UNBOUND_SHORTCUTS_FILE",(files.path()+"/Shortcuts.json").toUtf8());
    qputenv("UNBOUND_DIAGNOSTICS_DIR",files.path().toUtf8());
    int failures=0; auto check=[&](bool ok,const char *why) { if (!ok) { ++failures; std::fprintf(stderr,"FAIL: %s\n",why); } };
    QWidget main; main.setAttribute(Qt::WA_DontShowOnScreen); main.show(); QApplication::setActiveWindow(&main);
    bool ready=true; int source=0,program=0;
    Unbound::PanelCommands commands(nullptr); commands.context=[&] { return ready; };
    commands.add({"source","Show / Hide Source",[] { return true; },[&] { ++source; }});
    commands.add({"program","Show / Hide Program",[] { return true; },[&] { ++program; }});
    check(commands.save({{"source","F17"},{"program","F18"}}).isEmpty(),"save independent bindings");
    const auto send=[&](int key,bool repeat=false) {
        QKeyEvent over(QEvent::ShortcutOverride,key,Qt::NoModifier); QApplication::sendEvent(&main,&over);
        QKeyEvent press(QEvent::KeyPress,key,Qt::NoModifier,QString(),repeat); QApplication::sendEvent(&main,&press);
        QApplication::processEvents();
    };
    QApplication::processEvents(); send(Qt::Key_F17); check(source==1 && program==0,"Source command never invokes Program");
    send(Qt::Key_F18); check(source==1 && program==1,"Program command never invokes Source");
    send(Qt::Key_F18,true); check(program==1,"auto-repeat does not toggle doors repeatedly");
    QLineEdit edit(&main); edit.show(); QApplication::setActiveWindow(&main); edit.setFocus(); QApplication::processEvents();
    check(QApplication::focusWidget()==&edit,"typing fixture establishes actual text focus"); send(Qt::Key_F17); check(source==1,"typing yields to native text input");
    edit.clearFocus(); ready=false; send(Qt::Key_F17); check(source==1,"inactive page cannot dispatch a panel command"); ready=true;
    check(!commands.save({{"source","F17"},{"program","F17"}}).isEmpty(),"duplicate bindings rejected atomically");
    Unbound::PanelCommands reopened(nullptr); check(reopened.assigned().value("program")=="F18","bindings survive reopening after failed edit");
    QAction native(&main); native.setShortcut(Qt::Key_F19); main.addAction(&native);
    check(!commands.save({{"source","F19"}}).isEmpty(),"native shortcut conflicts rejected");
    native.setShortcut(Qt::Key_F17); QApplication::processEvents();
    send(Qt::Key_F17); check(source==1,"a newly assigned native collision blocks only that Unbound binding");
    send(Qt::Key_F18); check(program==2,"one native collision cannot disable unrelated panel commands");
    native.setShortcut(Qt::Key_F19); QApplication::processEvents();
    send(Qt::Key_F17); check(source==2,"removing a native collision restores the saved Unbound binding");
    auto dialog=Unbound::createPanelShortcutEditor(&commands,&main); dialog->setAttribute(Qt::WA_DontShowOnScreen);
    check(dialog->findChild<QKeySequenceEdit *>("binding_source")!=nullptr,"every command has a discoverable editable binding"); delete dialog;
    Unbound::flushDiagnostics(); QFile events(files.path()+"/events.jsonl"); events.open(QIODevice::ReadOnly);
    check(events.readAll().contains("command.invoke"),"command origin and acceptance logged");
    if (!failures) std::puts("PASS: independent commands, persistence, conflicts, typing, page scope, repeats and editor discovery");
    return failures ? 1 : 0;
}
