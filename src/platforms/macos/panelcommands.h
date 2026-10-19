// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QAction>
#include <QDir>
#include <QDialog>
#include <QFile>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QJsonDocument>
#include <QKeyEvent>
#include <QSaveFile>
#include <QShortcut>
#include <QTimer>
#include <functional>
#include "interactiondiagnostics.h"
namespace Unbound {
// Unbound-owned bindings. This is not registration in Resolve's command database.
class PanelCommands : public QObject {
public:
    struct Command { QString id,label; std::function<bool()> available; std::function<void()> invoke; };
    explicit PanelCommands(QObject *owner) : QObject(owner),path(qEnvironmentVariable("UNBOUND_SHORTCUTS_FILE",
        QDir::homePath()+"/Library/Application Support/Unbound/Shortcuts.json")) {
        QFile f(path); if (f.open(QIODevice::ReadOnly)) bindings=QJsonDocument::fromJson(f.readAll()).object();
        refresh.setSingleShot(true);
        connect(&refresh,&QTimer::timeout,this,[this] { conflicts=bindingErrors(bindings); });
        refresh.start(0); qApp->installEventFilter(this);
    }
    ~PanelCommands() override { if (qApp) qApp->removeEventFilter(this); }
    void add(Command value) { commands.append(std::move(value)); }
    void defaultBinding(const QString &id,const QString &sequence) {
        if (!bindings.contains(id)) bindings[id]=sequence;
        refresh.start(0);
    }
    bool invoke(const QString &id,const QString &origin) {
        auto active=QApplication::activeWindow();
        const bool eligible=active && !qobject_cast<QDialog *>(active) && !QApplication::activeModalWidget() &&
            !QApplication::activePopupWidget() && !textEditor(QApplication::focusWidget()) && context && context();
        for (const auto &command:commands) if (command.id==id) {
            const bool accepted=eligible && command.available && command.available();
            logEvent("command.invoke",{{"command",id},{"origin",origin},{"accepted",accepted}});
            if (accepted && command.invoke) command.invoke();
            return accepted;
        }
        logEvent("command.invoke.unknown",{{"command",id},{"origin",origin}}); return false;
    }
    const QList<Command> &entries() const { return commands; }
    QJsonObject assigned() const { return bindings; }
    QString validate(const QJsonObject &proposed) const {
        const auto errors=bindingErrors(proposed);
        for (const auto &command : commands) if (errors.contains(command.id)) return errors.value(command.id);
        return {};
    }
    QString save(const QJsonObject &proposed) {
        auto error=validate(proposed); if (!error.isEmpty()) return error;
        if (!QDir().mkpath(QFileInfo(path).absolutePath())) return "Cannot create shortcut settings directory.";
        QSaveFile f(path); const auto bytes=QJsonDocument(proposed).toJson();
        if (!f.open(QIODevice::WriteOnly) || f.write(bytes)!=bytes.size() || !f.commit()) return "Cannot save shortcut settings.";
        bindings=proposed; conflicts.clear(); logEvent("command.bindings.saved",{{"count",bindings.size()}}); return {};
    }
    std::function<bool()> context;
protected:
    bool eventFilter(QObject *,QEvent *event) override {
        if (event->type()==QEvent::ActionAdded || event->type()==QEvent::ActionChanged || event->type()==QEvent::ActionRemoved) {
            refresh.start(0); return false;
        }
        if (event->type()!=QEvent::ShortcutOverride && event->type()!=QEvent::KeyPress) return false;
        auto active=QApplication::activeWindow();
        if (!active || qobject_cast<QDialog *>(active) || active->property("unboundShortcutEditor").toBool() || QApplication::activeModalWidget() ||
            QApplication::activePopupWidget() || textEditor(QApplication::focusWidget()) || !context || !context()) return false;
        auto key=static_cast<QKeyEvent *>(event);
        const QKeySequence pressed(key->key() | int(key->modifiers() & ~Qt::KeypadModifier));
        for (const auto &command : commands) {
            const auto configured=QKeySequence::fromString(bindings.value(command.id).toString(),QKeySequence::PortableText);
            if (configured.isEmpty() || configured!=pressed || !command.available || !command.available()) continue;
            // Recheck native conflicts after loading a different Resolve keyboard preset.
            if (refresh.isActive() || conflicts.contains(command.id)) { logEvent("command.binding.conflict",{{"command",command.id}}); return false; }
            event->accept();
            if (event->type()==QEvent::KeyPress && !key->isAutoRepeat()) {
                const auto id=command.id; const auto available=command.available; const auto invoke=command.invoke;
                QTimer::singleShot(0,this,[id,available,invoke,this] {
                    const bool ready=context && context() && available();
                    logEvent("command.invoke",{{"command",id},{"origin","unbound-shortcut"},{"accepted",ready}});
                    if (ready && invoke) {
                        QElapsedTimer timing; timing.start(); invoke();
                        logEvent("command.dispatched",{{"command",id},{"dispatch_ms",timing.nsecsElapsed()/1000000.0},
                            {"completion","panel-transition-logged-separately"}});
                    }
                });
            }
            return true;
        }
        return false;
    }
private:
    QList<Command> commands;
    QJsonObject bindings;
    QString path;
    QHash<QString,QString> conflicts;
    QTimer refresh;
    QHash<QString,QString> bindingErrors(const QJsonObject &proposed) const {
        QHash<QString,QString> errors,seen;
        bool assigned=false;
        for (const auto &command : commands) assigned |= !proposed.value(command.id).toString().isEmpty();
        if (!assigned) return errors;
        // One native inventory per refresh/save, never one scan per command/key.
        QList<QKeySequence> nativeKeys; QSet<QAction *> visited;
        for (auto widget : QApplication::allWidgets()) {
            for (auto action : widget->actions()) if (!visited.contains(action)) {
                visited.insert(action); nativeKeys.append(action->shortcuts());
            }
            for (auto shortcut : widget->findChildren<QShortcut *>(QString(),Qt::FindDirectChildrenOnly))
                nativeKeys.append(shortcut->key());
        }
        for (const auto &command : commands) {
            const auto text=proposed.value(command.id).toString(); if (text.isEmpty()) continue;
            const auto key=QKeySequence::fromString(text,QKeySequence::PortableText);
            if (key.isEmpty() || key.count()!=1) { errors[command.id]="Use one key combination per command."; continue; }
            const auto normalized=key.toString(QKeySequence::PortableText);
            if (seen.contains(normalized)) {
                const auto message="Two OpenGate commands use "+normalized+".";
                errors[command.id]=message; errors[seen.value(normalized)]=message;
            }
            seen[normalized]=command.id;
            for (const auto &native : nativeKeys)
                if (!native.isEmpty() && (key.matches(native)!=QKeySequence::NoMatch || native.matches(key)!=QKeySequence::NoMatch)) {
                    errors[command.id]=normalized+" is already assigned to a native action or shortcut."; break;
                }
        }
        return errors;
    }
};
}
