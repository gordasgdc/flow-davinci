// SPDX-License-Identifier: MIT
#pragma once
#include <QApplication>
#include <QAction>
#include <QAbstractButton>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include "panelsession.h"
#include "workspacesettings.h"
#include "windowhierarchy.h"

class PanelControls : public QWidget
{
    struct Panel {
        QString label, name, toggle;
        bool contents = false, desired = false, pending = false;
        unsigned requestVersion = 0;
        Unbound::MacPanelSession *session = nullptr;
        QPushButton *undock = nullptr, *dock = nullptr, *show = nullptr, *hide = nullptr;
        QJsonObject saved = {};
    };
public:
    PanelControls() : QWidget(nullptr, Qt::Window)
    {
        setObjectName(QStringLiteral("unboundPanelTest"));
        setWindowTitle(QStringLiteral("Unbound — Panels"));
        setAttribute(Qt::WA_QuitOnClose, false);
        Unbound::logEvent(QStringLiteral("controls.create"));
        auto layout = new QVBoxLayout(this);
        status = new QLabel(QStringLiteral("Open the Edit page. Enable dual viewers for Source Monitor."), this);
        status->setWordWrap(true); layout->addWidget(status);
        auto options = new QHBoxLayout;
        pin = new QCheckBox(QStringLiteral("Keep controls on top"), this);
        pin->setObjectName(QStringLiteral("pinControls"));
        pin->setChecked(settings.data["pin_controls"].toBool());
        constrain = new QCheckBox(QStringLiteral("Keep windows on screen"), this);
        constrain->setChecked(settings.data["keep_on_screen"].toBool(true));
        options->addWidget(pin); options->addWidget(constrain); layout->addLayout(options);
        hierarchy = new Unbound::WindowHierarchy(this);
        hierarchyOption = new QCheckBox(QStringLiteral("Use panel hierarchy"), this);
        hierarchyOption->setObjectName(QStringLiteral("panelHierarchy"));
        hierarchyOption->setChecked(settings.data["panel_hierarchy"].toBool(true));
        hierarchy->setEnabled(hierarchyOption->isChecked());
        layout->addWidget(hierarchyOption);
        connect(hierarchyOption, &QCheckBox::toggled, this, [this](bool enabled) {
            action("panel-hierarchy"); hierarchy->setEnabled(enabled); save();
        });
        auto styleRow = new QHBoxLayout;
        styleRow->addWidget(new QLabel(QStringLiteral("Panel title bars"), this));
        style = new QComboBox(this); style->setObjectName(QStringLiteral("windowChrome"));
        style->addItems({"Standard", "Compact traffic lights", "Hidden — drag grip and resize edges"});
        style->setCurrentIndex(qBound(0, settings.data["window_chrome"].toInt(), 2));
        styleRow->addWidget(style); layout->addLayout(styleRow);
        connect(style, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int value) {
            action(QStringLiteral("window-style-%1").arg(value));
            for (auto &panel : panels) panel.session->setChrome(static_cast<Unbound::WindowChrome>(value));
            rescue(); save();
        });
        setWindowFlag(Qt::WindowStaysOnTopHint, pin->isChecked());
        connect(pin, &QCheckBox::toggled, this, [this](bool value) {
            action("pin-controls"); const bool visible = isVisible();
            setWindowFlag(Qt::WindowStaysOnTopHint, value);
            if (visible) { show(); raise(); }
            save();
        });
        connect(constrain, &QCheckBox::toggled, this, [this](bool value) {
            action("constrain-windows");
            for (auto &panel : panels) panel.session->setConstrained(value);
            save();
        });
        auto bulk = new QGridLayout;
        const QStringList actions = {"Undock All", "Dock All", "Show All", "Hide All", "Bring All On Screen", QString::fromUtf8("Quick Export…")};
        for (int i = 0; i < actions.size(); ++i) {
            auto button = new QPushButton(actions[i], this);
            button->setObjectName(QStringLiteral("bulk_%1").arg(i));
            bulk->addWidget(button, i / 3, i % 3);
            connect(button, &QPushButton::clicked, this, [this, i] {
                action(QStringLiteral("bulk-%1").arg(i));
                const unsigned command = ++bulkVersion;
                if (i == 0) {
                    for (int j = 0; j < panels.size(); ++j)
                        QTimer::singleShot(j * 80, this, [this, j, command] { if (command == bulkVersion) requestUndock(j); });
                    QTimer::singleShot(panels.size() * 80 + 200, this, [this, command] {
                        if (command != bulkVersion) return;
                        int count = 0; QStringList missing;
                        for (const auto &panel : panels) {
                            if (panel.session->active()) ++count;
                            else missing.append(panel.label);
                        }
                        status->setText(QStringLiteral("%1 of %2 panels floating.").arg(count).arg(panels.size()) +
                            (missing.isEmpty() ? QString() : QStringLiteral(" Not available: ") + missing.join(", ")));
                        Unbound::logEvent(QStringLiteral("bulk.undock.complete"), {{"floating", count}, {"total", panels.size()},
                            {"unavailable", missing.join(", ")}});
                    });
                } else if (i == 1) dockAll();
                else if (i == 5) quickExport();
                else for (auto &panel : panels) if (panel.session->active()) {
                    if (i == 2 || i == 3) panel.session->setHiddenByUser(i == 3);
                    else panel.session->bringOnScreen();
                }
                save();
            });
        }
        layout->addLayout(bulk);
        panels = {
            {"Inspector", "widgetEditInspector", "Inspector", false},
            {"Timeline", "frameConformTimelines", "", false},
            {"Source Monitor", "widgetEditSourcePlayer", "", false},
            {"Program Monitor", "widgetEditTimelinePlayer", "", false},
            {"Media Pool", "frameEditPageMediaPool", "MediaPool", true},
            {"Index", "frameEditPageEditIndex", "EditIndex", true},
            {"Effects", "frameEditPageEffectsLibrary", "EffectsLibrary", true},
            {"Metadata", "frameEditPageMetadata", "Metadata", true},
            {"Mixer", "frameEditAudioMeter", "Mixer", true},
            {"Sound Library", "frameEditPageSoundLibrary", "SoundLibrary", true},
            {"Keyframes", "frameEditPageKeyframeEditor", "KeyframeEditor", true}
        };
        const QJsonObject savedPanels = settings.data["panels"].toObject();
        auto grid = new QGridLayout; layout->addLayout(grid);
        for (int i = 0; i < panels.size(); ++i) {
            auto &panel = panels[i];
            panel.saved = savedPanels[panel.name].toObject();
            panel.desired = panel.saved["floating"].toBool();
            panel.session = new Unbound::MacPanelSession(this);
            panel.session->setChrome(static_cast<Unbound::WindowChrome>(style->currentIndex()));
            panel.session->setConstrained(constrain->isChecked());
            grid->addWidget(new QLabel(panel.label, this), i, 0);
            panel.undock = new QPushButton(QStringLiteral("Undock"), this);
            panel.dock = new QPushButton(QStringLiteral("Dock"), this);
            panel.show = new QPushButton(QStringLiteral("Show"), this);
            panel.hide = new QPushButton(QStringLiteral("Hide"), this);
            panel.undock->setObjectName(QStringLiteral("undock_") + panel.name);
            panel.dock->setObjectName(QStringLiteral("dock_") + panel.name);
            panel.show->setObjectName(QStringLiteral("show_") + panel.name);
            panel.hide->setObjectName(QStringLiteral("hide_") + panel.name);
            grid->addWidget(panel.undock, i, 1); grid->addWidget(panel.dock, i, 2);
            grid->addWidget(panel.show, i, 3); grid->addWidget(panel.hide, i, 4);
            panel.session->changed = [this, i] {
                if (!quitting) panels[i].desired = panels[i].session->active();
                updateRow(i); if (!quitting) saveTimer.start();
            };
            panel.session->arrangementChanged = [this] { if (!quitting) saveTimer.start(); };
            connect(panel.undock, &QPushButton::clicked, this, [this, i] { action("undock", panels[i].label); requestUndock(i); });
            connect(panel.dock, &QPushButton::clicked, this, [this, i] { action("dock", panels[i].label); dockOne(i); });
            connect(panel.show, &QPushButton::clicked, this, [this, i] { action("show", panels[i].label); panels[i].session->setHiddenByUser(false); });
            connect(panel.hide, &QPushButton::clicked, this, [this, i] { action("hide", panels[i].label); panels[i].session->setHiddenByUser(true); });
            updateRow(i);
        }
        saveTimer.setSingleShot(true); saveTimer.setInterval(250);
        connect(&saveTimer, &QTimer::timeout, this, [this] { save(); });
        restoreTimer.setSingleShot(true); restoreTimer.setInterval(250);
        connect(&restoreTimer, &QTimer::timeout, this, [this] { restore(); });
        qApp->installEventFilter(this);
        installEventFilter(this);
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this] { shutdown(); });
        connect(qApp, &QGuiApplication::screenRemoved, this, [this] { rescue(); });
        auto watchScreen = [this](QScreen *screen) {
            connect(screen, &QScreen::availableGeometryChanged, this, [this] { if (constrain->isChecked()) rescue(); });
        };
        for (auto screen : QGuiApplication::screens()) watchScreen(screen);
        connect(qApp, &QGuiApplication::screenAdded, this, [this, watchScreen](QScreen *screen) { watchScreen(screen); rescue(); });
        const QRect position = Unbound::WorkspaceSettings::rectangle(settings.data["controls"].toObject());
        if (position.isValid()) setGeometry(position);
        restoreTimer.start();
    }
    ~PanelControls() override
    {
        shutdown();
        qApp->removeEventFilter(this);
        for (auto &panel : panels) { panel.session->changed = {}; panel.session->arrangementChanged = {}; }
    }
