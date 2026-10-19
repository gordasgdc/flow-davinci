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
#include "nativepaneldecoration.h"
#include "timelinecontrol.h"
#include "nativeworkspacesizing.h"
#include "nativepaneldrag.h"
#include "nativeworkspacebinding.h"
#include "panelgroup.h"
#include "interactiondiagnostics.h"
#include "controlscroll.h"
#include "viewerpreparation.h"
#include "surfacereadiness.h"
#include "footerpanel.h"
#include "panelexpansion.h"
#include "workspacecallers.h"
#include "toolbars.h"
#include "viewermodecommand.h"
#include "arrangementcontrols.h"
#include "workspacearrangements.h"
#include "pagecontext.h"
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
        setWindowTitle(QStringLiteral("Unbound Panel Manager"));
        setAttribute(Qt::WA_QuitOnClose, false);
        Unbound::logEvent(QStringLiteral("controls.create"));
        auto layout = new QVBoxLayout(this);
        status = new QLabel(QStringLiteral("Open a project on the Edit page. Source Monitor can float with no clip loaded."), this);
        status->setWordWrap(true); layout->addWidget(status);
        presetsUi = new Unbound::ArrangementControls(this, layout, settings.data["selected_arrangement"].toString());
        presets = presetsUi->selection();
        presetsUi->workspace = [this] { return arrangement(); };
        presetsUi->changed = [this] { save(); };
        presetsUi->restoreRequested = [this] { restoreArrangement(); };
        presetsUi->report = [this](QString message) { status->setText(message); };
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
        background = new QCheckBox(QStringLiteral("Floating workspace (hide native background)"), this);
        background->setObjectName(QStringLiteral("hideResolveBackground"));
        const auto nativeMode=settings.data["native_workspace_mode"].toString();
        background->setChecked(nativeMode=="floating" || (nativeMode.isEmpty() && settings.data["hide_background"].toBool(false))); options->addWidget(background);
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
        groupReserve = new Unbound::GroupHostReserve(this);
        nativeSizing = new Unbound::NativeWorkspaceSizing(this);
        nativeSizing->blocked = [this] { return quitting || expanded >= 0; };
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
                    if (nativeWorkspace && !nativeWorkspace->release()) { status->setText("Could not release the native arrangement safely."); return; }
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
                else for (int j=0;j<panels.size();++j) if (panels[j].session->active()) {
                    if (i == 2 || i == 3) showPanel(j,i==2);
                    else if (panels[j].session->floating()) panels[j].session->centerOnCurrentDisplay();
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
            {"Keyframes", "frameEditPageKeyframeEditor", "KeyframeEditor", true},
            {"Timeline Toolbar", "frameEditToolbarTop", "", false},
            {"Workspace Toolbar", "unboundWorkspaceToolbar", "", false}
        };
        const QJsonObject savedPanels = settings.data["panels"].toObject();
        pendingGroups = settings.data["groups"].toArray();
        nativeMenus = new Unbound::PanelMenus(this);
        nativeWorkspace = new Unbound::NativeWorkspaceBinding(this);
        nativeWorkspace->sectionDetached=[this](Unbound::PanelGroup *group) { configureGroup(group); };
        nativeWorkspace->beforeMount = [this] {
            if (nativeSizing) nativeSizing->restore();
            const QSignalBlocker block(background); background->setChecked(false); applyBackgroundVisibility(false);
        };
        nativeWorkspace->changed = [this] {
            for (int i=0;i<panels.size();++i) updateRow(i);
            nativeWorkspace->refreshNativeTargets(page());
            if (!quitting) saveTimer.start();
        };
        nativeWorkspace->failed = [this](const QString &error) { status->setText(error); };
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
                new Unbound::SurfaceReadiness(this,page(),
                    [this,request] { return !quitting && request==groupVersion; },
                    [this,a,b] { return panels[a].session->active() && panels[b].session->active() && !panels[a].pending && !panels[b].pending; },
                    [this, a, b, i, request](bool ready) {
                    if (quitting || request != groupVersion) return;
                    const auto error = combinePanels(a, b, i == 3 ? Qt::Orientation(0) : i == 0 ? Qt::Horizontal : Qt::Vertical);
                    Unbound::logEvent("group.prepare.result",{{"ready",ready},{"success",error.isEmpty()},{"error",error}});
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
            nativeWorkspace->registerPanel(panel.name,panel.session,panel.label);
            panel.session->attachRequested = [this, i](QWidget *target, Qt::Edge edge) {
                attachDrop(i, target, edge);
            };
            nativeMenus->add({panel.name, [this, i] { return panels[i].session->floating(); },
                [this, i] { togglePanelPlacement(i); }, [this,i] { dockNative(i); },
                [this,i] { return panels[i].session->floating(); }});
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
                nativeWorkspace->queueRestore(page());
                nativeWorkspace->refreshNativeTargets(page());
                syncNativeToggle(i);
                if (viewerMode && page()) viewerMode->refresh(page()->window());
                updateRow(i); updateNativeSizing(); readyNativeDrag(i); if (!quitting) saveTimer.start();
            };
            panel.session->expandedClosed = [this, i] { if (expanded == i) restoreExpanded(); };
            panel.session->arrangementChanged = [this] { if (!quitting) saveTimer.start(); };
            connect(panel.undock, &QPushButton::clicked, this, [this, i] { action("undock", panels[i].label); if (nativeWorkspace->contains(panels[i].name)) nativeWorkspace->detach(panels[i].name); else requestUndock(i); });
            connect(panel.dock, &QPushButton::clicked, this, [this, i] { action("dock", panels[i].label); dockOne(i); });
            connect(panel.show, &QPushButton::clicked, this, [this, i] { action("show", panels[i].label); showPanel(i,true); });
            connect(panel.hide, &QPushButton::clicked, this, [this, i] { action("hide", panels[i].label); showPanel(i,false); });
            connect(panel.expand, &QPushButton::clicked, this, [this, i] { toggleExpanded(i); });
            connect(panel.center, &QPushButton::clicked, this, [this, i] { action("center", panels[i].label); panels[i].session->centerOnCurrentDisplay(); });
            updateRow(i);
        }
        const auto nativePlanError=nativeWorkspace->setPending(settings.data["native_arrangement"].toObject(),pendingGroups);
        if (!nativePlanError.isEmpty()) status->setText(nativePlanError);
        for (auto &panel : panels) if (nativeWorkspace->requiredPanels().contains(panel.name)) panel.desired=true;
        footer = new Unbound::FooterPanel(this, []() -> QWidget * {
            for (auto window : QApplication::topLevelWidgets()) if (window->objectName() == QLatin1String("UiMainWindow")) return window;
            return nullptr;
        }, settings.data["footer"].toObject());
        footer->changed = [this] { saveTimer.start(); };
        viewerMode = new Unbound::ViewerModeCommand(this,
            [this] { return page() && page()->isVisible() && panels[2].session->active() && panels[3].session->active(); },
            [this](bool single) {
                if (nativeWorkspace->contains(panels[2].name)) { nativeWorkspace->setVisible(panels[2].name,!single); return; }
                panels[2].session->setHiddenByUser(single, false);
                if (auto group = groupFor(2)) group->syncVisibility(single ? nullptr : panels[2].session);
            });
        toolbarDecoration = new Unbound::ToolbarDecoration(this);
        nativeDecoration = new Unbound::NativePanelDecoration(this);
        timelineControl = new Unbound::TimelineControl(this);
        nativeDrag = new Unbound::NativePanelDrag(this);
        callers = new Unbound::WorkspaceCallers(this);
        for (int index = 0; index < panels.size(); ++index) callers->add({QStringLiteral("unboundCaller_") + panels[index].name, panels[index].label,
            [this, index] { return panels[index].session->active() && !panels[index].session->hiddenByUser(); },
            [this, index] {
                if (panels[index].session->active()) queuePanelDoor(index);
                else QTimer::singleShot(0, this, [this, index] { requestUndock(index); });
            }, [this, index] { return panels[index].toggle.isEmpty() || panels[index].session->active(); }});
        callers->wire();
        workspaceMenu = new Unbound::WorkspaceArrangements(this);
        workspaceMenu->showPanelManager = [this] {
            showNormal(); Unbound::keepOnDisplay(this); raise(); activateWindow();
            Unbound::logEvent("controls.show", {{"origin","workspace-menu"}});
        };
        workspaceMenu->entries = [this] { return presetsUi->list(); };
        workspaceMenu->currentPage = [] { return Unbound::currentWorkspacePage(Unbound::resolveMainWindow()); };
        workspaceMenu->nativeActive = [this] {
            if (Unbound::currentWorkspacePage(Unbound::resolveMainWindow()) == "edit")
                for (const auto &panel : panels) if (panel.session->active()) return false;
            auto main = Unbound::resolveMainWindow();
            return main && !main->property("unboundBackgroundHidden").toBool() && !footer->snapshot()["floating"].toBool();
        };
        workspaceMenu->selected = [this] { return presets->currentData().toString(); };
        workspaceMenu->invoke = [this](const QString &operation, const QString &id) {
            if (operation == "Reorder") { showNormal(); raise(); activateWindow(); }
            presetsUi->invoke(operation, id);
        };
        workspaceMenu->nativeWorkspace = [this](const QString &activePage) {
            if (activePage == "edit") { ++bulkVersion; restoreExpanded(); dockAll(); save(); }
            else footer->dock();
            if (auto main = Unbound::resolveMainWindow()) {
                Unbound::setNativeBackgroundHidden(main,{},false);
                main->showNormal(); main->raise(); main->activateWindow();
            }
        };
        workspaceMenu->wire();
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
        action("hide-controls"); save(); event->accept(); hide();
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
            if (!toggleScanQueued) {
                toggleScanQueued = true;
                QTimer::singleShot(0, this, [this] { toggleScanQueued = false; wireNativeToggles(); });
            }
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
    void showPanel(int i,bool visible)
    {
        if (nativeWorkspace->contains(panels[i].name)) nativeWorkspace->setVisible(panels[i].name,visible);
        else {
            panels[i].session->setHiddenByUser(!visible);
            if (auto group=groupFor(i)) group->syncVisibility(visible ? panels[i].session : nullptr);
        }
    }
    void togglePanelPlacement(int i)
    {
        if (nativeWorkspace->contains(panels[i].name)) { nativeWorkspace->detach(panels[i].name); readyNativeDrag(i); save(); }
        else if (panels[i].session->active()) dockOne(i);
        else requestUndock(i);
    }
    void dockNative(int i,QWidget *target=nullptr,Qt::Edge edge=Qt::RightEdge)
    {
        if (quitting || QApplication::activeModalWidget() || !page() || !page()->isVisible()) return;
        restoreExpanded(); auto group=groupFor(i);
        const auto direction=edge==Qt::Edge(0) ? Qt::Orientation(0) : edge==Qt::TopEdge || edge==Qt::BottomEdge ? Qt::Vertical : Qt::Horizontal;
        const auto error=nativeWorkspace->dockSelection(page(),panels[i].name,group,
            target ? nativeWorkspace->targetName(target) : Unbound::nativeRemainderId(),direction,edge==Qt::LeftEdge || edge==Qt::TopEdge);
        if (!error.isEmpty() && group && group->active()) {
            group->setChrome(static_cast<Unbound::WindowChrome>(style->currentIndex())); group->setSnapping(snap->isChecked());
        }
        status->setText(error.isEmpty() ? QStringLiteral("Panel arrangement docked in the native workspace.") : error);
        Unbound::logEvent("workspace.native-command.complete",{{"panel",panels[i].name},{"success",error.isEmpty()},{"error",error}});
        for (int j=0;j<panels.size();++j) updateRow(j);
        nativeWorkspace->refreshNativeTargets(page());
        save();
    }
    void togglePanelDoor(int i)
    {
        auto session = panels[i].session;
        if (!session->active() || !page() || !page()->isVisible()) return;
        if (expanded >= 0) restoreExpanded();
        auto group = groupFor(i);
        const bool reveal = session->hiddenByUser() || (group && !group->selected(session)) ||
            (nativeWorkspace->contains(panels[i].name) && !nativeWorkspace->selected(panels[i].name));
        showPanel(i,reveal);
        syncNativeToggle(i);
        Unbound::logEvent(QStringLiteral("panel.visibility.native-toggle"), {{"panel", panels[i].name}, {"visible", reveal}});
    }
    void queuePanelDoor(int i)
    {
        if (!panels[i].session->active() || !page() || !page()->isVisible() || pendingNativeDoors.contains(i)) return;
        pendingNativeDoors.insert(i);
        QTimer::singleShot(0, this, [this, i] { pendingNativeDoors.remove(i); togglePanelDoor(i); });
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
                const auto request = [this, i] { queuePanelDoor(i); };
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
        result["page"] = "edit";
        result.remove(QStringLiteral("controls"));
        result.remove(QStringLiteral("selected_arrangement"));
        return result;
    }
    void updatePresetState()
    {
        if (presetsUi) presetsUi->updateState(arrangement());
        if (workspaceMenu) workspaceMenu->wire();
    }
    void restoreArrangement()
    {
        action(QStringLiteral("arrangement-restore"));
        const auto entry = presetsUi->find(presets->currentData().toString());
        if (entry.isEmpty()) { status->setText(QStringLiteral("Select a saved arrangement first.")); return; }
        if (!page() || !page()->isVisible()) { status->setText(QStringLiteral("Open the Edit page to restore an arrangement.")); return; }
        const auto value = entry["workspace"].toObject();
        const auto validation=nativeWorkspace->validate(value["native_arrangement"].toObject(),value["groups"].toArray());
        if (!validation.isEmpty()) { status->setText(validation); Unbound::logEvent("workspace.native-plan.failed",{{"error",validation},{"stage","preset-preflight"}}); return; }
        ++bulkVersion;
        ++groupVersion;
        restoreExpanded();
        dockAll();
        for (const auto &panel : panels) if (panel.session->active()) {
            status->setText(QStringLiteral("Could not restore: a panel could not be docked safely."));
            Unbound::logEvent(QStringLiteral("arrangement.restore.failed"), {{"panel", panel.name}}); return;
        }
        if (footer) footer->setState(value["footer"].toObject());
        { const QSignalBlocker blocker(background); background->setChecked(value["hide_background"].toBool()); }
        pendingGroups = value["groups"].toArray();
        const auto nativeError=nativeWorkspace->setPending(value["native_arrangement"].toObject(),pendingGroups);
        if (!nativeError.isEmpty()) { status->setText(nativeError); return; }
        pin->setChecked(value["pin_controls"].toBool());
        constrain->setChecked(value["keep_on_screen"].toBool(true));
        snap->setChecked(value["snap_windows"].toBool(true));
        hierarchyOption->setChecked(value["panel_hierarchy"].toBool(true));
        style->setCurrentIndex(qBound(0, value["window_chrome"].toInt(), 2));
        const auto saved = value["panels"].toObject();
        for (auto &panel : panels) {
            panel.saved = saved[panel.name].toObject();
            panel.desired = panel.saved["floating"].toBool() || nativeWorkspace->requiredPanels().contains(panel.name);
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
    void readyNativeDrag(int i)
    {
        if (!nativeDrag || !panels[i].session->active() || panels[i].session->grouped()) return;
        nativeDrag->ready(panels[i].name,static_cast<Unbound::MacFloatingHost *>(panels[i].session->window()));
    }
    void updateRow(int i)
    {
        auto &p = panels[i]; const bool active = p.session->active();
        optionsScroll->preservePosition([&] {
            p.undock->setEnabled((!active || p.session->placement()==Unbound::PanelPlacement::NativeSurface) && !p.pending);
            p.dock->setEnabled(active && !p.session->dockPending());
            p.dock->setText(p.session->dockPending() ? QStringLiteral("Pending") : QStringLiteral("Dock"));
            p.show->setEnabled(active); p.hide->setEnabled(active && !p.session->hiddenByUser());
            p.expand->setText(expanded == i ? QStringLiteral("Restore Size") : QStringLiteral("Expand"));
            p.expand->setEnabled(true);
            p.center->setEnabled(p.session->floating());
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
        beforeExpandedNative = nativeWorkspace->contains(panel.name) ? nativeWorkspace->saved() : QJsonObject();
        beforeOverflowOffset = panel.session->overflowOffset();
        expandedWasFloating = panel.session->active();
        expandedWasGrouped = panel.session->grouped();
        expandedGroup = groupFor(i);
        beforeExpandedGroup = expandedGroup ? expandedGroup->snapshot() : QJsonObject();
        if (expandedWasGrouped && !panel.session->beginGroupedExpansion()) return;
        expanded = i;
        const unsigned request = ++expandVersion;
        if (!expandedWasFloating) requestUndock(i, true);
        new Unbound::SurfaceReadiness(this,page(),
            [this,i,request] { return expanded==i && request==expandVersion && !quitting; },
            [this,i] { return panels[i].session->active() && !panels[i].pending &&
                (expandedWasGrouped ? panels[i].session->expandedWindow() : panels[i].session->window()); },
            [this, i, request](bool ready) {
            if (expanded != i || request != expandVersion) return;
            auto host = expandedWasGrouped ? panels[i].session->expandedWindow() : panels[i].session->window();
            if (!ready || !host) { expanded = -1; updateRow(i); save(); return; }
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
        if (!beforeExpandedNative.isEmpty()) {
            const auto nativeError=nativeWorkspace->restorePresentation(beforeExpandedNative);
            if (!nativeError.isEmpty()) error=nativeError;
            beforeExpandedNative={};
        }
        panel.session->restoreOverflowOffset(beforeOverflowOffset);
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
        QWidget *target = Unbound::panelSurface(root, p.name);
        if (nativeSizing && expanded<0 && root && root->isVisible()) nativeSizing->prepare(root->window(),root);
        if (p.name == QLatin1String("widgetEditSourcePlayer") && root && root->isVisible() &&
            Unbound::prepareSourceViewer(root, target)) {
            p.pending = true; updateRow(i);
            const unsigned request = ++p.requestVersion;
            awaitUndockSurface(i, request, root);
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
                    awaitUndockSurface(i, request, root);
                    return;
                }
            }
        }
        finishUndock(i);
    }
    void awaitUndockSurface(int i, unsigned request, QWidget *root)
    {
        new Unbound::SurfaceReadiness(this,root,
            [this,i,request] { return !quitting && panels[i].requestVersion==request; },
            [this,i] {
                auto surface=Unbound::panelSurface(page(),panels[i].name);
                if (panels[i].name==QLatin1String("widgetEditSourcePlayer")) surface=Unbound::sourceViewerSurface(surface);
                return page() && page()->isVisible() && surface && surface->isVisible();
            }, [this,i,request](bool ready) {
                if (quitting || panels[i].requestVersion!=request) return;
                Unbound::logEvent("panel.undock.readiness",{{"panel",panels[i].name},{"ready",ready}});
                panels[i].pending=false; finishUndock(i);
            });
    }
    void finishUndock(int i)
    {
        if (quitting) return;
        auto &p = panels[i]; QWidget *root = page();
        QWidget *target = Unbound::panelSurface(root, p.name);
        Unbound::logEvent(QStringLiteral("panel.undock.preconditions"), {{"panel", p.name},
            {"page_found", bool(root)}, {"page_visible", root && root->isVisible()}, {"target_found", bool(target)},
            {"target_visible", target && target->isVisible()}, {"target_in_page", root && target && root->isAncestorOf(target)}});
        if (p.name == QLatin1String("widgetEditSourcePlayer")) target = Unbound::sourceViewerSurface(target);
        const QSize nativeSize = target ? target->size() : QSize();
        const QString error = p.contents ? p.session->undockContents(target, root, p.label) :
            p.session->undock(target, root, p.label, p.name == QLatin1String("widgetEditSourcePlayer"));
        if (error.isEmpty()) {
            groupReserve->request(p.session->nativeOwnerWindow());
            p.session->window()->setObjectName(QStringLiteral("unboundFloating_") + p.name);
            p.desired = true;
            if (p.label.endsWith("Toolbar")) {
                auto frame = p.session->floatingGeometry(); frame.setSize(QSize(qMax(320, nativeSize.width()), qMax(64, nativeSize.height()+28)));
                p.session->setFloatingGeometry(frame);
            }
            const QRect position = Unbound::WorkspaceSettings::rectangle(p.saved);
            if (position.isValid()) p.session->setFloatingGeometry(position);
            p.session->setPinned(p.saved["pinned"].toBool());
            if (p.saved["hidden"].toBool()) p.session->setHiddenByUser(true);
            if (background->isChecked()) QTimer::singleShot(0, this, [this] {
                if (!quitting && background->isChecked()) applyBackgroundVisibility(true);
            });
        }
        outcome("undock", i, error); updateRow(i); updateNativeSizing(); save();
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
    void dockAll() {
        if (nativeWorkspace && !nativeWorkspace->release(true)) { status->setText("The native layout changed; restoration was stopped."); return; }
        if (footer) footer->dock(); background->setChecked(false); ++groupVersion; pendingGroups = {};
        for (int i = panels.size() - 1; i >= 0; --i) dockOne(i);
    }
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
    QString createGroup(int a, int b, Qt::Orientation orientation, const QJsonObject &saved = {}, const QRect &destination = {})
    {
        auto group = new Unbound::PanelGroup(this,groupReserve);
        const auto error = group->join(panels[a].session, panels[b].session, orientation,destination);
        if (!error.isEmpty()) { delete group; return error; }
        configureGroup(group);
        if (!saved.isEmpty()) group->restore(saved);
        return {};
    }
    void configureGroup(Unbound::PanelGroup *group)
    {
        Unbound::UiWorkTiming timing("group.configure",group->window());
        groups.append(group);
        group->sectionDetached = [this](Unbound::PanelGroup *section) { configureGroup(section); };
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
    QString combinePanels(int a, int b, Qt::Orientation orientation, bool before = true, const QString &targetLeaf = {}, const QRect &destination = {})
    {
        auto left = groupFor(a), right = groupFor(b);
        if (left && left == right) return QStringLiteral("Those panels already share a group.");
        if (right && left) return right->merge(left, orientation, before, targetLeaf);
        if (right) return right->add(panels[a].session, orientation, before, targetLeaf);
        if (left) return left->add(panels[b].session, orientation, !before);
        return before ? createGroup(a, b, orientation,{},destination) : createGroup(b, a, orientation,{},destination);
    }
    void attachDrop(int source, QWidget *target, Qt::Edge edge)
    {
        Unbound::UiWorkTiming timing("group.drop",target ? target->window() : nullptr);
        if (quitting || QApplication::activeModalWidget() || expanded >= 0 || !panels[source].session->active()) return;
        if (nativeWorkspace->ownsTarget(target)) { dockNative(source,target,edge); return; }
        for (int j = 0; j < panels.size(); ++j) if (j != source && panels[j].session->active() &&
            (panels[j].session->window() == target || panels[j].session->window()->window() == target)) {
            if (groupFor(source) && groupFor(source) == groupFor(j)) return;
            const auto frame = target->window()->frameGeometry();
            const auto error = combinePanels(source, j, edge == Qt::Edge(0) ? Qt::Orientation(0) : edge == Qt::LeftEdge || edge == Qt::RightEdge ? Qt::Horizontal : Qt::Vertical,
                edge == Qt::LeftEdge || edge == Qt::TopEdge, target->isWindow() ? QString() : panels[j].name,target->window()->geometry());
            if (error.isEmpty()) if (auto group = groupFor(source)) group->fitFrame(frame);
            status->setText(error.isEmpty() ? QStringLiteral("Panels joined. Drag a member header to detach.") : error);
            Unbound::logEvent(QStringLiteral("group.drop.result"), {{"success", error.isEmpty()}, {"error", error}});
            save(); return;
        }
    }
    void releaseGroups()
    {
        ++groupVersion; pendingGroups = {};
        if (nativeWorkspace) nativeWorkspace->release();
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
        if (nativeMenus) nativeMenus->wire();
        if (nativeDecoration && page()) for (int i=0; i<panels.size(); ++i) {
            auto surface = panels[i].session->active() ? panels[i].session->window() : page()->findChild<QWidget *>(panels[i].name);
            if (panels[i].name=="frameConformTimelines") {
                timelineControl->wire(surface,[this,i] { togglePanelPlacement(i); },[this,i](QPoint) {
                    nativeDrag->begin(panels[i].name);
                    if (nativeWorkspace->contains(panels[i].name)) nativeWorkspace->detach(panels[i].name);
                    else if (auto group=groupFor(i)) group->detachMember(panels[i].session);
                    if (!panels[i].session->active()) requestUndock(i);
                    readyNativeDrag(i);
                });
                continue;
            }
            nativeDecoration->wire(surface,panels[i].name,panels[i].label,panels[i].session->floating(),
                [this,i] { togglePanelPlacement(i); },
                [this,i](QPoint) {
                    nativeDrag->begin(panels[i].name);
                    if (nativeWorkspace->contains(panels[i].name)) nativeWorkspace->detach(panels[i].name);
                    else if (auto group=groupFor(i)) group->detachMember(panels[i].session);
                    if (!panels[i].session->active()) requestUndock(i);
                    readyNativeDrag(i);
                });
        }
        if (workspaceMenu) workspaceMenu->wire();
        if (viewerMode && page()) viewerMode->refresh(page()->window());
        if (toolbarDecoration) for (int i = 0; i < panels.size(); ++i)
            if (panels[i].name == QLatin1String("frameEditToolbarTop") || panels[i].name == QLatin1String("unboundWorkspaceToolbar"))
                toolbarDecoration->wire(page(), panels[i].name, [this, i] { togglePanelPlacement(i); });
        QWidget *root = page();
        if (!root || !root->isVisible()) return;
        const auto anchorError=nativeWorkspace->preparePending(root);
        if (!anchorError.isEmpty()) { status->setText(anchorError); return; }
        const auto floatingAnchor=nativeWorkspace->pendingFloatingAnchor();
        if (!floatingAnchor.isEmpty()) for (const auto &panel : panels) if (panel.name==floatingAnchor && panel.session->active()) {
            const auto geometry=Unbound::WorkspaceSettings::rectangle(panel.saved);
            if (geometry.isValid()) panel.session->setFloatingGeometry(geometry);
            panel.session->setPinned(panel.saved["pinned"].toBool());
            panel.session->setHiddenByUser(panel.saved["hidden"].toBool(),false);
        }
        for (int i = 0; i < panels.size(); ++i)
            if (panels[i].desired && !panels[i].session->active()) requestUndock(i, true);
        const auto nativeError=nativeWorkspace->restoreReady(root);
        if (!nativeError.isEmpty()) status->setText(nativeError);
        nativeWorkspace->refreshNativeTargets(root);
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
        Unbound::UiWorkTiming timing("workspace.save",this);
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
            {"native_arrangement",expanded>=0 && !beforeExpandedNative.isEmpty() ? beforeExpandedNative : nativeWorkspace ? nativeWorkspace->saved() : QJsonObject()},
            {"hide_background", background->isChecked()}, {"native_workspace_mode", background->isChecked() ? "floating" : "native"}, {"footer", footer ? footer->snapshot() : QJsonObject{}},
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
    void updateNativeSizing()
    {
        if (quitting || expanded>=0 || !nativeSizing) return;
        if (nativeWorkspace && nativeWorkspace->active()) return;
        QSet<QString> managed;
        for (const auto &panel : panels) if (panel.session->active()) managed.insert(panel.name);
        if (auto root=page()) nativeSizing->setPanels(root->window(),root,managed);
    }
    void shutdown()
    {
        if (quitting) return;
        restoreExpanded(); save(); quitting = true; saveTimer.stop(); restoreTimer.stop();
        groupReserve->clear();
        if (nativeWorkspace) nativeWorkspace->release(true);
        if (nativeSizing) nativeSizing->restore();
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
    Unbound::GroupHostReserve *groupReserve = nullptr;
    Unbound::NativeWorkspaceSizing *nativeSizing = nullptr;
    Unbound::NativeWorkspaceBinding *nativeWorkspace = nullptr;
    Unbound::PanelMenus *nativeMenus = nullptr;
    QList<Panel> panels;
    QSet<QAbstractButton *> observedToggles;
    QSet<int> pendingNativeDoors;
    Unbound::WorkspaceSettings settings;
    Unbound::ArrangementControls *presetsUi = nullptr;
    Unbound::WorkspaceArrangements *workspaceMenu = nullptr;
    QComboBox *presets = nullptr;
    Unbound::PanelExpansion expansion;
    Unbound::ViewerModeCommand *viewerMode = nullptr;
    Unbound::ToolbarDecoration *toolbarDecoration = nullptr;
    Unbound::NativePanelDecoration *nativeDecoration = nullptr;
    Unbound::TimelineControl *timelineControl = nullptr;
    Unbound::NativePanelDrag *nativeDrag = nullptr;
    Unbound::WorkspaceCallers *callers = nullptr;
    Unbound::FooterPanel *footer = nullptr;
    QTimer saveTimer, restoreTimer;
    bool quitting = false;
    bool toggleScanQueued = false;
    unsigned bulkVersion = 0;
    unsigned expandVersion = 0;
    int expanded = -1;
    bool expandedWasFloating = false, expandedWasGrouped = false;
    QJsonObject beforeExpanded, beforeExpandedGroup, beforeExpandedNative;
    QPoint beforeOverflowOffset;
    QPointer<Unbound::PanelGroup> expandedGroup;
    QList<Unbound::PanelGroup *> groups;
    QJsonArray pendingGroups;
    unsigned groupVersion = 0;
};
