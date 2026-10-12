// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QAbstractButton>
#include <QTimer>
#include <QMap>
#include <QSet>
#include <QMessageBox>
#include <atomic>
#include "panesettings.h"
#include "mainwindowsnap.h"
#include "mainwindowplacement.h"
#include "sidebarfloatsession.h"
#include "panedecoration.h"

namespace {
QString colorToggle(const QString &name)
{
    if (name == QLatin1String("m_pFrameNodeGraph")) return QStringLiteral("btnColorToggleNodeGraph");
    if (name == QLatin1String("m_pWidgetGallery")) return QStringLiteral("btnColorToggleGallery");
    if (name == QLatin1String("m_pWidgetLutBrowser")) return QStringLiteral("btnColorToggleLutsBrowser");
    if (name == QLatin1String("m_pWidgetMediaPool")) return QStringLiteral("btnColorToggleMediaPool");
    return {};
}
bool matchesColorToggle(const QString &name, const QString &buttonName)
{
    const auto base=colorToggle(name);
    return !base.isEmpty() && (buttonName==base || buttonName==base+QStringLiteral("InDSM"));
}
bool matchesNativeToggle(const QString &name, const QString &buttonName)
{
    if (!colorToggle(name).isEmpty()) return matchesColorToggle(name, buttonName);
    static const QMap<QString, QString> editToggles = {
        {"frameEditPageMediaPool", "MediaPool"}, {"frameEditPageEffectsLibrary", "EffectsLibrary"},
        {"frameEditPageSoundLibrary", "SoundLibrary"}, {"frameEditPageKeyframeEditor", "KeyframeEditor"},
        {"frameEditPageEditIndex", "EditIndex"}, {"frameEditPageInspector", "Inspector"},
        {"frameEditPageMetadata", "Metadata"}, {"frameEditAudioMeter", "Mixer"}};
    const auto suffix = editToggles.value(name);
    if (suffix.isEmpty()) return false;
    const auto base = QStringLiteral("btnEditToggle") + suffix;
    return buttonName == base + QStringLiteral("InSSM")
        || buttonName == base + QStringLiteral("InDSM")
        || buttonName == base + QStringLiteral("InEFTM");
}
QString settingsKey(const QString &name)
{
    // Preserve existing Edit keys; new pages use an explicit namespace.
    return !colorToggle(name).isEmpty() ? QStringLiteral("Color/") + name : name;
}
QWidget *panePage(QWidget *pane)
{
    const auto rootName = !colorToggle(pane->objectName()).isEmpty()
        ? QStringLiteral("WStackPage_Color") : QStringLiteral("frameEditPageWidgetsContainer");
    for (auto p = pane->parentWidget(); p; p = p->parentWidget())
        if (p->objectName() == rootName) return p;
    return nullptr;
}
class Controller : public QObject
{
public:
    Controller() : QObject(qApp)
    {
        setObjectName(QStringLiteral("UnboundPaneController"));
        m_mainPlacement.error = [this](const QString &message) { settingsError(message); };
        m_names = {{"frameEditPageMediaPool", "Media Pool"}, {"frameEditPageSoundLibrary", "Sound Library"},
            {"frameEditPageKeyframeEditor", "Keyframe Editor"}, {"frameEditPageEffectsLibrary", "Effects"},
            {"frameEditPageEditIndex", "Edit Index"}, {"frameEditPageInspector", "Inspector"},
            {"frameEditPageMetadata", "Metadata"}, {"frameEditAudioMeter", "Mixer"},
            {"m_pFrameNodeGraph", "Nodes"}, {"m_pWidgetGallery", "Gallery"},
            {"m_pWidgetLutBrowser", "LUTs"}, {"m_pWidgetMediaPool", "Media Pool"}};
        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this] { discover(); });
        timer->start(1000);
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this] { delete this; }, Qt::DirectConnection);
        discover();
    }
    ~Controller() override
    {
        m_mainPlacement.flush();
        m_mainPlacement.error = {};
        m_mainSnap.setWindow(nullptr);
        qDeleteAll(m_decorations);
        qDeleteAll(m_sessions);
    }
