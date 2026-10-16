#include <QApplication>
#include <QCoreApplication>
#include <QStackedLayout>
#include <cstdio>
#include "panelsession.h"

static int failures = 0;
static void check(bool condition, const char *message)
{
    if (!condition) { ++failures; std::fprintf(stderr, "FAIL: %s\n", message); }
}
static void settle()
{
    QCoreApplication::processEvents();
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    {
        QWidget page;
        auto layout = new QHBoxLayout(&page);
        auto left = new QWidget(&page);
        auto right = new QWidget(&page);
        layout->addWidget(left, 2, Qt::AlignTop);
        layout->addWidget(right, 3);
        page.show(); settle();
        QPointer<QWidget> surviving = left;
        Unbound::MacPanelSession session;
        for (int i = 0; i < 30; ++i) {
            check(session.undock(left, &page, "Panel").isEmpty(), "box detach");
            check(left->parentWidget() == session.window(), "content moved into host");
            check(layout->count() == 2 && layout->itemAt(1)->widget() == right, "sibling preserved");
            check(!session.undock(left, &page, "Panel").isEmpty(), "double detach rejected");
            check(session.dock().isEmpty(), "box restore");
            check(left->parentWidget() == &page && layout->itemAt(0)->widget() == left, "original parent and position");
            check(layout->stretch(0) == 2 && layout->stretch(1) == 3 &&
                  layout->itemAt(0)->alignment() == Qt::AlignTop, "stretch and alignment preserved");
            settle();
            check(surviving && left->isVisible(), "content survives host deletion");
        }
        check(session.undock(left, &page, "Panel").isEmpty(), "close setup");
        check(session.window()->close() && !session.active() && left->parentWidget() == &page, "close docks panel");
        settle();
        left->hide();
        check(!session.undock(left, &page, "Panel").isEmpty() && left->parentWidget() == &page, "hidden panel rejected unchanged");
    }
    {
        QWidget page;
        auto layout = new QGridLayout(&page);
        auto content = new QWidget(&page);
        layout->addWidget(content, 1, 2, 2, 3, Qt::AlignLeft);
        page.show(); settle();
        Unbound::MacPanelSession session;
        check(session.undock(content, &page, "Grid").isEmpty(), "grid detach");
        check(session.dock().isEmpty(), "grid restore");
        int row, column, rows, columns;
        layout->getItemPosition(layout->indexOf(content), &row, &column, &rows, &columns);
        check(row == 1 && column == 2 && rows == 2 && columns == 3 &&
              layout->itemAt(0)->alignment() == Qt::AlignLeft, "grid span and alignment preserved");
    }
    {
        QWidget page;
        auto box = new QVBoxLayout(&page);
        auto splitter = new QSplitter(&page);
        auto a = new QWidget(splitter);
        auto b = new QWidget(splitter);
        splitter->addWidget(a); splitter->addWidget(b);
        box->addWidget(splitter);
        page.resize(800, 400); page.show(); settle();
        splitter->setSizes({220, 560});
        const auto sizes = splitter->sizes();
        Unbound::MacPanelSession session;
        check(session.undock(a, &page, "Split").isEmpty(), "splitter detach");
        check(session.dock().isEmpty() && splitter->widget(0) == a && splitter->widget(1) == b, "splitter order restored");
        check(splitter->sizes() == sizes, "splitter sizes restored");
    }
    {
        QWidget page;
        page.resize(900, 600);
        QWidget source(&page), program(&page), timeline(&page);
        source.setGeometry(0, 0, 400, 250);
        program.setGeometry(420, 0, 400, 250);
        timeline.setGeometry(0, 270, 820, 300);
        page.show(); settle();
        const QRect sourceGeometry = source.geometry(), programGeometry = program.geometry(), timelineGeometry = timeline.geometry();
        Unbound::MacPanelSession sourceSession, programSession, timelineSession;
        check(sourceSession.undock(&source, &page, "Source").isEmpty(), "independent source detach");
        check(programSession.undock(&program, &page, "Program").isEmpty(), "independent program detach");
        check(timelineSession.undock(&timeline, &page, "Timeline").isEmpty(), "independent timeline detach");
        page.hide(); settle();
        check(!sourceSession.window()->isVisible() && !programSession.window()->isVisible() &&
              !timelineSession.window()->isVisible(), "page change hides all hosts");
        page.show(); settle();
        check(sourceSession.window()->isVisible() && programSession.window()->isVisible() &&
              timelineSession.window()->isVisible(), "page return shows all hosts");
        check(programSession.dock().isEmpty() && sourceSession.active() && timelineSession.active(), "program docks independently");
        check(sourceSession.dock().isEmpty() && timelineSession.dock().isEmpty(), "remaining panels dock");
        check(source.geometry() == sourceGeometry && program.geometry() == programGeometry &&
              timeline.geometry() == timelineGeometry, "manual geometry restored");
    }
    {
        QWidget page;
        auto layout = new QStackedLayout(&page);
        auto content = new QWidget(&page);
        layout->addWidget(content); page.show(); settle();
        Unbound::MacPanelSession session;
        check(!session.undock(content, &page, "Stack").isEmpty() && layout->currentWidget() == content &&
              content->parentWidget() == &page && !session.active(), "unsupported layout rejected unchanged");
    }
    {
        QWidget page;
        auto layout = new QVBoxLayout(&page);
        auto content = new QWidget(&page);
        layout->addWidget(content); page.show(); settle();
        { Unbound::MacPanelSession session; check(session.undock(content, &page, "Lifetime").isEmpty(), "lifetime setup"); }
        settle();
        check(layout->itemAt(0)->widget() == content && content->parentWidget() == &page, "session destruction restores content");
    }
    settle();
    if (!failures) std::puts("PASS: 30 docking cycles, box/grid/splitter restoration, close, visibility, independent panels, rejection and lifetime");
    return failures ? 1 : 0;
}
