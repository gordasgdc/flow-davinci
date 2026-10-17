// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTemporaryDir>
#include <QElapsedTimer>
#include <QThread>
#include <cstdio>
#include "controlscroll.h"
#include "floatinghost.h"
#include "viewerpreparation.h"
#include "tabtearoff.h"
#include "workspacecallers.h"
int main(int argc, char **argv)
{
    qputenv("QT_MAC_DISABLE_FOREGROUND_APPLICATION_TRANSFORM", "1");
    const auto foreground = Unbound::nativeFrontmostProcessForTest();
    QApplication app(argc, argv); app.setQuitOnLastWindowClosed(false);
    QTemporaryDir logs; qputenv("UNBOUND_DIAGNOSTICS_DIR", logs.path().toUtf8());
    int failures = 0;
    const auto check = [&](bool pass, const char *message) { if (!pass) { ++failures; std::fprintf(stderr, "FAIL: %s\n", message); } };
    const auto exercise = [&](QScrollArea &scroll, bool guarded) {
        scroll.setAttribute(Qt::WA_DontShowOnScreen); scroll.resize(220, 120);
        auto body = new QWidget; auto row = new QHBoxLayout(body);
        auto undock = new QPushButton("Undock", body); auto middle = new QWidget(body); middle->setMinimumWidth(700);
        auto next = new QLineEdit("Next focus target", body); row->addWidget(undock); row->addWidget(middle); row->addWidget(next);
        scroll.setWidget(body); scroll.show(); QApplication::processEvents(); QApplication::setActiveWindow(&scroll);
        undock->setFocus(); scroll.horizontalScrollBar()->setValue(0);
        if (guarded) static_cast<Unbound::ControlScroll &>(scroll).preservePosition([&] { undock->setEnabled(false); });
        else undock->setEnabled(false);
        const int result = scroll.horizontalScrollBar()->value();
        check(!(Unbound::nativeVisibilityFlags(&scroll) & 1), "scroll fixture remains absent from screen");
        scroll.hide(); return result;
    };
    QScrollArea baseline; const auto before = exercise(baseline, false);
    Unbound::ControlScroll candidate(nullptr); const auto after = exercise(candidate, true);
    check(before > 0, "reproduce Qt focus-transfer horizontal scroll jump");
    check(after == 0, "row-update guard prevents the reproduced scroll jump");
    QMenu nativeMenu; nativeMenu.setObjectName("workspaceShowPanel"); nativeMenu.setAttribute(Qt::WA_DontShowOnScreen);
    bool calledVisible = false;
    Unbound::WorkspaceCallers callers(nullptr);
    callers.add({"testSourceCaller", "Source Monitor", [&] { return calledVisible; }, [&] { calledVisible = !calledVisible; }});
    callers.wire();
    auto caller = nativeMenu.findChild<QAction *>("testSourceCaller");
    check(caller && !caller->isChecked() && !nativeMenu.isVisible(),
        "workspace caller exists before any QWidget menu Show event");
    if (caller) caller->trigger(); QApplication::processEvents(); callers.wire();
    check(calledVisible && caller && caller->isChecked(), "native menu caller shares command visibility state");
    callers.wire(); int callerCount = 0;
    for (auto action : nativeMenu.actions()) callerCount += action->objectName() == "testSourceCaller";
    check(callerCount == 1, "repeated menu discovery does not duplicate callers");
    QTabWidget tabs; tabs.setAttribute(Qt::WA_DontShowOnScreen); tabs.resize(400,200);
    auto firstTab = new QWidget; auto secondTab = new QWidget;
    tabs.addTab(firstTab, "Inspector"); tabs.addTab(secondTab, "Media Pool"); tabs.show(); QApplication::processEvents();
    int tears = 0; QWidget *torn = nullptr;
    Unbound::TabTearOff tearOff(&tabs, &tabs, [&](QWidget *panel, QPoint) { ++tears; torn = panel; });
    auto bar = tabs.tabBar(); const auto tabPoint = bar->tabRect(0).center();
    const auto send = [&](QEvent::Type type, QPoint point, Qt::MouseButton button, Qt::MouseButtons buttons) {
        QMouseEvent event(type, QPointF(point), QPointF(bar->mapToGlobal(point)), button, buttons, Qt::NoModifier);
        QApplication::sendEvent(bar, &event);
    };
    send(QEvent::MouseButtonPress, tabPoint, Qt::LeftButton, Qt::LeftButton);
    send(QEvent::MouseButtonRelease, tabPoint, Qt::LeftButton, Qt::NoButton); QApplication::processEvents();
    check(tears == 0, "clicking a tab does not detach it");
    send(QEvent::MouseButtonPress, tabPoint, Qt::LeftButton, Qt::LeftButton);
    send(QEvent::MouseMove, QPoint(tabPoint.x(),bar->height()+40), Qt::NoButton, Qt::LeftButton);
    QApplication::processEvents();
    check(tears == 1 && torn == firstTab, "dragging a tab outside the strip requests that exact panel");
    send(QEvent::MouseButtonRelease, tabPoint, Qt::LeftButton, Qt::NoButton);
    tabs.hide();
    QWidget nativeRoot; nativeRoot.setAttribute(Qt::WA_DontShowOnScreen);
    QWidget source(&nativeRoot); source.hide();
    QAction singleViewer(&nativeRoot); singleViewer.setText("Single Viewer Mode");
    singleViewer.setCheckable(true); singleViewer.setChecked(true);
    int commands = 0;
    QObject::connect(&singleViewer, &QAction::triggered, [&] { ++commands; });
    check(Unbound::prepareSourceViewer(&nativeRoot, &source) && commands == 1 && !singleViewer.isChecked(),
        "hidden source requests dual viewers through the existing checked native command");
    check(!Unbound::prepareSourceViewer(&nativeRoot, &source) && commands == 1,
        "source preparation does not toggle dual viewers back off");
    singleViewer.setChecked(true); singleViewer.setEnabled(false);
    check(!Unbound::prepareSourceViewer(&nativeRoot, &source) && commands == 1,
        "disabled native viewer command is left untouched");
    Unbound::MacFloatingHost host; host.setAttribute(Qt::WA_DontShowOnScreen); host.resize(400, 250);
    for (auto style : {Unbound::WindowChrome::Standard, Unbound::WindowChrome::Compact}) {
        host.setChrome(style); const auto lights = Unbound::nativeTrafficFramesForTest(&host);
        check(lights.size() == 4 && lights[3].size() == lights[2].size() && lights[3].y() == lights[2].y() &&
            lights[3].x() - lights[2].x() == lights[2].x() - lights[1].x(), "native pin matches traffic light size, spacing and baseline");
        if (style == Unbound::WindowChrome::Standard) {
            const auto title = Unbound::nativeTitleFrameForTest(&host);
            check(title.isValid() && lights.size() == 4 && title.left() > lights[3].right(),
                "native title reserves space after the fourth traffic light");
        }
        Unbound::clickNativePinForTest(&host); check(host.pinned(), "native pin light pins shared state");
        Unbound::clickNativePinForTest(&host); check(!host.pinned(), "native pin light unpins shared state");
    }
    check(Unbound::saveNativeTrafficForTest(&host, "/private/tmp/unbound-pin-candidate.png"), "render native traffic lights for inspection");
    host.setChrome(Unbound::WindowChrome::Hidden);
    check(!host.findChild<QToolButton *>("unboundPinWindow")->isHidden(), "hidden titlebar keeps a pin fallback");
    check(!(Unbound::nativeVisibilityFlags(&host) & 1), "pin fixture never appears onscreen");
    Unbound::DockCompass compass(&host); compass.setAttribute(Qt::WA_DontShowOnScreen);
    compass.display(Unbound::dockCompassZones(QRect(0,0,300,300)), QPoint(149,149));
    check(compass.grab().save("/private/tmp/unbound-compass-candidate.png"), "render five-way compass for inspection");
    check(!(Unbound::nativeVisibilityFlags(&compass) & 1), "compass fixture never appears onscreen");
    host.setChrome(Unbound::WindowChrome::Standard);
    Unbound::postNativeMoveStartForTest(&host);
    host.move(host.pos() + QPoint(30, 20));
    QElapsedTimer release; release.start();
    while (release.elapsed() < 80) { QApplication::processEvents(); QThread::msleep(1); }
    QFile events(logs.path() + "/events.jsonl"); events.open(QIODevice::ReadOnly); const auto trace = events.readAll();
    check(trace.contains("native-titlebar-release") && !QWidget::mouseGrabber(), "native titlebar notification path ends without acquiring mouse capture");
    check(!(Unbound::nativeVisibilityFlags(&host) & 1), "native move fixture remains excluded from the screen");
    check(Unbound::nativeFrontmostProcessForTest() == foreground, "background checks preserve the foreground application");
    std::printf("Scroll baseline=%d candidate=%d\n", before, after);
    if (!failures) std::puts("PASS: reproduced scrolling regression, native pins and hidden compass rendering without foreground changes");
    return failures ? 1 : 0;
}