private:
    QAbstractButton *nativeToggle(QWidget *pane, const QString &name)
    {
        // Use Resolve's own toggle to initialize the pane, including lazy
        // content and internal visibility state. Do not unhide descendants:
        // some of them are alternative meter views that must remain hidden.
        QAbstractButton *toggle = nullptr;
        for (auto button : pane->window()->findChildren<QAbstractButton *>()) {
            const bool matches = matchesNativeToggle(name, button->objectName());
            if (!matches || !button->isVisible()
                || !button->isCheckable() || !button->isEnabled()) continue;
            if (toggle) return nullptr; // Ambiguous UI: never click an arbitrary control.
            toggle = button;
        }
        return toggle;
    }
    void rememberUserToggles()
    {
        if (m_restoringToggles || QCoreApplication::closingDown()) return;
        for (auto it=m_sessions.cbegin(); it!=m_sessions.cend(); ++it) {
            if (!it.value()->active()) continue;
            auto decoration=m_decorations.value(it.key());
            auto pane=decoration ? decoration->pane() : nullptr;
            if (!pane || !panePage(pane) || !panePage(pane)->isVisible()) continue;
            if (auto toggle=nativeToggle(pane,it.key()))
                settingsError(m_settings.saveToggle(settingsKey(it.key()),toggle->isChecked()));
        }
    }
    void queueRememberUserToggles()
    {
        if (m_restoringToggles) return;
        // Resolve can update exclusive sibling selectors in a queued handler.
        QTimer::singleShot(0,this,[this] { rememberUserToggles(); });
    }
    void restoreColorToggles()
    {
        if (m_pendingColorToggles.isEmpty()) return;
        for (const auto &name : m_pendingColorToggles) {
            auto decoration=m_decorations.value(name);
            auto pane=decoration ? decoration->pane() : nullptr;
            if (!pane || !panePage(pane) || !panePage(pane)->isVisible()) return;
        }
        for (auto it=m_decorations.cbegin();it!=m_decorations.cend();++it) {
            if (colorToggle(it.key()).isEmpty() || !m_settings.floating(settingsKey(it.key()))) continue;
            auto pane=it.value()->pane();
            if (pane && panePage(pane)->isVisible() && !m_restoreAttempted.contains(it.key())) return;
        }
        m_restoringToggles=true;
        // Off first, then the selected pane: do not let an off action hide a
        // shared sidebar after its selected pane has already been restored.
        for (bool checked : {false,true}) for (const auto &name : m_pendingColorToggles) {
            if (m_settings.toggle(settingsKey(name))!=checked) continue;
            auto decoration=m_decorations.value(name);
            auto pane=decoration ? decoration->pane() : nullptr;
            if (!pane) continue;
            if (auto toggle=nativeToggle(pane,name))
                if(toggle->isChecked()!=checked) toggle->click();
        }
        m_pendingColorToggles.clear();
        m_restoringToggles=false;
    }
    bool prepareNativePane(QWidget *pane, const QString &name, bool &requested)
    {
        auto toggle=nativeToggle(pane,name);
        if (!toggle) return false;
        if (!toggle->isChecked()) {
            if (m_activationRequested.contains(name)) return false;
            m_activationRequested.insert(name);
            requested = true;
            m_restoringToggles = true;
            toggle->click();
            m_restoringToggles = false;
            QTimer::singleShot(0, this, [this] { discover(); });
            return false; // Let Resolve finish its handlers before detaching.
        }
        return true;
    }
    void settingsError(const QString &message)
    {
        if (!message.isEmpty() && !m_settingsErrorShown) {
            m_settingsErrorShown = true;
            error(message);
        }
    }
    void showDockedPanes()
    {
        for (const auto &name : m_pendingDockToggles.values()) {
            auto decoration = m_decorations.value(name);
            auto pane = decoration ? decoration->pane() : nullptr;
            auto root = pane ? panePage(pane) : nullptr;
            // A close on an inactive page must not select that page. Apply its
            // native toggle when the user returns and its toolbar is available.
            if (!root || !root->isVisible()) continue;
            if (auto toggle = nativeToggle(pane, name)) {
                if (!toggle->isChecked()) toggle->click();
                settingsError(m_settings.saveToggle(settingsKey(name), toggle->isChecked()));
                m_pendingDockToggles.remove(name);
            }
        }
    }
    void undock(const QString &name, bool automatic = false)
    {
        auto decoration = m_decorations.value(name);
        if (!decoration || !decoration->pane()) return;
        auto session = m_sessions.value(name);
        if (!session) {
            session = new Unbound::SidebarFloatSession(this);
            session->setSavedGeometry(m_settings.geometry(settingsKey(name)));
            session->geometrySaved = [this, name](const QByteArray &geometry) {
                settingsError(m_settings.save(settingsKey(name), geometry));
            };
            session->userDocked = [this, name] {
                settingsError(m_settings.saveFloating(settingsKey(name), false));
                // Also suppress retries if saving the user's choice failed.
                m_restoreAttempted.insert(name);
                m_pendingDockToggles.insert(name);
                showDockedPanes();
            };
            session->report = [this](const QString &text) { error(text); discover(); };
            m_sessions.insert(name, session);
        }
        if (session->active()) {
            // Dock explicitly. Resolve can intercept a host Close event before
            // the session sees it, leaving hidden content and a live placeholder.
            // The title-bar Close handler calls this same docking operation.
            if (!automatic) session->dock();
            decoration->setFloating(session->active());
            return;
        }
        m_pendingDockToggles.remove(name);
        auto pane = decoration->pane();
        auto toggle = nativeToggle(pane,name);
        const bool preserveNativeLayout = !colorToggle(name).isEmpty();
        error(session->start(pane, m_names.value(name), panePage(pane), name == QStringLiteral("frameEditAudioMeter"), automatic, preserveNativeLayout));
        if (session->active()) {
            if (preserveNativeLayout) for (auto button : pane->window()->findChildren<QAbstractButton *>()) {
                if (matchesColorToggle(name, button->objectName())) session->observeNativeToggle(button);
            }
            if (toggle) {
                const bool savedChoice=automatic && m_settings.hasToggle(settingsKey(name));
                if (savedChoice && preserveNativeLayout) m_pendingColorToggles.insert(name);
                else if (savedChoice) {
                    m_restoringToggles = true;
                    if (toggle->isChecked() != m_settings.toggle(settingsKey(name))) toggle->click();
                    m_restoringToggles = false;
                }
                else if (!automatic && toggle->isChecked()) {
                    // Detach first, then let Resolve collapse the original slot
                    // through its native handler. Never alter a failed detach.
                    session->reclaimNativeSpace(toggle);
                }
                if (!savedChoice) settingsError(m_settings.saveToggle(settingsKey(name),toggle->isChecked()));
                if (!m_watchedToggles.contains(toggle)) {
                    m_watchedToggles.insert(toggle);
                    connect(toggle, &QAbstractButton::clicked, this, [this] { queueRememberUserToggles(); });
                    connect(toggle, &QObject::destroyed, this, [this,toggle] { m_watchedToggles.remove(toggle); });
                }
            }
            session->rememberGeometry();
            settingsError(m_settings.saveFloating(settingsKey(name), true));
        }
        decoration->setFloating(session->active());
    }
    void error(const QString &text)
    {
        if (text.isEmpty()) return;
        auto box = new QMessageBox(QMessageBox::Warning, QStringLiteral("Unbound"), text, QMessageBox::Ok);
        box->setAttribute(Qt::WA_DeleteOnClose);
        box->setAttribute(Qt::WA_QuitOnClose, false);
        box->show();
    }
    void discover()
    {
        for (auto window : QApplication::topLevelWidgets())
            if (window->objectName() == QLatin1String("UiMainWindow")) {
                m_mainSnap.setWindow(window); m_mainPlacement.attach(window); break;
            }
        for (auto it = m_decorations.begin(); it != m_decorations.end();) {
            if (!it.value()->pane()) {
                m_activationRequested.remove(it.key());
                m_restoreAttempted.remove(it.key());
                delete it.value(); it = m_decorations.erase(it);
            }
            else { it.value()->setFloating(m_sessions.value(it.key()) && m_sessions.value(it.key())->active()); ++it; }
        }
        if (m_decorations.size() != m_names.size()) for (auto pane : QApplication::allWidgets()) {
            const auto name = pane->objectName();
            if (!m_names.contains(name) || m_decorations.contains(name) || !panePage(pane)
                || (!qobject_cast<QBoxLayout *>(pane->layout()) && !qobject_cast<QGridLayout *>(pane->layout()))) continue;
            auto decoration = new Unbound::PaneDecoration(pane, this, [this, name] {
                undock(name);
            });
            m_decorations.insert(name, decoration);
        }
        // Project Manager/other pages can exist before Edit is ready. Keep
        // intent pending, without selecting a page or touching empty layouts.
        if (QCoreApplication::closingDown() || QApplication::activeModalWidget()) return;
        showDockedPanes();
        // Watch all selectors, including docked siblings: selecting one can
        // turn off a floating sibling's mutually exclusive sidebar selector.
        for (auto it=m_decorations.cbegin(); it!=m_decorations.cend(); ++it) {
            auto pane=it.value()->pane();
            if (!pane) continue;
            auto toggle=nativeToggle(pane,it.key());
            if (!toggle || m_watchedToggles.contains(toggle)) continue;
            m_watchedToggles.insert(toggle);
            connect(toggle,&QAbstractButton::clicked,this,[this] { queueRememberUserToggles(); });
            connect(toggle,&QObject::destroyed,this,[this,toggle] { m_watchedToggles.remove(toggle); });
        }
        for (auto it = m_decorations.cbegin(); it != m_decorations.cend(); ++it) {
            const auto name = it.key();
            if (!m_settings.floating(settingsKey(name)) || m_restoreAttempted.contains(name)) continue;
            auto pane = it.value()->pane();
            auto root = pane ? panePage(pane) : nullptr;
            if (!root || !root->isVisible() || !pane->layout() || !pane->layout()->count()) continue;
            bool requested = false;
            if ((name == QStringLiteral("frameEditAudioMeter") || name == QStringLiteral("frameEditPageInspector")
                || name == QStringLiteral("frameEditPageMetadata") || !colorToggle(name).isEmpty())
                && !prepareNativePane(pane, name, requested)) {
                // Shared sidebar selectors are mutually exclusive. Let one
                // native activation finish and detach it before selecting next.
                if (requested && !colorToggle(name).isEmpty()) return;
                continue;
            }
            m_restoreAttempted.insert(name); // No repeated errors for incompatible panes.
            undock(name, true);
        }
        restoreColorToggles();
    }
    Unbound::PaneSettings m_settings;
    Unbound::MainWindowPlacement m_mainPlacement{m_settings};
    Unbound::MainWindowSnap m_mainSnap;
    bool m_settingsErrorShown = false;
    bool m_restoringToggles = false;
    QSet<QAbstractButton *> m_watchedToggles;
    QSet<QString> m_activationRequested;
    QSet<QString> m_restoreAttempted;
    QSet<QString> m_pendingColorToggles;
    QSet<QString> m_pendingDockToggles;
    QMap<QString, QString> m_names;
    QMap<QString, Unbound::PaneDecoration *> m_decorations;
    QMap<QString, Unbound::SidebarFloatSession *> m_sessions;
};
std::atomic<bool> started{false};
}

// Invoked by the launcher on a short-lived remote thread. All Qt widget work
// is queued to Resolve's existing GUI thread; no work is done in DllMain.
extern "C" __declspec(dllexport) DWORD WINAPI UnboundStart(void *)
{
    auto app = qobject_cast<QApplication *>(QCoreApplication::instance());
    if (!app || QCoreApplication::closingDown()) return 0;
    if (started.exchange(true)) return 1;
    if (!QMetaObject::invokeMethod(app, [] { new Controller; }, Qt::QueuedConnection)) {
        started = false;
        return 0;
    }
    return 1;
}
