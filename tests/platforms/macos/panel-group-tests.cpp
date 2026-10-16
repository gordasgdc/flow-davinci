#include <QApplication>
#include <QTemporaryDir>
#include <QElapsedTimer>
#include <QThread>
#include <cstdio>
#include "panelgroup.h"

static int failures = 0;
static void check(bool pass, const char *message)
{ if (!pass) { ++failures; std::fprintf(stderr, "FAIL: %s\n", message); } }
static void settle(int duration = 30)
{
    QElapsedTimer timer; timer.start();
    do { QCoreApplication::processEvents(); QThread::msleep(1); } while (timer.elapsed() < duration);
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}
int main(int argc, char **argv)
{
    QApplication app(argc, argv); app.setQuitOnLastWindowClosed(false);
    QTemporaryDir temporary;
    qputenv("UNBOUND_DIAGNOSTICS_DIR", temporary.path().toUtf8());
    for (auto orientation : {Qt::Horizontal, Qt::Vertical}) for (bool contents : {false, true}) {
        QWidget main; main.setObjectName("UiMainWindow"); main.resize(1100, 700);
        auto mainLayout = new QVBoxLayout(&main);
        auto page = new QWidget(&main); mainLayout->addWidget(page);
        auto row = new QHBoxLayout(page);
        auto source = new QWidget(page); source->setObjectName("widgetEditSourcePlayer");
        auto program = new QWidget(page); program->setObjectName("widgetEditTimelinePlayer");
        auto sourceLayout = new QVBoxLayout(source); sourceLayout->addWidget(new QLabel("Source", source));
        auto programLayout = new QVBoxLayout(program); programLayout->addWidget(new QLabel("Program", program));
        row->addWidget(source); row->addWidget(program); main.show(); settle();
        Unbound::MacPanelSession a, b;
        check((contents ? a.undockContents(source, page, "Source") : a.undock(source, page, "Source")).isEmpty(), "float source");
        check((contents ? b.undockContents(program, page, "Program") : b.undock(program, page, "Program")).isEmpty(), "float program");
        settle();
        const auto beforeA = a.floatingGeometry(), beforeB = b.floatingGeometry();
        Unbound::PanelGroup group(nullptr);
        check(group.join(&a, &b, orientation).isEmpty(), "join pair"); settle();
        check(a.grouped() && b.grouped() && !a.window()->isWindow() && !b.window()->isWindow(), "members cease to be independent top-level windows");
        check(a.window()->window() == group.window() && b.window()->window() == group.window(), "both members share exactly one native host");
        auto splitter = group.window()->findChild<QSplitter *>();
        check(splitter && splitter->count() == 2 && splitter->orientation() == orientation, "group uses native split layout in requested direction");
        const auto startA = a.window()->mapToGlobal(QPoint()), startB = b.window()->mapToGlobal(QPoint());
        group.window()->move(group.window()->pos() + QPoint(15, 20));
        check(a.window()->mapToGlobal(QPoint()) - startA == QPoint(15, 20) &&
            b.window()->mapToGlobal(QPoint()) - startB == QPoint(15, 20), "one move updates both panels synchronously without follower timer");
        splitter->setSizes({250, 400}); settle();
        const auto saved = group.snapshot();
        check(saved["sizes"].toArray().size() == 2, "split sizes are serializable");
        page->hide(); settle(); check(!group.window()->isVisible(), "group hides when Edit leaves");
        page->show(); settle(); check(group.window()->isVisible() && a.window()->isVisible() && b.window()->isVisible(), "group and both panels return with Edit");
        a.setHiddenByUser(true); b.setHiddenByUser(true); settle();
        check(!group.window()->isVisible(), "fully hidden group stays hidden");
        a.setHiddenByUser(false); settle(); check(group.window()->isVisible(), "showing a member recalls its group");
        group.dissolve(); settle();
        check(!a.grouped() && !b.grouped() && a.window()->isWindow() && b.window()->isWindow(), "ungroup restores independent windows");
        check(a.floatingGeometry() == beforeA && b.floatingGeometry() == beforeB, "ungroup restores standalone geometries");
        b.setHiddenByUser(false);
        check(group.join(&a, &b, orientation).isEmpty(), "regroup"); group.restore(saved); settle();
        check(group.snapshot()["sizes"] == saved["sizes"], "group restores saved split ratio");
        QLabel *header = nullptr;
        for (auto candidate : group.window()->findChildren<QLabel *>("unboundGroupHeader"))
            if (candidate->property("groupMember").toInt() == 0) header = candidate;
        const QPoint press = header->mapToGlobal(QPoint(12, 7));
        const QPoint anchor = press - header->parentWidget()->parentWidget()->mapToGlobal(QPoint());
        QMouseEvent down(QEvent::MouseButtonPress, QPointF(12, 7), QPointF(press), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(header, &down);
        const QPoint moved = press + QPoint(70, 45);
        QMouseEvent move(QEvent::MouseMove, QPointF(82, 52), QPointF(moved), Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(header, &move);
        check(!group.active() && !a.grouped() && !b.grouped(), "dragging a member header tears off its group");
        check(a.window()->frameGeometry().topLeft() == moved - anchor, "tear-off preserves pointer-to-panel offset without delayed movement");
        const QPoint released = moved + QPoint(20, 10);
        QMouseEvent up(QEvent::MouseButtonRelease, QPointF(12, 7), QPointF(released), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        QApplication::sendEvent(a.window(), &up);
        check(a.window()->frameGeometry().topLeft() == released - anchor, "detached window follows pointer synchronously through release");
        check(group.join(&a, &b, orientation).isEmpty(), "regroup after drag tear-off");
        check(a.dock().isEmpty(), "docking one member safely releases group"); settle();
        check(!group.active() && !a.active() && b.active() && !b.grouped(), "neighbor remains a usable floating panel");
        check(b.dock().isEmpty(), "dock remaining panel"); settle();
        check(source->layout() == sourceLayout && program->layout() == programLayout && row->count() == 2,
            "both original Resolve slots and layouts survive grouping and docking");
        check((contents ? a.undockContents(source, page, "Source") : a.undock(source, page, "Source")).isEmpty(), "refloat source for close test");
        check((contents ? b.undockContents(program, page, "Program") : b.undock(program, page, "Program")).isEmpty(), "refloat program for close test");
        check(group.join(&a, &b, orientation).isEmpty(), "regroup for close test");
        group.window()->close(); settle();
        check(!group.active() && !a.active() && !b.active(), "closing group safely docks both members");
    }
    if (!failures) std::puts("PASS: single-host horizontal/vertical groups, synchronous movement, split restoration, page lifecycle, visibility and reversible docking");
    return failures ? 1 : 0;
}
