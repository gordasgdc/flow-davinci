// SPDX-License-Identifier: MIT
#include <QCoreApplication>
#include <cstdio>
#include "grouplayout.h"
#include "windowsnap.h"
#include "viewport.h"
int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    int failures = 0;
    const auto check = [&](bool pass, const char *message) { if (!pass) { ++failures; std::fprintf(stderr, "FAIL: %s\n", message); } };
    const QRect frame(100, 100, 800, 600);
    int offered = 0;
    for (int x = -100; x < 1200; x += 10) for (int y = -100; y < 1000; y += 10)
        offered += Unbound::localDockZone(frame, QPoint(x, y)).target.isValid();
    check(offered < 100, "targets occupy isolated approach areas, not the whole screen or perimeter");
    for (int i = 0; i < 4; ++i) {
        QPoint edge = i == 0 ? QPoint(frame.left(), frame.center().y()) : i == 1 ? QPoint(frame.right(), frame.center().y()) :
            i == 2 ? QPoint(frame.center().x(), frame.top()) : QPoint(frame.center().x(), frame.bottom());
        auto zone = Unbound::localDockZone(frame, edge);
        check(zone.active && zone.edge != Qt::Edge(0), "edge anchor predicts a split");
        check(Unbound::localDockZone(frame, edge + QPoint(2, 2)).target == zone.target, "target is stable through small pointer movement");
    }
    auto tab = Unbound::localDockZone(frame, frame.center());
    check(tab.active && tab.edge == Qt::Edge(0) && tab.preview == frame, "tab preview is distinct from a half split");
    auto tree = Unbound::splitGroupTree({{"panel", "Inspector"}}, {{"panel", "MediaPool"}}, Qt::Orientation(0));
    tree = Unbound::insertGroupTree(tree, {{"panel", "Effects"}}, "Inspector", Qt::Orientation(0), false);
    QStringList names;
    check(Unbound::groupTreeNames(tree, names) && names.size() == 3 && tree["children"].toArray().size() == 3, "adding another tab extends the existing tab stack");
    auto split = Unbound::splitGroupTree(tree, {{"panel", "Timeline"}}, Qt::Vertical);
    names.clear(); check(Unbound::groupTreeNames(split, names) && names.size() == 4, "tab stacks coexist with split groups");
    auto removed = Unbound::removeGroupLeaf(split, "Timeline");
    check(removed["orientation"] == "tabs", "removing a split neighbor preserves its tab stack");
    auto top = Unbound::splitGroupTree({{"panel", "Inspector"}}, {{"panel", "MediaPool"}}, Qt::Horizontal);
    auto puzzle = Unbound::splitGroupTree(top, {{"panel", "Source"}}, Qt::Vertical);
    names.clear(); check(Unbound::groupTreeNames(puzzle, names) && names.size() == 3 &&
        puzzle["children"].toArray()[0].toObject()["orientation"] == "horizontal",
        "Inspector and Media Pool can sit side by side with Source spanning below");
    const QPoint outer(frame.right() + 24, frame.center().y());
    check(Unbound::outerDockZone(frame, outer).active && !Unbound::localDockZone(frame, outer).active,
        "whole-group target is separate from panel edge targets");
    const QRect desktop(0, 0, 1920, 1080), partial(-450, 50, 600, 500);
    check(Unbound::keepGripReachable(partial, {desktop}) == partial, "intentional partial off-screen placement stays unchanged");
    const auto recovered = Unbound::keepGripReachable(QRect(4000, -3000, 2400, 1500), {desktop});
    check(recovered.size() == QSize(2400, 1500) && recovered.top() >= 0 && recovered.left() <= desktop.right() - 95,
        "lost windows retain size while a draggable strip is rescued");
    if (!failures) std::puts("PASS: fixed local docking targets, tab/split plans and partial off-screen recovery (no GUI windows)");
    return failures ? 1 : 0;
}
