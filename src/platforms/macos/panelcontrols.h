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
#include <QScrollArea>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QWindow>
#include "panelsession.h"
#include "workspacesettings.h"
#include "windowhierarchy.h"
#include "arrangementstore.h"
#include "panelmenus.h"

class PanelControls : public QWidget
{
    struct Panel {
        QString label, name, toggle;
        bool contents = false, desired = false, pending = false;
        unsigned requestVersion = 0;
        Unbound::MacPanelSession *session = nullptr;
        QPushButton *undock = nullptr, *dock = nullptr, *show = nullptr, *hide = nullptr, *expand = nullptr;
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
        auto presetRow = new QHBoxLayout;
        presets = new QComboBox(this); presets->setObjectName(QStringLiteral("arrangements"));
        presets->setMinimumWidth(0); presets->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        presetRow->addWidget(presets, 1);
        auto restorePreset = new QPushButton(QStringLiteral("Restore"), this);
        restorePreset->setObjectName(QStringLiteral("restoreArrangement"));
        presetRow->addWidget(restorePreset);
        auto manage = new QPushButton(QStringLiteral("Manage"), this);
        auto menu = new QMenu(manage); manage->setMenu(menu); presetRow->addWidget(manage);
        for (const auto &operation : {"Save As…", "Update", "Rename…", "Duplicate…", "Delete"}) {
            auto command = menu->addAction(QString::fromUtf8(operation));
            connect(command, &QAction::triggered, this, [this, operation] { manageArrangement(QString::fromUtf8(operation)); });
        }
        layout->addLayout(presetRow);
        presetState = new QLabel(this); presetState->setObjectName(QStringLiteral("arrangementState")); layout->addWidget(presetState);
        reloadPresets(settings.data["selected_arrangement"].toString());
        connect(presets, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this] { save(); });
        connect(restorePreset, &QPushButton::clicked, this, [this] { restoreArrangement(); });
        auto scroll = new QScrollArea(this);
        scroll->setObjectName(QStringLiteral("panelOptionsScroll"));
        scroll->setWidgetResizable(true);
        auto body = new QWidget(scroll);
        auto bodyLayout = new QVBoxLayout(body);
        auto options = new QVBoxLayout;
        pin = new QCheckBox(QStringLiteral("Keep controls on top"), this);
        pin->setObjectName(QStringLiteral("pinControls"));
        pin->setChecked(settings.data["pin_controls"].toBool());
        constrain = new QCheckBox(QStringLiteral("Keep windows on screen"), this);
        constrain->setChecked(settings.data["keep_on_screen"].toBool(true));
        options->addWidget(pin); options->addWidget(constrain); bodyLayout->addLayout(options);
        snap = new QCheckBox(QStringLiteral("Snap window edges (Option bypasses)"), this);
        snap->setObjectName(QStringLiteral("snapWindows"));
        snap->setChecked(settings.data["snap_windows"].toBool(true));
        options->addWidget(snap);
        connect(snap, &QCheckBox::toggled, this, [this](bool enabled) {
            action(QStringLiteral("snap-windows"));
            for (auto &panel : panels) panel.session->setSnapping(enabled);
            save();
        });
        hierarchy = new Unbound::WindowHierarchy(this);
        hierarchyOption = new QCheckBox(QStringLiteral("Use panel hierarchy"), this);
        hierarchyOption->setObjectName(QStringLiteral("panelHierarchy"));
        hierarchyOption->setChecked(settings.data["panel_hierarchy"].toBool(true));
        hierarchy->setEnabled(hierarchyOption->isChecked());
        bodyLayout->addWidget(hierarchyOption);
        connect(hierarchyOption, &QCheckBox::toggled, this, [this](bool enabled) {
            action("panel-hierarchy"); hierarchy->setEnabled(enabled); save();
        });
        auto styleRow = new QHBoxLayout;
        styleRow->addWidget(new QLabel(QStringLiteral("Panel title bars"), this));
        style = new QComboBox(this); style->setObjectName(QStringLiteral("windowChrome"));
        style->addItems({"Standard", "Compact traffic lights", "Hidden — drag grip and resize edges"});
        style->setCurrentIndex(qBound(0, settings.data["window_chrome"].toInt(), 2));
        styleRow->addWidget(style); bodyLayout->addLayout(styleRow);
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
        const QStringList actions = {"Undock All", "Dock All", "Show All", "Hide All", "Recover Windows", QString::fromUtf8("Quick Export…")};
        for (int i = 0; i < actions.size(); ++i) {
            auto button = new QPushButton(actions[i], this);
            button->setObjectName(QStringLiteral("bulk_%1").arg(i));
            bulk->addWidget(button, i / 2, i % 2);
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
        nativeMenus = new Unbound::PanelMenus(this);
        layout->addWidget(scroll, 1);
        scroll->setWidget(body);
        auto grid = new QGridLayout; bodyLayout->addLayout(grid);
        for (int i = 0; i < panels.size(); ++i) {
            auto &panel = panels[i];
            panel.saved = savedPanels[panel.name].toObject();
            panel.desired = panel.saved["floating"].toBool();
            panel.session = new Unbound::MacPanelSession(this);
            nativeMenus->add({panel.name, [this, i] { return panels[i].session->active(); },
                [this, i] { if (panels[i].session->active()) dockOne(i); else requestUndock(i); }});
            panel.session->setChrome(static_cast<Unbound::WindowChrome>(style->currentIndex()));
            panel.session->setConstrained(constrain->isChecked());
            panel.session->setSnapping(snap->isChecked());
            grid->addWidget(new QLabel(panel.label, this), i, 0);
            panel.undock = new QPushButton(QStringLiteral("Undock"), this);
            panel.dock = new QPushButton(QStringLiteral("Dock"), this);
            panel.show = new QPushButton(QStringLiteral("Show"), this);
            panel.hide = new QPushButton(QStringLiteral("Hide"), this);
            panel.expand = new QPushButton(QStringLiteral("Expand"), this);
            panel.expand->setObjectName(QStringLiteral("expand_") + panel.name);
            panel.undock->setObjectName(QStringLiteral("undock_") + panel.name);
            panel.dock->setObjectName(QStringLiteral("dock_") + panel.name);
            panel.show->setObjectName(QStringLiteral("show_") + panel.name);
            panel.hide->setObjectName(QStringLiteral("hide_") + panel.name);
            grid->addWidget(panel.undock, i, 1); grid->addWidget(panel.dock, i, 2);
            grid->addWidget(panel.show, i, 3); grid->addWidget(panel.hide, i, 4);
            grid->addWidget(panel.expand, i, 5);
            panel.session->changed = [this, i] {
                if (expanded == i && !panels[i].session->active()) expanded = -1;
                if (!quitting) panels[i].desired = panels[i].session->active() && !panels[i].session->dockPending();
                updateRow(i); if (!quitting) saveTimer.start();
            };
            panel.session->arrangementChanged = [this] { if (!quitting) saveTimer.start(); };
            connect(panel.undock, &QPushButton::clicked, this, [this, i] { action("undock", panels[i].label); requestUndock(i); });
            connect(panel.dock, &QPushButton::clicked, this, [this, i] { action("dock", panels[i].label); dockOne(i); });
            connect(panel.show, &QPushButton::clicked, this, [this, i] { action("show", panels[i].label); panels[i].session->setHiddenByUser(false); });
            connect(panel.hide, &QPushButton::clicked, this, [this, i] { action("hide", panels[i].label); panels[i].session->setHiddenByUser(true); });
            connect(panel.expand, &QPushButton::clicked, this, [this, i] { toggleExpanded(i); });
            updateRow(i);
        }
        bodyLayout->addStretch();
        setMinimumSize(280, 200);
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
        delete nativeMenus;
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
    QJsonObject arrangement() const
    {
        auto result = settings.data;
        result.remove(QStringLiteral("controls"));
        result.remove(QStringLiteral("selected_arrangement"));
        return result;
    }
    void reloadPresets(const QString &selected = {})
    {
        const QSignalBlocker blocker(presets);
        presets->clear(); presets->addItem(QStringLiteral("Unsaved arrangement"), QString());
        for (const auto &value : arrangements.list()) {
            const auto entry = value.toObject();
            presets->addItem(entry["name"].toString(), entry["id"].toString());
        }
        presets->setCurrentIndex(qMax(0, presets->findData(selected)));
    }
    void updatePresetState()
    {
        if (!presetState) return;
        const auto saved = arrangements.find(presets->currentData().toString());
        presetState->setText(!arrangements.error().isEmpty() ? arrangements.error() : saved.isEmpty() ?
            QStringLiteral("Current layout is not saved as a preset.") :
            saved["workspace"].toObject() == arrangement() ? QStringLiteral("Matches saved arrangement") : QStringLiteral("Modified — use Update to save changes"));
        presetState->setWordWrap(true);
    }
    void manageArrangement(const QString &operation)
    {
        action(QStringLiteral("arrangement-") + operation);
        save();
        QString id = presets->currentData().toString(), error;
        const auto entry = arrangements.find(id);
        if (operation != QString::fromUtf8("Save As…") && entry.isEmpty()) {
            status->setText(QStringLiteral("Select a saved arrangement first.")); return;
        }
        if (operation == "Update") error = arrangements.update(id, arrangement());
        else if (operation == "Delete") {
            if (QMessageBox::question(this, QStringLiteral("Delete arrangement"),
                QStringLiteral("Delete “%1”? Your current windows will stay in place.").arg(entry["name"].toString())) != QMessageBox::Yes) return;
            error = arrangements.remove(id); if (error.isEmpty()) id.clear();
        } else {
            bool accepted = false;
            QString initial = entry["name"].toString();
            if (operation == QString::fromUtf8("Duplicate…")) initial += QStringLiteral(" copy");
            const auto name = QInputDialog::getText(this, operation, QStringLiteral("Arrangement name"), QLineEdit::Normal, initial, &accepted);
            if (!accepted) return;
            if (operation == QString::fromUtf8("Rename…")) error = arrangements.rename(id, name);
            else error = arrangements.create(name, operation == QString::fromUtf8("Duplicate…") ? entry["workspace"].toObject() : arrangement(), id);
        }
        reloadPresets(id); save();
        status->setText(error.isEmpty() ? QStringLiteral("Arrangement saved.") : error);
        Unbound::logEvent(QStringLiteral("arrangement.result"), {{"operation", operation}, {"id", id}, {"success", error.isEmpty()}, {"error", error}});
    }
    void restoreArrangement()
    {
        action(QStringLiteral("arrangement-restore"));
        const auto entry = arrangements.find(presets->currentData().toString());
        if (entry.isEmpty()) { status->setText(QStringLiteral("Select a saved arrangement first.")); return; }
        if (!page() || !page()->isVisible()) { status->setText(QStringLiteral("Open the Edit page to restore an arrangement.")); return; }
        ++bulkVersion;
        restoreExpanded();
        dockAll();
        for (const auto &panel : panels) if (panel.session->active()) {
            status->setText(QStringLiteral("Could not restore: a panel could not be docked safely."));
            Unbound::logEvent(QStringLiteral("arrangement.restore.failed"), {{"panel", panel.name}}); return;
        }
        const auto value = entry["workspace"].toObject();
        pin->setChecked(value["pin_controls"].toBool());
        constrain->setChecked(value["keep_on_screen"].toBool(true));
        snap->setChecked(value["snap_windows"].toBool(true));
        hierarchyOption->setChecked(value["panel_hierarchy"].toBool(true));
        style->setCurrentIndex(qBound(0, value["window_chrome"].toInt(), 2));
        const auto saved = value["panels"].toObject();
        for (auto &panel : panels) {
            panel.saved = saved[panel.name].toObject();
            panel.desired = panel.saved["floating"].toBool();
        }
        restore(); save();
        QStringList missing;
        for (const auto &panel : panels) if (panel.desired && !panel.session->active() && !panel.pending) missing.append(panel.label);
        status->setText(missing.isEmpty() ? QStringLiteral("Arrangement restored.") : QStringLiteral("Unavailable: ") + missing.join(", "));
        Unbound::logEvent(QStringLiteral("arrangement.restore"), {{"id", entry["id"]}, {"unavailable", missing.join(", ")}});
    }
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
        p.dock->setEnabled(active && !p.session->dockPending());
        p.dock->setText(p.session->dockPending() ? QStringLiteral("Pending") : QStringLiteral("Dock"));
        p.show->setEnabled(active); p.hide->setEnabled(active && !p.session->hiddenByUser());
        p.expand->setText(expanded == i ? QStringLiteral("Restore Size") : QStringLiteral("Expand"));
    }
    void toggleExpanded(int i)
    {
        action(QStringLiteral("expand-toggle"), panels[i].label);
        if (expanded == i) { restoreExpanded(); return; }
        restoreExpanded();
        if (!page() || !page()->isVisible() || QApplication::activeModalWidget()) {
            status->setText(QStringLiteral("Open the Edit page and close dialogs before expanding a panel.")); return;
        }
        save();
        auto &panel = panels[i];
        beforeExpanded = panel.saved;
        expandedWasFloating = panel.session->active();
        expanded = i;
        const unsigned request = ++expandVersion;
        if (!expandedWasFloating) requestUndock(i, true);
        QTimer::singleShot(100, this, [this, i, request] {
            if (expanded != i || request != expandVersion) return;
            auto host = panels[i].session->window();
            if (!host) { expanded = -1; updateRow(i); save(); return; }
            auto screen = host->screen();
            if (!screen) screen = QApplication::primaryScreen();
            const auto margins = host->windowHandle() ? host->windowHandle()->frameMargins() : QMargins();
            host->setGeometry(screen->availableGeometry().adjusted(margins.left(), margins.top(), -margins.right(), -margins.bottom()));
            panels[i].session->setHiddenByUser(false);
            updateRow(i);
            Unbound::logEvent(QStringLiteral("panel.expand"), {{"panel", panels[i].name}, {"originally_floating", expandedWasFloating}});
        });
        updateRow(i);
    }
    void restoreExpanded()
    {
        if (expanded < 0) return;
        const int i = expanded;
        ++expandVersion;
        expanded = -1;
        auto &panel = panels[i];
        ++panel.requestVersion; panel.pending = false;
        QString error;
        if (panel.session->active()) {
            panel.session->setFloatingGeometry(Unbound::WorkspaceSettings::rectangle(beforeExpanded));
            panel.session->setHiddenByUser(beforeExpanded["hidden"].toBool());
            if (!expandedWasFloating) error = panel.session->dock();
        }
        panel.saved = beforeExpanded;
        panel.desired = panel.session->active() && !panel.session->dockPending();
        updateRow(i); save();
        Unbound::logEvent(QStringLiteral("panel.expand.restore"), {{"panel", panel.name}, {"success", error.isEmpty()}, {"error", error}});
        if (!error.isEmpty()) status->setText(error);
    }
    void outcome(const QString &operation, int i, const QString &error)
    {
        Unbound::logEvent(QStringLiteral("button.result"), {{"action", operation}, {"panel", panels[i].label},
            {"success", error.isEmpty()}, {"error", error}});
        status->setText(error.isEmpty() ? panels[i].label + (operation == "dock" ?
            (panels[i].session->dockPending() ? " will dock when Edit returns." : " returned to Resolve.") : " is floating.")
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
        if (expanded == i) restoreExpanded();
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
        for (int i = 0; i < panels.size(); ++i) {
            auto &p = panels[i];
            QJsonObject value = p.saved;
            value["floating"] = p.desired;
            if (p.session->active()) {
                value = Unbound::WorkspaceSettings::rectangle(p.session->window()->geometry());
                value["floating"] = p.desired; value["hidden"] = p.session->hiddenByUser();
            }
            if (expanded == i) value = beforeExpanded;
            p.saved = value; saved[p.name] = value;
        }
        settings.data = {{"version", 1}, {"pin_controls", pin->isChecked()},
            {"selected_arrangement", presets->currentData().toString()},
            {"window_chrome", style->currentIndex()},
            {"panel_hierarchy", hierarchyOption->isChecked()},
            {"snap_windows", snap->isChecked()},
            {"keep_on_screen", constrain->isChecked()}, {"controls", Unbound::WorkspaceSettings::rectangle(geometry())}, {"panels", saved}};
        const bool success = settings.save();
        if (!success) status->setText(QStringLiteral("Could not save the workspace. Check the diagnostic log."));
        Unbound::logEvent(QStringLiteral("workspace.save"), {{"success", success}});
        updatePresetState();
    }
    void shutdown()
    {
        if (quitting) return;
        save(); quitting = true; saveTimer.stop(); restoreTimer.stop();
        for (auto &p : panels) if (p.session->active()) p.session->dock();
    }
    QLabel *status = nullptr;
    QCheckBox *pin = nullptr, *constrain = nullptr;
    QCheckBox *snap = nullptr;
    QComboBox *style = nullptr;
    QCheckBox *hierarchyOption = nullptr;
    Unbound::WindowHierarchy *hierarchy = nullptr;
    Unbound::PanelMenus *nativeMenus = nullptr;
    QList<Panel> panels;
    Unbound::WorkspaceSettings settings;
    Unbound::ArrangementStore arrangements;
    QComboBox *presets = nullptr;
    QLabel *presetState = nullptr;
    QTimer saveTimer, restoreTimer;
    bool quitting = false;
    unsigned bulkVersion = 0;
    unsigned expandVersion = 0;
    int expanded = -1;
    bool expandedWasFloating = false;
    QJsonObject beforeExpanded;
};
