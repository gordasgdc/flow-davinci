#include <QApplication>
#include <QTemporaryDir>
#include <QElapsedTimer>
#include <QThread>
#include <cstdio>
#include "panelgroup.h"
#include "panelcontrols.h"

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
    qputenv("UNBOUND_WORKSPACE_FILE", (temporary.path() + "/Workspace.json").toUtf8());
    qputenv("UNBOUND_ARRANGEMENTS_FILE", (temporary.path() + "/Arrangements.json").toUtf8());
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
    {
        QWidget main; main.setObjectName("UiMainWindow"); main.resize(1100, 700);
        auto page = new QWidget(&main); auto mainLayout = new QVBoxLayout(&main); mainLayout->addWidget(page);
        auto row = new QHBoxLayout(page);
        QList<QWidget *> widgets;
        QList<Unbound::MacPanelSession *> sessions;
        const QStringList names = {"Inspector", "Timeline", "Source", "Program", "MediaPool"};
        for (const auto &name : names) {
            auto widget = new QLabel(name, page); widget->setObjectName(name); row->addWidget(widget); widgets.append(widget);
            sessions.append(new Unbound::MacPanelSession(&main));
        }
        main.show(); settle();
        for (int i = 0; i < widgets.size(); ++i) check(sessions[i]->undock(widgets[i], page, names[i]).isEmpty(), "multi-panel setup");
        Unbound::PanelGroup left(nullptr), right(nullptr);
        check(left.join(sessions[0], sessions[1], Qt::Horizontal).isEmpty() && right.join(sessions[2], sessions[3], Qt::Vertical).isEmpty(), "create two independent groups");
        check(left.merge(&right, Qt::Horizontal).isEmpty(), "merge two groups without redocking members"); settle();
        check(left.active() && !right.active() && left.snapshot()["panels"].toArray().size() == 4, "merged group retains four panel identities");
        for (int i = 0; i < 4; ++i) check(sessions[i]->window()->window() == left.window(), "all merged panels share one native window");
        check(left.add(sessions[4], Qt::Vertical, true, "Timeline").isEmpty(), "insert fifth panel beside a specific leaf"); settle();
        QLabel *transferredHeader = nullptr;
        for (auto header : left.window()->findChildren<QLabel *>("unboundGroupHeader")) if (header->property("groupPanel") == "Program") transferredHeader = header;
        const auto press = transferredHeader->mapToGlobal(QPoint(10, 7));
        QMouseEvent down(QEvent::MouseButtonPress, QPointF(10, 7), QPointF(press), Qt::LeftButton, Qt::LeftButton, Qt::AltModifier);
        QApplication::sendEvent(transferredHeader, &down);
        QMouseEvent move(QEvent::MouseMove, QPointF(), QPointF(press + QPoint(80, 40)), Qt::NoButton, Qt::LeftButton, Qt::AltModifier);
        QApplication::sendEvent(transferredHeader, &move);
        QMouseEvent release(QEvent::MouseButtonRelease, QPointF(), QPointF(press + QPoint(80, 40)), Qt::LeftButton, Qt::NoButton, Qt::AltModifier);
        QApplication::sendEvent(&main, &release); settle();
        check(!sessions[3]->grouped() && left.active() && left.snapshot()["panels"].toArray().size() == 4,
            "a member transferred from another group retains functional tear-off handlers");
        check(left.add(sessions[3], Qt::Horizontal, false, "Source").isEmpty(), "reattach transferred member without dismantling the remainder");
        auto saved = left.snapshot(); QStringList leaves;
        check(Unbound::groupTreeNames(saved["tree"].toObject(), leaves) && leaves.size() == 5, "nested split tree serializes every panel once");
        check(sessions[1]->dock().isEmpty(), "dock one leaf from a larger group"); settle();
        check(left.active() && left.snapshot()["panels"].toArray().size() == 4 && !sessions[1]->active(), "removing one member preserves the remaining group");
        check(widgets[1]->parentWidget() == page, "removed leaf restores its original Resolve slot");
        saved = left.snapshot(); left.dissolve(); settle();
        QList<Unbound::MacPanelSession *> remaining{sessions[0], sessions[2], sessions[3], sessions[4]};
        Unbound::PanelGroup restored(nullptr);
        check(restored.initialize(remaining, saved["tree"].toObject(), Unbound::WorkspaceSettings::rectangle(saved)).isEmpty() && restored.restore(saved), "restore nested arrangement from saved tree");
        settle(); leaves.clear();
        check(Unbound::groupTreeNames(restored.snapshot()["tree"].toObject(), leaves) && leaves.size() == 4 && !leaves.contains("Timeline"), "restored tree collapses removed/unary branches without losing other leaves");
        auto invalid = Unbound::splitGroupTree({{"panel", "Inspector"}}, {{"panel", "Inspector"}}, Qt::Horizontal); leaves.clear();
        check(!Unbound::groupTreeNames(invalid, leaves), "duplicate saved panel identities are rejected");
        restored.window()->close(); settle();
        for (auto session : sessions) check(!session->active(), "closing nested group docks every remaining member");
    }
    {
        QWidget main; main.setObjectName("UiMainWindow"); main.resize(1100, 700);
        auto page = new QWidget(&main); page->setObjectName("WStackPage_Conform");
        auto mainLayout = new QVBoxLayout(&main); mainLayout->addWidget(page); auto row = new QHBoxLayout(page);
        QList<QLabel *> content;
        for (const auto &name : {"frameConformTimelines", "widgetEditSourcePlayer", "widgetEditTimelinePlayer", "frameEditPageMediaPool"}) {
            auto widget = new QWidget(page); widget->setObjectName(name); row->addWidget(widget);
            auto label = new QLabel(name, widget); auto layout = new QVBoxLayout(widget); layout->addWidget(label); content.append(label);
        }
        main.show(); settle();
        {
            PanelControls controls; controls.show(); settle();
            const auto combine = [&](int first, int second, int direction) {
                controls.findChild<QComboBox *>("groupFirst")->setCurrentIndex(first);
                controls.findChild<QComboBox *>("groupSecond")->setCurrentIndex(second);
                controls.findChild<QPushButton *>(QStringLiteral("group_%1").arg(direction))->click(); settle(200);
            };
            combine(4, 2, 1); combine(1, 3, 0);
            auto moving = static_cast<Unbound::MacFloatingHost *>(content[1]->window());
            QWidget *targetCell = content[0];
            while (targetCell && !targetCell->objectName().startsWith("unboundGroupCell_")) targetCell = targetCell->parentWidget();
            check(targetCell != nullptr, "group drag finds an individual destination leaf");
            const auto destination = targetCell->parentWidget();
            const QPoint pointer = destination->mapToGlobal(QPoint(destination->width() / 2, 1));
            const auto press = moving->mapToGlobal(QPoint(100, 7));
            moving->beginPointerDrag(press);
            QMouseEvent move(QEvent::MouseMove, QPointF(), QPointF(pointer), Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
            QApplication::sendEvent(moving, &move);
            QMouseEvent release(QEvent::MouseButtonRelease, QPointF(), QPointF(pointer), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
            QApplication::sendEvent(&main, &release); settle(250);
            for (auto label : content) check(label->window() == content.first()->window(), "controls merge two saved pairs into one four-panel workspace");
            content.first()->window()->move(20000, -20000);
            controls.findChild<QPushButton *>("center_widgetEditSourcePlayer")->click(); settle();
            auto screen = QGuiApplication::screenAt(QCursor::pos()); if (!screen) screen = QGuiApplication::primaryScreen();
            check(screen->availableGeometry().contains(content.first()->window()->frameGeometry()), "Center rescues a complete nested group without dissolving it");
            Unbound::WorkspaceSettings saved;
            const auto layouts = saved.data["groups"].toArray();
            check(layouts.size() == 1 && layouts.first().toObject()["panels"].toArray().size() == 4 && layouts.first().toObject()["tree"].isObject(),
                "workspace persistence records nested group instead of flattening it into pairs");
        }
        settle();
        {
            PanelControls controls; controls.show(); settle(650);
            for (auto label : content) check(label->window() == content.first()->window() && label->window() != &main,
                "reloaded workspace restores all four panels into the same native group");
            controls.findChild<QPushButton *>("dock_widgetEditSourcePlayer")->click(); settle();
            check(content[1]->window() == &main && content[0]->window() == content[2]->window() && content[2]->window() == content[3]->window(),
                "docking one member leaves the other three grouped");
            controls.findChild<QPushButton *>("bulk_1")->click(); settle();
            for (auto label : content) check(label->window() == &main, "Dock All restores a mixed nested workspace");
        }
    }
    if (!failures) std::puts("PASS: nested single-host groups, group merge, leaf insertion/removal, persisted control reload, drag release and original-slot docking");
    return failures ? 1 : 0;
}