protected:
    void closeEvent(QCloseEvent *event) override
    {
        action("hide-controls"); save(); event->ignore(); hide();
    }
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (quitting) return false;
        if (object == this && (event->type() == QEvent::Move || event->type() == QEvent::Resize)) saveTimer.start();
        if (event->type() == QEvent::Show) {
            auto widget = qobject_cast<QWidget *>(object);
            if (widget && (widget->objectName() == QLatin1String("UiMainWindow") ||
                widget->objectName() == QLatin1String("WStackPage_Conform"))) restoreTimer.start();
            else if (widget) for (const auto &panel : panels)
                if (panel.desired && !panel.pending && !panel.session->active() && widget->objectName() == panel.name)
                    restoreTimer.start();
        }
        return false;
    }
private:
    static void action(const QString &name, const QString &panel = {})
    { Unbound::logEvent(QStringLiteral("button.click"), {{"action", name}, {"panel", panel}}); }
    QWidget *page() const
    {
        for (QWidget *window : QApplication::topLevelWidgets())
            if (window->objectName() == QLatin1String("UiMainWindow"))
                return window->findChild<QWidget *>(QStringLiteral("WStackPage_Conform"));
        return nullptr;
    }
    void updateRow(int i)
    {
        auto &p = panels[i]; const bool active = p.session->active();
        p.undock->setEnabled(!active && !p.pending);
        p.dock->setEnabled(active); p.show->setEnabled(active); p.hide->setEnabled(active && !p.session->hiddenByUser());
    }
    void outcome(const QString &operation, int i, const QString &error)
    {
        Unbound::logEvent(QStringLiteral("button.result"), {{"action", operation}, {"panel", panels[i].label},
            {"success", error.isEmpty()}, {"error", error}});
        status->setText(error.isEmpty() ? panels[i].label + (operation == "dock" ? " returned to Resolve." : " is floating.")
            : panels[i].label + ": " + error);
    }
    void requestUndock(int i, bool restoring = false)
    {
        if (quitting) return;
        if (QApplication::activeModalWidget()) return;
        auto &p = panels[i];
        if (p.pending || p.session->active()) return;
        if (!restoring) p.saved["hidden"] = false;
        QWidget *root = page();
        QWidget *target = root ? root->findChild<QWidget *>(p.name) : nullptr;
        if (root && root->isVisible() && target && !target->isVisible() && !p.toggle.isEmpty()) {
            const QString base = QStringLiteral("btnEditToggle") + p.toggle;
            for (auto button : root->findChildren<QAbstractButton *>()) {
                if ((button->objectName() == base + "InSSM" || button->objectName() == base + "InDSM" ||
                    button->objectName() == base + "InEFTM") && button->isVisible() && button->isEnabled()) {
                    if (!button->isChecked()) button->click();
                    p.pending = true; updateRow(i);
                    const unsigned request = ++p.requestVersion;
                    QTimer::singleShot(50, this, [this, i, request] {
                        if (panels[i].requestVersion != request) return;
                        panels[i].pending = false; finishUndock(i);
                    });
                    return;
                }
            }
        }
        finishUndock(i);
    }
    void finishUndock(int i)
    {
        if (quitting) return;
        auto &p = panels[i]; QWidget *root = page();
        QWidget *target = root ? root->findChild<QWidget *>(p.name) : nullptr;
        const QString error = p.contents ? p.session->undockContents(target, root, p.label) : p.session->undock(target, root, p.label);
        if (error.isEmpty()) {
            p.desired = true;
            const QRect position = Unbound::WorkspaceSettings::rectangle(p.saved);
            if (position.isValid()) p.session->setFloatingGeometry(position);
            if (p.saved["hidden"].toBool()) p.session->setHiddenByUser(true);
        }
        outcome("undock", i, error); updateRow(i); save();
    }
    void dockOne(int i)
    {
        auto &p = panels[i];
        ++p.requestVersion; p.pending = false;
        const QString error = p.session->active() ? p.session->dock() : QString();
        if (error.isEmpty()) p.desired = false;
        outcome("dock", i, error); updateRow(i); save();
    }
    void dockAll() { for (int i = panels.size() - 1; i >= 0; --i) dockOne(i); }
    void quickExport()
    {
        auto root = page();
        if (root) for (auto item : root->window()->findChildren<QAction *>()) {
            QString text = item->text();
            text.remove('&'); text.remove(QChar(0x2026)); text.remove(QStringLiteral("..."));
            if ((text.trimmed().compare(QStringLiteral("Quick Export"), Qt::CaseInsensitive) == 0 ||
                item->objectName().compare(QStringLiteral("actionQuickExport"), Qt::CaseInsensitive) == 0) && item->isEnabled()) {
                Unbound::logEvent(QStringLiteral("quick-export.open.request"), {{"action", item->objectName()}});
                status->setText(QStringLiteral("Opening Resolve's Quick Export window."));
                item->trigger(); return;
            }
        }
        status->setText(QStringLiteral("Quick Export is not available. Open a timeline first."));
        Unbound::logEvent(QStringLiteral("quick-export.unavailable"));
    }
    void restore()
    {
        QWidget *root = page();
        if (!root || !root->isVisible()) return;
        for (int i = 0; i < panels.size(); ++i)
            if (panels[i].desired && !panels[i].session->active()) requestUndock(i, true);
    }
    void rescue()
    {
        Unbound::logEvent(QStringLiteral("workspace.recover")); Unbound::keepOnDisplay(this);
        for (auto &p : panels) if (p.session->active()) p.session->bringOnScreen();
    }
    void save()
    {
        if (quitting) return;
        QJsonObject saved;
        for (auto &p : panels) {
            QJsonObject value = p.saved;
            value["floating"] = p.desired;
            if (p.session->active()) {
                value = Unbound::WorkspaceSettings::rectangle(p.session->window()->geometry());
                value["floating"] = true; value["hidden"] = p.session->hiddenByUser();
            }
            p.saved = value; saved[p.name] = value;
        }
        settings.data = {{"version", 1}, {"pin_controls", pin->isChecked()},
            {"window_chrome", style->currentIndex()},
            {"panel_hierarchy", hierarchyOption->isChecked()},
            {"keep_on_screen", constrain->isChecked()}, {"controls", Unbound::WorkspaceSettings::rectangle(geometry())}, {"panels", saved}};
        const bool success = settings.save();
        if (!success) status->setText(QStringLiteral("Could not save the workspace. Check the diagnostic log."));
        Unbound::logEvent(QStringLiteral("workspace.save"), {{"success", success}});
    }
    void shutdown()
    {
        if (quitting) return;
        save(); quitting = true; saveTimer.stop(); restoreTimer.stop();
        for (auto &p : panels) if (p.session->active()) p.session->dock();
    }
    QLabel *status = nullptr;
    QCheckBox *pin = nullptr, *constrain = nullptr;
    QComboBox *style = nullptr;
    QCheckBox *hierarchyOption = nullptr;
    Unbound::WindowHierarchy *hierarchy = nullptr;
    QList<Panel> panels;
    Unbound::WorkspaceSettings settings;
    QTimer saveTimer, restoreTimer;
    bool quitting = false;
    unsigned bulkVersion = 0;
};
