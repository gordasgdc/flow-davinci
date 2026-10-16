// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include "panelsession.h"

class PanelControls : public QWidget
{
public:
    PanelControls() : QWidget(nullptr, Qt::Tool)
    {
        setObjectName(QStringLiteral("unboundPanelTest"));
        setWindowTitle(QStringLiteral("Unbound — panel tests"));
        Unbound::logEvent(QStringLiteral("controls.create"));
        setAttribute(Qt::WA_QuitOnClose, false);
        auto layout = new QVBoxLayout(this);
        status = new QLabel(QStringLiteral("Use the Edit page. Enable dual viewers for Source Monitor."), this);
        status->setWordWrap(true);
        layout->addWidget(status);
        struct Target { const char *label; const char *name; };
        const Target targets[] = {
            {"Inspector", "widgetEditInspector"},
            {"Timeline", "frameConformTimelines"},
            {"Source Monitor", "widgetEditSourcePlayer"},
            {"Program Monitor", "widgetEditTimelinePlayer"}
        };
        for (const auto &target : targets) {
            auto session = new Unbound::MacPanelSession(this);
            sessions.append(session);
            auto row = new QHBoxLayout;
            auto undock = new QPushButton(QStringLiteral("Undock %1").arg(target.label), this);
            auto dock = new QPushButton(QStringLiteral("Dock %1").arg(target.label), this);
            undock->setObjectName(QStringLiteral("undock_%1").arg(target.name));
            dock->setObjectName(QStringLiteral("dock_%1").arg(target.name));
            dock->setEnabled(false);
            session->changed = [session, undock, dock] {
                undock->setEnabled(!session->active());
                dock->setEnabled(session->active());
            };
            row->addWidget(undock); row->addWidget(dock);
            layout->addLayout(row);
            connect(undock, &QPushButton::clicked, this, [this, session, target] {
                Unbound::logEvent(QStringLiteral("button.click"), {{"action", "undock"}, {"panel", target.label}});
                QWidget *page = nullptr;
                for (QWidget *window : QApplication::topLevelWidgets()) {
                    if (window->objectName() == QLatin1String("UiMainWindow")) {
                        page = window->findChild<QWidget *>(QStringLiteral("WStackPage_Conform"));
                        break;
                    }
                }
                auto panel = page ? page->findChild<QWidget *>(QLatin1String(target.name)) : nullptr;
                const QString error = session->undock(panel, page, QLatin1String(target.label));
                Unbound::logEvent(QStringLiteral("button.result"), {{"action", "undock"}, {"panel", target.label},
                    {"success", error.isEmpty()}, {"error", error}});
                status->setText(error.isEmpty() ? QStringLiteral("%1 is floating.").arg(target.label) : error);
            });
            connect(dock, &QPushButton::clicked, this, [this, session, target] {
                Unbound::logEvent(QStringLiteral("button.click"), {{"action", "dock"}, {"panel", target.label}});
                const QString error = session->dock();
                Unbound::logEvent(QStringLiteral("button.result"), {{"action", "dock"}, {"panel", target.label},
                    {"success", error.isEmpty()}, {"error", error}});
                status->setText(error.isEmpty() ? QStringLiteral("%1 returned to Resolve.").arg(target.label) : error);
            });
        }
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this] { dockAll(); });
    }
protected:
    void closeEvent(QCloseEvent *event) override { event->setAccepted(dockAll()); }
private:
    bool dockAll()
    {
        bool success = true;
        for (auto session : sessions) {
            if (!session->active()) continue;
            const QString error = session->dock();
            if (!error.isEmpty()) { status->setText(error); success = false; }
        }
        return success;
    }
    QLabel *status;
    QList<Unbound::MacPanelSession *> sessions;
};

