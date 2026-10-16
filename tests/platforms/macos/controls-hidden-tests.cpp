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
    Unbound::MacFloatingHost host; host.setAttribute(Qt::WA_DontShowOnScreen); host.resize(400, 250);
    for (auto style : {Unbound::WindowChrome::Standard, Unbound::WindowChrome::Compact}) {
        host.setChrome(style); const auto lights = Unbound::nativeTrafficFramesForTest(&host);
        check(lights.size() == 4 && lights[3].size() == lights[2].size() && lights[3].y() == lights[2].y() &&
            lights[3].x() - lights[2].x() == lights[2].x() - lights[1].x(), "native pin matches traffic light size, spacing and baseline");
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
