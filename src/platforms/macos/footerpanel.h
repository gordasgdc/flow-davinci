// SPDX-License-Identifier: MIT
#pragma once
#include <QMenu>
#include <QToolButton>
#include <QBoxLayout>
#include <QSet>
#include "panelsession.h"
#include "workspacesettings.h"
#include "panelaccessorylease.h"
namespace Unbound {
// Global navigation belongs to the main window, not to a particular Resolve page.
class FooterPanel : public QObject
{
public:
    FooterPanel(QObject *owner, std::function<QWidget *()> mainWindow, QJsonObject saved)
        : QObject(owner), root(std::move(mainWindow)), state(std::move(saved)), session(this), accessory(this)
    {
        qApp->installEventFilter(this);
        QTimer::singleShot(0, this, [this] { wireMenu(); });
        session.changed = [this] { decorate(); if (changed) changed(); };
        session.arrangementChanged = [this] { if (changed) changed(); };
    }
    ~FooterPanel() override { qApp->removeEventFilter(this); session.changed = {}; session.arrangementChanged = {}; }
    std::function<void()> changed;
    std::function<QWidget *()> attachmentOwner;
    std::function<void()> revealAttachmentOwner;
    QWidget *floatingWindow() const { return session.active() && session.window() ? session.window()->window() : nullptr; }
    bool releaseAttachment() { return accessory.release(); }
    void syncAttachment()
    {
        const bool wantsOwner=!state["floating"].toBool() && state["attachment"].toString()!="native";
        auto owner=wantsOwner && attachmentOwner ? attachmentOwner() : nullptr;
        if (!owner) { if (accessory.active()) accessory.release(); return; }
        if (accessory.active() || session.active()) return;
        auto main=root(); auto footer=main ? main->findChild<QWidget *>("m_PageSwitcherContainer") : nullptr;
        const auto error=accessory.attach(footer,main,owner);
        if (!error.isEmpty()) {
            if (error!=attachmentError) logEvent("footer.attach.failed",{{"error",error}});
            attachmentError=error; return;
        }
        attachmentError.clear(); accessory.setHidden(state["hidden"].toBool());
        if (changed) changed();
    }
    QString attachToTimeline()
    {
        if (!attachmentOwner || !attachmentOwner()) return QStringLiteral("Undock Timeline before attaching Page Navigation.");
        if (session.active()) {
            const auto error=session.dock(); if (!error.isEmpty()) return error;
        }
        state["floating"]=false; state["attachment"]="timeline";
        syncAttachment(); if (changed) changed(); return attachmentError;
    }
    QString floatPanel()
    {
        if (session.active()) { session.setHiddenByUser(false); return {}; }
        if (!accessory.release()) return QStringLiteral("Could not release Page Navigation from its owner.");
        auto main = root();
        auto footer = main ? main->findChild<QWidget *>("m_PageSwitcherContainer") : nullptr;
        if (!footer) return QStringLiteral("Native page navigation is not available yet.");
        const QPoint position = footer->mapToGlobal(QPoint());
        const int rowHeight = qMax(footer->minimumHeight(), footer->sizeHint().height());
        footer->show();
        const auto error = session.undock(footer, main, QStringLiteral("Page Navigation"));
        if (!error.isEmpty()) return error;
        const auto saved = WorkspaceSettings::rectangle(state);
        auto window = session.window();
        const int chromeHeight = window->contentsMargins().top() + window->contentsMargins().bottom();
        const int compactHeight = qMax(32, rowHeight) + chromeHeight;
        window->setMaximumHeight(compactHeight);
        session.setFloatingGeometry(QRect(saved.isValid() ? saved.topLeft() : position,
            QSize(saved.isValid() ? saved.width() : 900, compactHeight)));
        session.setPinned(state["pinned"].toBool());
        state["floating"] = true; state["attachment"]="floating";
        logEvent(QStringLiteral("footer.undock"));
        if (changed) changed();
        return {};
    }
    QString showPanel() {
        state["hidden"]=false;
        if (!session.active() && !accessory.active()) if (auto main=root())
            if (auto widget=main->findChild<QWidget *>("m_PageSwitcherContainer")) {
                widget->show();
                if (state["attachment"].toString()=="native") {
                    logEvent("footer.visibility",{{"placement","native"},{"hidden",false},{"origin","show-panel"}});
                    if (changed) changed(); return {};
                }
            }
        syncAttachment();
        if (accessory.active()) {
            if (revealAttachmentOwner) revealAttachmentOwner();
            accessory.setHidden(false); state["hidden"]=false;
            logEvent("footer.visibility",{{"placement","timeline"},{"hidden",false},{"origin","show-panel"}});
            if (changed) changed(); return {};
        }
        return floatPanel();
    }
    QString toggleVisibility()
    {
        syncAttachment();
        if (accessory.active()) {
            if (accessory.ownerSurface() && !accessory.ownerSurface()->isVisible()) {
                if (revealAttachmentOwner) revealAttachmentOwner();
                accessory.setHidden(false);
            } else accessory.setHidden(!accessory.hidden());
            state["hidden"]=accessory.hidden();
            logEvent("footer.visibility",{{"placement","timeline"},{"hidden",accessory.hidden()}});
            if (changed) changed(); return {};
        }
        if (session.active()) { session.setHiddenByUser(!session.hiddenByUser()); return {}; }
        return floatPanel();
    }
    QString hidePanel() {
        syncAttachment(); state["hidden"]=true;
        if (accessory.active()) accessory.setHidden(true);
        else if (session.active()) session.setHiddenByUser(true);
        else if (auto main=root()) if (auto widget=main->findChild<QWidget *>("m_PageSwitcherContainer")) widget->hide();
        logEvent("footer.visibility",{{"hidden",true},{"origin","manager-hide"}});
        if (changed) changed(); return {};
    }
    void dock()
    {
        auto next=snapshot();
        if (!accessory.release()) { logEvent("footer.dock.failed",{{"reason","attachment-ownership-changed"}}); return; }
        if (session.active()) {
            const auto error=session.dock();
            if (!error.isEmpty()) { logEvent("footer.dock.failed",{{"error",error}}); return; }
        }
        state=next; state["floating"] = false; state["attachment"]="native";
        if (changed) changed();
    }
    void restore()
    {
        wireMenu(); syncAttachment();
        if (!state["floating"].toBool() || session.active()) return;
        const bool hidden = state["hidden"].toBool();
        if (floatPanel().isEmpty()) session.setHiddenByUser(hidden);
    }
    void setState(const QJsonObject &value)
    {
        if (!accessory.release()) { logEvent("footer.restore.failed",{{"reason","attachment-ownership-changed"}}); return; }
        if (session.active()) {
            const auto error=session.dock();
            if (!error.isEmpty()) { logEvent("footer.restore.failed",{{"error",error}}); return; }
        }
        state=value; restore();
    }
    QJsonObject snapshot() const
    {
        auto value = state;
        if (session.active()) {
            value = WorkspaceSettings::rectangle(session.floatingGeometry());
            value["floating"] = true; value["hidden"] = session.hiddenByUser(); value["pinned"] = session.pinned();
        }
        if (accessory.active()) {
            value["floating"]=false; value["attachment"]="timeline"; value["hidden"]=accessory.hidden();
        }
        return value;
    }
    void wireMenu()
    {
        decorate();
        for (auto widget : QApplication::allWidgets()) if (auto menu = qobject_cast<QMenu *>(widget))
            if (menu->objectName() == QLatin1String("workspaceShowPanel")) refreshMenu(menu);
    }
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (event->type() != QEvent::Show && event->type() != QEvent::Hide && event->type() != QEvent::Resize) return false;
        auto widget=qobject_cast<QWidget *>(object);
        if (widget && widget->objectName()=="WStackPage_Conform")
            QTimer::singleShot(0,this,[this] { syncAttachment(); });
        auto menu = qobject_cast<QMenu *>(object);
        if (menu && event->type() == QEvent::Show && menu->objectName() == QLatin1String("workspaceShowPanel")) refreshMenu(menu);
        if (object == decoratedFooter && event->type() == QEvent::Resize) positionButton();
        return false;
    }
private:
    void decorate()
    {
        auto main = root(); auto footer = main ? main->findChild<QWidget *>("m_PageSwitcherContainer") : nullptr;
        if (!footer) return;
        auto button = footer->findChild<QToolButton *>("unboundUndockPageNavigation");
        if (!button) {
            button = new QToolButton(footer); button->setObjectName("unboundUndockPageNavigation");
            button->setAutoRaise(true); button->setFocusPolicy(Qt::NoFocus);
        }
        // Reserve a small action at the end of the row, not a second layout row.
        if (decoratedFooter != footer) {
            decoratedFooter = footer; footer->installEventFilter(this);
            if (!footer->property("unboundFooterActionSpace").toBool()) {
                auto margins = footer->contentsMargins(); margins.setRight(margins.right() + 24);
                footer->setContentsMargins(margins); footer->setProperty("unboundFooterActionSpace", true);
            }
        }
        if (footer->layout()) footer->layout()->removeWidget(button);
        if (nativeButton != button) {
            nativeButton = button;
            if (button->menu()) button->menu()->deleteLater();
            auto menu = new QMenu(button); button->setMenu(menu); button->setPopupMode(QToolButton::InstantPopup);
            auto attach=menu->addAction("Attach Page Navigation to Timeline");
            attach->setObjectName("unboundAttachFooterTimeline");
            connect(menu,&QMenu::aboutToShow,this,[this,attach] { attach->setEnabled(bool(attachmentOwner && attachmentOwner())); });
            connect(attach,&QAction::triggered,this,[this] {
                logEvent("footer.attach.request",{{"origin","footer-menu"}});
                QTimer::singleShot(0,this,[this] {
                    const auto error=attachToTimeline();
                    logEvent("footer.attach.result",{{"success",error.isEmpty()},{"error",error}});
                });
            });
            auto native=menu->addAction("Dock Page Navigation to Native Footer");
            native->setObjectName("unboundDockFooterNative");
            connect(menu,&QMenu::aboutToShow,this,[this,native] { native->setVisible(accessory.active()); });
            connect(native,&QAction::triggered,this,[this] {
                logEvent("footer.native-dock.request",{{"origin","footer-menu"}});
                QTimer::singleShot(0,this,[this] { dock(); });
            });
            auto toggle = menu->addAction(QString());
            connect(menu, &QMenu::aboutToShow, this, [this, toggle] {
                toggle->setText(session.active() ? "Dock Page Navigation to Native Footer" : "Float Page Navigation");
            });
            connect(toggle, &QAction::triggered, this, [this] {
                logEvent(QStringLiteral("footer.native-toggle.clicked"));
                QTimer::singleShot(0, this, [this] { if (session.active()) dock(); else floatPanel(); });
            });
        }
        button->setText(QStringLiteral("…"));
        button->setToolTip(session.active() ? "Dock Page Navigation to native footer" : "Float Page Navigation");
        positionButton(); button->show(); button->raise();
    }
    void positionButton()
    {
        if (nativeButton && decoratedFooter)
            nativeButton->setGeometry(decoratedFooter->width() - 24, qMax(0, (decoratedFooter->height() - 24) / 2), 24, 24);
    }
    void refreshMenu(QMenu *menu)
    {
        QAction *entry = nullptr;
        for (auto action : menu->actions()) if (action->objectName() == QLatin1String("unboundPageNavigation")) entry = action;
        if (!entry) {
            entry = menu->addAction(QStringLiteral("Page Navigation (Footer)"));
            entry->setObjectName(QStringLiteral("unboundPageNavigation")); entry->setCheckable(true);
        }
        if (!wiredEntries.contains(entry)) {
            wiredEntries.insert(entry);
            connect(entry, &QObject::destroyed, this, [this, entry] { wiredEntries.remove(entry); });
            connect(menu, &QMenu::aboutToShow, this, [this, menu] { refreshMenu(menu); });
            connect(entry, &QAction::triggered, this, [this] {
                QTimer::singleShot(0, this, [this] {
                    const auto error=toggleVisibility();
                    logEvent(QStringLiteral("footer.open.result"), {{"success",error.isEmpty()},{"error",error}});
                });
            });
        }
        entry->setChecked(accessory.active() ? accessory.ownerSurface() && accessory.ownerSurface()->isVisible() && !accessory.hidden() :
            session.active() && !session.hiddenByUser());
    }
private:
    QPointer<QToolButton> nativeButton;
    QPointer<QWidget> decoratedFooter;
    QSet<QAction *> wiredEntries;
    std::function<QWidget *()> root;
    QJsonObject state;
    MacPanelSession session;
    PanelAccessoryLease accessory;
    QString attachmentError;
};
}
