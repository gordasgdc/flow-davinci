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
#include "panelgroup.h"
#include "interactiondiagnostics.h"
#include "controlscroll.h"
#include "viewerpreparation.h"
#include "footerpanel.h"
#include "panelexpansion.h"
#include "workspacecallers.h"
#include <QGroupBox>

class PanelControls : public QWidget
{
    struct Panel {
        QString label, name, toggle;
        bool contents = false, desired = false, pending = false;
        unsigned requestVersion = 0;
        Unbound::MacPanelSession *session = nullptr;
        QPushButton *undock = nullptr, *dock = nullptr, *show = nullptr, *hide = nullptr, *expand = nullptr, *center = nullptr;
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
        auto scroll = new Unbound::ControlScroll(this); optionsScroll = scroll;
        scroll->setObjectName(QStringLiteral("panelOptionsScroll"));
        scroll->setWidgetResizable(true);
        auto body = new QWidget(scroll);
        auto bodyLayout = new QVBoxLayout(body);
        auto options = new QVBoxLayout;
        pin = new QCheckBox(QStringLiteral("Keep controls on top"), this);
        pin->setObjectName(QStringLiteral("pinControls"));
        pin->setChecked(settings.data["pin_controls"].toBool());
        constrain = new QCheckBox(QStringLiteral("Keep a window grip reachable"), this);
        constrain->setChecked(settings.data["keep_on_screen"].toBool(true));
        options->addWidget(pin); options->addWidget(constrain); bodyLayout->addLayout(options);
        snap = new QCheckBox(QStringLiteral("Snap window edges (Option bypasses)"), this);
        snap->setObjectName(QStringLiteral("snapWindows"));
        snap->setChecked(settings.data["snap_windows"].toBool(true));
        options->addWidget(snap);
        background = new QCheckBox(QStringLiteral("Hide empty Resolve background"), this);
        background->setObjectName(QStringLiteral("hideResolveBackground"));
        background->setChecked(settings.data["hide_background"].toBool()); options->addWidget(background);
        connect(background, &QCheckBox::toggled, this, [this](bool hidden) {
            action(QStringLiteral("background-visibility"));
            if (!applyBackgroundVisibility(hidden)) { const QSignalBlocker blocker(background); background->setChecked(!hidden); }
            save();
        });
        connect(snap, &QCheckBox::toggled, this, [this](bool enabled) {
            action(QStringLiteral("snap-windows"));
            for (auto &panel : panels) panel.session->setSnapping(enabled);
            for (auto group : groups) group->setSnapping(enabled);
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
            for (auto group : groups) group->setChrome(static_cast<Unbound::WindowChrome>(value));
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
                    else panel.session->centerOnCurrentDisplay();
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
        pendingGroups = settings.data["groups"].toArray();
        nativeMenus = new Unbound::PanelMenus(this);
        auto groupBox = new QGroupBox(QStringLiteral("Join panels or their groups"), body);
        auto groupLayout = new QGridLayout(groupBox);
        auto firstPanel = new QComboBox(groupBox), secondPanel = new QComboBox(groupBox);
        firstPanel->setObjectName(QStringLiteral("groupFirst")); secondPanel->setObjectName(QStringLiteral("groupSecond"));
        for (const auto &panel : panels) { firstPanel->addItem(panel.label); secondPanel->addItem(panel.label); }
        firstPanel->setCurrentIndex(2); secondPanel->setCurrentIndex(3);
        groupLayout->addWidget(firstPanel, 0, 0); groupLayout->addWidget(secondPanel, 0, 1);
        for (int i = 0; i < 4; ++i) {
            auto button = new QPushButton(i == 0 ? QStringLiteral("Side by Side") : i == 1 ? QStringLiteral("Stack") : i == 2 ? QStringLiteral("Release Groups") : QStringLiteral("Tabs"), groupBox);
            button->setObjectName(QStringLiteral("group_%1").arg(i));
            groupLayout->addWidget(button, i >= 2 ? 2 : 1, i >= 2 ? i - 2 : i);
            connect(button, &QPushButton::clicked, this, [this, firstPanel, secondPanel, i] {
                action(QStringLiteral("group-%1").arg(i));
                if (i == 2) { releaseGroups(); save(); return; }
                const int a = firstPanel->currentIndex(), b = secondPanel->currentIndex();
                if (a == b) { status->setText(QStringLiteral("Choose two different panels.")); return; }
                restoreExpanded();
                requestUndock(a, true); requestUndock(b, true);
                const unsigned request = ++groupVersion;
                QTimer::singleShot(100, this, [this, a, b, i, request] {
                    if (request != groupVersion) return;
                    const auto error = combinePanels(a, b, i == 3 ? Qt::Orientation(0) : i == 0 ? Qt::Horizontal : Qt::Vertical);
                    status->setText(error.isEmpty() ? QStringLiteral("Panels joined. Drag the group grip; resize the shared divider.") : error);
                    save();
                });
            });
        }
        bodyLayout->addWidget(groupBox);
        layout->addWidget(scroll, 1);
        scroll->setWidget(body);
        auto grid = new QGridLayout; bodyLayout->addLayout(grid);
        for (int i = 0; i < panels.size(); ++i) {
            auto &panel = panels[i];
            panel.saved = savedPanels[panel.name].toObject();
            panel.desired = panel.saved["floating"].toBool();
            panel.session = new Unbound::MacPanelSession(this);
            panel.session->attachRequested = [this, i](QWidget *target, Qt::Edge edge) {
                attachDrop(i, target, edge);
            };
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
            panel.center = new QPushButton(QStringLiteral("Center"), this);
            panel.center->setObjectName(QStringLiteral("center_") + panel.name);
            panel.expand->setObjectName(QStringLiteral("expand_") + panel.name);
            panel.undock->setObjectName(QStringLiteral("undock_") + panel.name);
            panel.dock->setObjectName(QStringLiteral("dock_") + panel.name);
            panel.show->setObjectName(QStringLiteral("show_") + panel.name);
            panel.hide->setObjectName(QStringLiteral("hide_") + panel.name);
            grid->addWidget(panel.undock, i, 1); grid->addWidget(panel.dock, i, 2);
            grid->addWidget(panel.show, i, 3); grid->addWidget(panel.hide, i, 4);
            grid->addWidget(panel.expand, i, 5);
            grid->addWidget(panel.center, i, 6);
            panel.session->changed = [this, i] {
                if (expanded == i && !panels[i].session->active()) expanded = -1;
                if (!quitting) panels[i].desired = panels[i].session->active() && !panels[i].session->dockPending();
                if (auto group = groupFor(i)) group->syncVisibility();
                syncNativeToggle(i);
                updateRow(i); if (!quitting) saveTimer.start();
            };
            panel.session->expandedClosed = [this, i] { if (expanded == i) restoreExpanded(); };
            panel.session->arrangementChanged = [this] { if (!quitting) saveTimer.start(); };
            connect(panel.undock, &QPushButton::clicked, this, [this, i] { action("undock", panels[i].label); requestUndock(i); });
            connect(panel.dock, &QPushButton::clicked, this, [this, i] { action("dock", panels[i].label); dockOne(i); });
            connect(panel.show, &QPushButton::clicked, this, [this, i] { action("show", panels[i].label); panels[i].session->setHiddenByUser(false); if (auto group = groupFor(i)) group->syncVisibility(panels[i].session); });
            connect(panel.hide, &QPushButton::clicked, this, [this, i] { action("hide", panels[i].label); panels[i].session->setHiddenByUser(true); });
            connect(panel.expand, &QPushButton::clicked, this, [this, i] { toggleExpanded(i); });
            connect(panel.center, &QPushButton::clicked, this, [this, i] { action("center", panels[i].label); panels[i].session->centerOnCurrentDisplay(); });
            updateRow(i);
        }
        footer = new Unbound::FooterPanel(this, []() -> QWidget * {
            for (auto window : QApplication::topLevelWidgets()) if (window->objectName() == QLatin1String("UiMainWindow")) return window;
            return nullptr;
        }, settings.data["footer"].toObject());
        footer->changed = [this] { saveTimer.start(); };
        callers = new Unbound::WorkspaceCallers(this);
        for (int index : {1,2,3}) callers->add({QStringLiteral("unboundCaller_") + panels[index].name, panels[index].label,
            [this, index] { return panels[index].session->active() && !panels[index].session->hiddenByUser(); },
            [this, index] { if (panels[index].session->active()) togglePanelDoor(index); else requestUndock(index); }});
        callers->add({"unboundShowNativeWorkspace", "Show Native Workspace",
            [this] { return !background->isChecked(); }, [this] { background->setChecked(!background->isChecked()); }});
        callers->wire();
        auto footerRow = new QHBoxLayout;
        auto floatFooter = new QPushButton(QStringLiteral("Float Page Navigation"), body);
        floatFooter->setObjectName("floatPageNavigation"); footerRow->addWidget(floatFooter);
        auto dockFooter = new QPushButton(QStringLiteral("Dock Page Navigation"), body);
        dockFooter->setObjectName("dockPageNavigation"); footerRow->addWidget(dockFooter);
        connect(floatFooter, &QPushButton::clicked, this, [this] { status->setText(footer->floatPanel()); });
        connect(dockFooter, &QPushButton::clicked, this, [this] { footer->dock(); });
        bodyLayout->addLayout(footerRow);
        bodyLayout->addStretch();
        setMinimumSize(280, 200);
        saveTimer.setSingleShot(true); saveTimer.setInterval(250);
        connect(&saveTimer, &QTimer::timeout, this, [this] { save(); });
        restoreTimer.setSingleShot(true); restoreTimer.setInterval(250);
        connect(&restoreTimer, &QTimer::timeout, this, [this] { restore(); });
        qApp->installEventFilter(this);
        installEventFilter(this);
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this] { shutdown(); });
        connect(qApp, &QGuiApplication::screenRemoved, this, [this] { keepGripsReachable(); });
        auto watchScreen = [this](QScreen *screen) {
            connect(screen, &QScreen::availableGeometryChanged, this, [this] { if (constrain->isChecked()) keepGripsReachable(); });
        };
        for (auto screen : QGuiApplication::screens()) watchScreen(screen);
        connect(qApp, &QGuiApplication::screenAdded, this, [this, watchScreen](QScreen *screen) { watchScreen(screen); keepGripsReachable(); });
        const QRect position = Unbound::WorkspaceSettings::rectangle(settings.data["controls"].toObject());
        if (position.isValid()) setGeometry(position);
        restoreTimer.start();
        wireNativeToggles();
    }
    ~PanelControls() override
    {
        shutdown();
        qDeleteAll(groups); groups.clear();
        delete nativeMenus;
        qApp->removeEventFilter(this);
        for (auto &panel : panels) { panel.session->expandedClosed = {}; panel.session->changed = {}; panel.session->arrangementChanged = {}; panel.session->attachRequested = {}; }
    }
protected:
    void closeEvent(QCloseEvent *event) override
    {
        action("hide-controls"); save(); event->ignore(); hide();
    }
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (quitting) return false;
        if ((event->type() == QEvent::ShortcutOverride || event->type() == QEvent::KeyPress) &&
            !QApplication::activeModalWidget() && !QApplication::activePopupWidget() &&
            !Unbound::textEditor(QApplication::focusWidget())) {
            auto key = static_cast<QKeyEvent *>(event);
            if ((key->key() == Qt::Key_QuoteLeft || key->key() == Qt::Key_AsciiTilde) &&
                !(key->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier))) {
                int target = expanded;
                auto hovered = QApplication::widgetAt(QCursor::pos());
                if (target < 0) for (int i = 0; i < panels.size(); ++i) {
                    QWidget *surface = panels[i].session->active() ? panels[i].session->window() : page() ? page()->findChild<QWidget *>(panels[i].name) : nullptr;
                    bool headerMatch = false;
                    for (auto ancestor = hovered; ancestor; ancestor = ancestor->parentWidget())
                        headerMatch |= ancestor->property("groupPanel").toString() == panels[i].name;
                    if (surface && surface->isVisible() && hovered && (surface == hovered || surface->isAncestorOf(hovered) || headerMatch)) { target = i; break; }
                }
                if (target >= 0) {
                    event->accept();
                    if (event->type() == QEvent::KeyPress && !key->isAutoRepeat()) toggleExpanded(target);
                    return true;
                }
            }
        }
        if (expanded >= 0 && event->type() == QEvent::Hide && object == page()) restoreExpanded();
        if (object == this && (event->type() == QEvent::Show || event->type() == QEvent::Resize || event->type() == QEvent::WindowTitleChange)) QTimer::singleShot(0, this, [this] {
            QPointer<PanelControls> alive(this);
            Unbound::setNativePin(this, pin->isChecked(), [alive](bool value) { if (alive) alive->pin->setChecked(value); });
        });
        if (object == this && (event->type() == QEvent::Move || event->type() == QEvent::Resize)) saveTimer.start();
        if (event->type() == QEvent::Show) {
            QTimer::singleShot(0, this, [this] { wireNativeToggles(); });
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
    void togglePanelDoor(int i)
    {
        auto session = panels[i].session;
        if (!session->active() || !page() || !page()->isVisible()) return;
        if (expanded >= 0) restoreExpanded();
        auto group = groupFor(i);
        const bool reveal = session->hiddenByUser() || (group && !group->selected(session));
        session->setHiddenByUser(!reveal);
        if (group) group->syncVisibility(reveal ? session : nullptr);
        syncNativeToggle(i);
        Unbound::logEvent(QStringLiteral("panel.visibility.native-toggle"), {{"panel", panels[i].name}, {"visible", reveal}});
    }
    void syncNativeToggle(int i)
    {
        if (!panels[i].session->active()) return;
        const QString base = QStringLiteral("btnEditToggle") + panels[i].toggle;
        if (panels[i].toggle.isEmpty() || !page()) return;
        for (auto button : page()->findChildren<QAbstractButton *>()) if (button->objectName().startsWith(base + "In")) {
            QSignalBlocker block(button); button->setChecked(!panels[i].session->hiddenByUser());
        }
    }
    void wireNativeToggles()
    {
        auto root = page(); if (!root || quitting) return;
        for (int i = 0; i < panels.size(); ++i) {
            if (panels[i].toggle.isEmpty()) continue;
            const QString base = QStringLiteral("btnEditToggle") + panels[i].toggle;
            for (auto button : root->findChildren<QAbstractButton *>()) {
                if (!button->objectName().startsWith(base + "In") || observedToggles.contains(button)) continue;
                observedToggles.insert(button);
                connect(button, &QObject::destroyed, this, [this, button] { observedToggles.remove(button); });
                const auto request = [this, i] {
                    if (!panels[i].session->active() || !page() || !page()->isVisible() || pendingNativeDoors.contains(i)) return;
                    pendingNativeDoors.insert(i);
                    // Clicks and native shortcut-driven check changes share one queued transition.
                    QTimer::singleShot(0, this, [this, i] { pendingNativeDoors.remove(i); togglePanelDoor(i); });
                };
                connect(button, &QAbstractButton::clicked, this, request);
                connect(button, &QAbstractButton::toggled, this, [this, i, request](bool checked) {
                    if (checked != !panels[i].session->hiddenByUser()) request();
                });
            }
        }
    }
    QJsonObject arrangement() const
    {
        auto result = settings.data;
        result.remove(QStringLiteral("controls"));
        result.remove(QStringLiteral("selected_arrangement"));
        return result;
    }
    static QJsonObject normalizedArrangement(QJsonObject value)
    {
        value["hide_background"] = value.value("hide_background").toBool();
        value["snap_windows"] = value.value("snap_windows").toBool(true);
        QJsonArray layouts;
        for (auto saved : value.value("groups").toArray()) {
            auto group = saved.toObject(); const auto tree = Unbound::PanelGroup::savedTree(group);
            QStringList names; Unbound::groupTreeNames(tree, names);
            group["pinned"] = group.value("pinned").toBool();
            group["tree"] = tree; group["panels"] = QJsonArray::fromStringList(names);
            group.remove("orientation"); group.remove("sizes"); layouts.append(group);
        }
        value["groups"] = layouts;
        auto savedPanels = value["panels"].toObject();
        for (auto entry = savedPanels.begin(); entry != savedPanels.end(); ++entry) {
            auto panel = entry.value().toObject(); panel["pinned"] = panel.value("pinned").toBool(); entry.value() = panel;
        }
        value["panels"] = savedPanels; return value;
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
            normalizedArrangement(saved["workspace"].toObject()) == normalizedArrangement(arrangement()) ? QStringLiteral("Matches saved arrangement") : QStringLiteral("Modified — use Update to save changes"));
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
        ++groupVersion;
        restoreExpanded();
        dockAll();
        for (const auto &panel : panels) if (panel.session->active()) {
            status->setText(QStringLiteral("Could not restore: a panel could not be docked safely."));
            Unbound::logEvent(QStringLiteral("arrangement.restore.failed"), {{"panel", panel.name}}); return;
        }
        const auto value = entry["workspace"].toObject();
        if (footer) footer->setState(value["footer"].toObject());
        { const QSignalBlocker blocker(background); background->setChecked(value["hide_background"].toBool()); }
        pendingGroups = value["groups"].toArray();
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
        status->setText(missing.isEmpty() ? (pendingGroups.isEmpty() ? QStringLiteral("Arrangement restored.") : QStringLiteral("Restoring panel groups…")) : QStringLiteral("Unavailable: ") + missing.join(", "));
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
        optionsScroll->preservePosition([&] {
            p.undock->setEnabled(!active && !p.pending);
            p.dock->setEnabled(active && !p.session->dockPending());
            p.dock->setText(p.session->dockPending() ? QStringLiteral("Pending") : QStringLiteral("Dock"));
            p.show->setEnabled(active); p.hide->setEnabled(active && !p.session->hiddenByUser());
            p.expand->setText(expanded == i ? QStringLiteral("Restore Size") : QStringLiteral("Expand"));
            p.expand->setEnabled(true);
            p.center->setEnabled(active);
        });
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
        expandedWasGrouped = panel.session->grouped();
        expandedGroup = groupFor(i);
        beforeExpandedGroup = expandedGroup ? expandedGroup->snapshot() : QJsonObject();
        if (expandedWasGrouped && !panel.session->beginGroupedExpansion()) return;
        expanded = i;
        const unsigned request = ++expandVersion;
        if (!expandedWasFloating) requestUndock(i, true);
        QTimer::singleShot(100, this, [this, i, request] {
            if (expanded != i || request != expandVersion) return;
            auto host = expandedWasGrouped ? panels[i].session->expandedWindow() : panels[i].session->window();
            if (!host) { expanded = -1; updateRow(i); save(); return; }
            auto screen = host->screen();
            if (!screen) screen = QApplication::primaryScreen();
            const auto margins = host->windowHandle() ? host->windowHandle()->frameMargins() : QMargins();
            host->setGeometry(screen->availableGeometry().adjusted(margins.left(), margins.top(), -margins.right(), -margins.bottom()));
            panels[i].session->setHiddenByUser(false);
            expansion.foreground(host);
            updateRow(i);
            Unbound::logEvent(QStringLiteral("panel.expand"), {{"panel", panels[i].name}, {"originally_floating", expandedWasFloating}});
        });
        updateRow(i);
    }
    void restoreExpanded()
    {
        if (expanded < 0) return;
        const int i = expanded;
        expansion.restore();
        ++expandVersion;
        expanded = -1;
        auto &panel = panels[i];
        ++panel.requestVersion; panel.pending = false;
        QString error;
        if (expandedWasGrouped) {
            panel.session->endGroupedExpansion();
            if (expandedGroup && expandedGroup->active()) expandedGroup->restore(beforeExpandedGroup);
        }
        expandedGroup = nullptr;
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
        if (p.name == QLatin1String("widgetEditSourcePlayer") && root && root->isVisible() &&
            Unbound::prepareSourceViewer(root, target)) {
            p.pending = true; updateRow(i);
            const unsigned request = ++p.requestVersion;
            QTimer::singleShot(100, this, [this, i, request] {
                if (panels[i].requestVersion != request) return;
                panels[i].pending = false; finishUndock(i);
            });
            return;
        }
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
        Unbound::logEvent(QStringLiteral("panel.undock.preconditions"), {{"panel", p.name},
            {"page_found", bool(root)}, {"page_visible", root && root->isVisible()}, {"target_found", bool(target)},
            {"target_visible", target && target->isVisible()}, {"target_in_page", root && target && root->isAncestorOf(target)}});
        if (p.name == QLatin1String("widgetEditSourcePlayer")) target = Unbound::sourceViewerSurface(target);
        const QString error = p.contents ? p.session->undockContents(target, root, p.label) : p.session->undock(target, root, p.label);
        if (error.isEmpty()) {
            p.session->window()->setObjectName(QStringLiteral("unboundFloating_") + p.name);
            p.desired = true;
            const QRect position = Unbound::WorkspaceSettings::rectangle(p.saved);
            if (position.isValid()) p.session->setFloatingGeometry(position);
            p.session->setPinned(p.saved["pinned"].toBool());
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
    void dockAll() { if (footer) footer->dock(); background->setChecked(false); ++groupVersion; pendingGroups = {}; for (int i = panels.size() - 1; i >= 0; --i) dockOne(i); }
    bool applyBackgroundVisibility(bool hidden)
    {
        auto root = page(); if (!root) return !hidden;
        QList<QWidget *> floating;
        for (auto widget : QApplication::topLevelWidgets()) if (widget->isVisible() && widget->objectName().startsWith("unboundFloating_")) floating.append(widget);
        if (hidden && floating.isEmpty()) { status->setText(QStringLiteral("Undock a panel before hiding the background.")); return false; }
        const bool success = Unbound::setNativeBackgroundHidden(root->window(), floating, hidden);
        Unbound::logEvent(QStringLiteral("background.visibility"), {{"hidden", hidden}, {"success", success}});
        if (!success) status->setText(QStringLiteral("Could not hide the background independently; it remains visible."));
        return success;
    }
    QString createGroup(int a, int b, Qt::Orientation orientation, const QJsonObject &saved = {})
    {
        auto group = new Unbound::PanelGroup(this);
        const auto error = group->join(panels[a].session, panels[b].session, orientation);
        if (!error.isEmpty()) { delete group; return error; }
        configureGroup(group);
        if (!saved.isEmpty()) group->restore(saved);
        return {};
    }
    void configureGroup(Unbound::PanelGroup *group)
    {
        groups.append(group);
        group->setChrome(static_cast<Unbound::WindowChrome>(style->currentIndex()));
        group->setSnapping(snap->isChecked());
        group->attachRequested = [this, group](QWidget *target, Qt::Edge edge) {
            for (int i = 0; i < panels.size(); ++i) if (group->contains(panels[i].session)) { attachDrop(i, target, edge); return; }
        };
        group->changed = [this] {
            for (int i = 0; i < panels.size(); ++i) updateRow(i);
            if (!quitting) saveTimer.start();
        };
        for (int i = 0; i < panels.size(); ++i) updateRow(i);
    }
    Unbound::PanelGroup *groupFor(int panel) const
    { for (auto group : groups) if (group->contains(panels[panel].session)) return group; return nullptr; }
    QString combinePanels(int a, int b, Qt::Orientation orientation, bool before = true, const QString &targetLeaf = {})
    {
        auto left = groupFor(a), right = groupFor(b);
        if (left && left == right) return QStringLiteral("Those panels already share a group.");
        if (right && left) return right->merge(left, orientation, before, targetLeaf);
        if (right) return right->add(panels[a].session, orientation, before, targetLeaf);
        if (left) return left->add(panels[b].session, orientation, !before);
        return before ? createGroup(a, b, orientation) : createGroup(b, a, orientation);
    }
    void attachDrop(int source, QWidget *target, Qt::Edge edge)
    {
        if (quitting || QApplication::activeModalWidget() || expanded >= 0 || !panels[source].session->active()) return;
        for (int j = 0; j < panels.size(); ++j) if (j != source && panels[j].session->active() &&
            (panels[j].session->window() == target || panels[j].session->window()->window() == target)) {
            if (groupFor(source) && groupFor(source) == groupFor(j)) return;
            const auto frame = target->window()->frameGeometry();
            const auto error = combinePanels(source, j, edge == Qt::Edge(0) ? Qt::Orientation(0) : edge == Qt::LeftEdge || edge == Qt::RightEdge ? Qt::Horizontal : Qt::Vertical,
                edge == Qt::LeftEdge || edge == Qt::TopEdge, target->isWindow() ? QString() : panels[j].name);
            if (error.isEmpty()) if (auto group = groupFor(source)) group->fitFrame(frame);
            status->setText(error.isEmpty() ? QStringLiteral("Panels joined. Drag a member header to detach.") : error);
            Unbound::logEvent(QStringLiteral("group.drop.result"), {{"success", error.isEmpty()}, {"error", error}});
            save(); return;
        }
    }
    void releaseGroups()
    {
        ++groupVersion; pendingGroups = {};
        for (auto group : groups) group->dissolve();
    }
    void restoreGroups()
    {
        if (quitting || !page() || !page()->isVisible()) return;
        const bool requested = !pendingGroups.isEmpty();
        for (int index = pendingGroups.size() - 1; index >= 0; --index) {
            const auto saved = pendingGroups[index].toObject();
            const auto tree = Unbound::PanelGroup::savedTree(saved); QStringList names;
            if (!Unbound::groupTreeNames(tree, names)) { status->setText(QStringLiteral("Invalid saved group layout; left unchanged.")); continue; }
            QList<Unbound::MacPanelSession *> available;
            for (const auto &name : names) for (const auto &panel : panels)
                if (panel.name == name && panel.session->active() && !panel.session->grouped()) available.append(panel.session);
            if (available.size() != names.size()) continue;
            auto group = new Unbound::PanelGroup(this);
            const auto error = group->initialize(available, tree, Unbound::WorkspaceSettings::rectangle(saved));
            if (error.isEmpty()) { configureGroup(group); group->restore(saved); pendingGroups.removeAt(index); }
            else { delete group; Unbound::logEvent(QStringLiteral("group.restore.pending"), {{"error", error}}); }
        }
        if (requested) status->setText(pendingGroups.isEmpty() ? QStringLiteral("Panel groups restored.") : QStringLiteral("Some groups are waiting for unavailable panels."));
        save();
    }
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
        if (footer) footer->restore();
        if (callers) callers->wire();
        QWidget *root = page();
        if (!root || !root->isVisible()) return;
        for (int i = 0; i < panels.size(); ++i)
            if (panels[i].desired && !panels[i].session->active()) requestUndock(i, true);
        if (!pendingGroups.isEmpty()) QTimer::singleShot(120, this, [this] { restoreGroups(); });
        if (background->isChecked()) QTimer::singleShot(200, this, [this] { if (!quitting && background->isChecked()) applyBackgroundVisibility(true); });
    }
    void keepGripsReachable()
    {
        Unbound::keepOnDisplay(this);
        for (auto &p : panels) if (p.session->active()) p.session->bringOnScreen();
    }
    void rescue()
    {
        Unbound::logEvent(QStringLiteral("workspace.recover")); Unbound::keepOnDisplay(this);
        for (auto &p : panels) if (p.session->active()) p.session->centerOnCurrentDisplay();
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
                value = Unbound::WorkspaceSettings::rectangle(p.session->floatingGeometry());
                value["floating"] = p.desired; value["hidden"] = p.session->hiddenByUser(); value["pinned"] = p.session->pinned();
            }
            if (expanded == i) value = beforeExpanded;
            p.saved = value; saved[p.name] = value;
        }
        QJsonArray savedGroups = pendingGroups;
        for (auto group : groups) if (group->active()) savedGroups.append(expanded >= 0 && group == expandedGroup ? beforeExpandedGroup : group->snapshot());
        settings.data = {{"version", 1}, {"pin_controls", pin->isChecked()}, {"groups", savedGroups},
            {"hide_background", background->isChecked()}, {"footer", footer ? footer->snapshot() : QJsonObject{}},
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
        restoreExpanded(); save(); quitting = true; saveTimer.stop(); restoreTimer.stop();
        if (auto root = page()) if (root->window()->property("unboundBackgroundHidden").toBool()) Unbound::setNativeBackgroundHidden(root->window(), {}, false);
        for (auto &p : panels) if (p.session->active()) p.session->dock();
    }
    Unbound::ControlScroll *optionsScroll = nullptr;
    QLabel *status = nullptr;
    QCheckBox *pin = nullptr, *constrain = nullptr;
    QCheckBox *snap = nullptr;
    QCheckBox *background = nullptr;
    QComboBox *style = nullptr;
    QCheckBox *hierarchyOption = nullptr;
    Unbound::WindowHierarchy *hierarchy = nullptr;
    Unbound::PanelMenus *nativeMenus = nullptr;
    QList<Panel> panels;
    QSet<QAbstractButton *> observedToggles;
    QSet<int> pendingNativeDoors;
    Unbound::WorkspaceSettings settings;
    Unbound::ArrangementStore arrangements;
    QComboBox *presets = nullptr;
    QLabel *presetState = nullptr;
    Unbound::PanelExpansion expansion;
    Unbound::WorkspaceCallers *callers = nullptr;
    Unbound::FooterPanel *footer = nullptr;
    QTimer saveTimer, restoreTimer;
    bool quitting = false;
    unsigned bulkVersion = 0;
    unsigned expandVersion = 0;
    int expanded = -1;
    bool expandedWasFloating = false, expandedWasGrouped = false;
    QJsonObject beforeExpanded, beforeExpandedGroup;
    QPointer<Unbound::PanelGroup> expandedGroup;
    QList<Unbound::PanelGroup *> groups;
    QJsonArray pendingGroups;
    unsigned groupVersion = 0;
};
