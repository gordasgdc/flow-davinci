// SPDX-License-Identifier: MIT
#pragma once
#include <QLineEdit>
#include <QPointer>
#include <QTimer>
#include "keyboardlauncher.h"
namespace Unbound {
// Navigate Resolve's real editor rather than edit a second shortcut database.
// Only its command search is changed; bindings/preset/conflict/save stay native.
inline bool filterNativeKeyboardCommands(QWidget *commands,const QString &query) {
    if (!commands) return false;
    const auto searches=commands->findChildren<QLineEdit *>();
    if (searches.size()!=1) return false;
    searches.first()->setText(query); return true;
}
class NativeKeyboardNavigation : public QObject {
public:
    explicit NativeKeyboardNavigation(QObject *owner=nullptr) : QObject(owner) { qApp->installEventFilter(this); }
    ~NativeKeyboardNavigation() override { if (qApp) qApp->removeEventFilter(this); }
    bool open(const QString &query) {
        pending=query;
        for (auto widget:QApplication::topLevelWidgets()) if (widget->objectName()=="UiActionsEditorDialog" && widget->isVisible()) {
            filter(widget); widget->raise(); widget->activateWindow(); return true;
        }
        const bool opened=openNativeKeyboardCustomization();
        if (!opened) pending.clear();
        logEvent("keyboard.native.navigate",{{"opened",opened},{"query",query},{"binding_store","resolve-native"}});
        return opened;
    }
protected:
    bool eventFilter(QObject *object,QEvent *event) override {
        if (pending.isEmpty() || event->type()!=QEvent::Show) return false;
        auto widget=qobject_cast<QWidget *>(object);
        if (!widget || widget->objectName()!="UiActionsEditorDialog") return false;
        QPointer<QWidget> alive=widget;
        QTimer::singleShot(0,this,[this,alive] { if (alive && alive->isVisible()) filter(alive); });
        return false;
    }
private:
    QString pending;
    void filter(QWidget *dialog) {
        const auto query=pending; pending.clear(); QWidget *commands=nullptr;
        for (auto widget:dialog->findChildren<QWidget *>()) if (QByteArray(widget->metaObject()->className())=="UiActionsEditorCommandsWidgetImp") {
            if (commands) { commands=nullptr; break; } commands=widget;
        }
        const bool filtered=filterNativeKeyboardCommands(commands,query);
        logEvent("keyboard.native.search",{{"query",query},{"filtered",filtered},{"binding_store","resolve-native"}});
    }
};
}
