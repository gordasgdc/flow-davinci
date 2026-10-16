#include <QApplication>
#include <QCoreApplication>
#include <QStackedLayout>
#include <QTemporaryDir>
#include <QSaveFile>
#include <QElapsedTimer>
#include <QThread>
#include <cstdio>
#include "panelsession.h"
#include "panelcontrols.h"
#include "controlservice.h"

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
    QTemporaryDir logs;
    qputenv("UNBOUND_DIAGNOSTICS_DIR", logs.path().toUtf8());
    qputenv("UNBOUND_LAUNCH_DIRECTORY", logs.path().toUtf8());
    Unbound::WindowDiagnostics windowLogs(&app);
    app.installEventFilter(&windowLogs);
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
        auto row = new QHBoxLayout(&page);
        auto stack = new QStackedWidget(&page);
        auto source = new QWidget;
        auto offline = new QWidget;
        auto sourceTimeline = new QWidget;
        stack->addWidget(source); stack->addWidget(offline); stack->addWidget(sourceTimeline);
        auto program = new QWidget(&page);
        row->addWidget(stack); row->addWidget(program);
        page.show(); settle();
        Unbound::MacPanelSession session;
        check(session.undock(source, &page, "Source").isEmpty(), "source child detaches from stack");
        check(source->parentWidget() == session.window() && stack->parentWidget() == &page &&
              offline->parentWidget() == stack && sourceTimeline->parentWidget() == stack &&
              program->parentWidget() == &page, "source floats without stack, alternate viewers or program");
        check(session.dock().isEmpty() && stack->widget(0) == source && stack->widget(1) == offline &&
              stack->widget(2) == sourceTimeline && stack->currentWidget() == source, "source stack order and selection restored");
        check(session.undock(source, &page, "Source").isEmpty(), "second source detach");
        stack->setCurrentWidget(offline);
        check(session.dock().isEmpty() && stack->currentWidget() == offline && source->isHidden(), "native viewer selection change survives docking");
    }
    {
        QWidget main;
        main.setObjectName(QStringLiteral("UiMainWindow"));
        auto page = new QWidget(&main);
        page->setObjectName(QStringLiteral("WStackPage_Conform"));
        auto mainLayout = new QVBoxLayout(&main);
        mainLayout->addWidget(page);
        auto row = new QHBoxLayout(page);
        auto stack = new QStackedWidget(page);
        auto source = new QWidget;
        source->setObjectName(QStringLiteral("widgetEditSourcePlayer"));
        stack->addWidget(source); row->addWidget(stack);
        auto program = new QWidget(page);
        program->setObjectName(QStringLiteral("widgetEditTimelinePlayer")); row->addWidget(program);
        main.show(); settle();
        PanelControls controls;
        controls.show(); settle();
        auto sourceUndock = controls.findChild<QPushButton *>(QStringLiteral("undock_widgetEditSourcePlayer"));
        auto sourceDock = controls.findChild<QPushButton *>(QStringLiteral("dock_widgetEditSourcePlayer"));
        auto programDock = controls.findChild<QPushButton *>(QStringLiteral("dock_widgetEditTimelinePlayer"));
        check(sourceUndock && sourceDock && programDock, "controls have distinct panel identities");
        check(!sourceDock->isEnabled() && !programDock->isEnabled(), "inactive Dock buttons are disabled");
        sourceUndock->click(); settle();
        check(sourceDock->isEnabled() && !sourceUndock->isEnabled() && !programDock->isEnabled(), "only detached panel enables Dock");
        programDock->click();
        check(source->parentWidget() != stack, "unrelated Dock cannot claim or change source state");
        sourceDock->click(); settle();
        check(source->parentWidget() == stack && !sourceDock->isEnabled() && sourceUndock->isEnabled(), "source Dock button restores correct child");
        sourceUndock->click(); settle();
        check(source->window()->close(), "source close-to-dock"); settle();
        check(source->parentWidget() == stack && !sourceDock->isEnabled(), "closing host refreshes Dock button state");
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
    {
        Unbound::ControlService service(&app);
        PanelControls *controls = nullptr;
        for (auto window : QApplication::topLevelWidgets())
            if (window->objectName() == QLatin1String("unboundPanelTest")) controls = static_cast<PanelControls *>(window);
        check(controls != nullptr, "resident service creates controls");
        for (int i = 0; i < 2 && controls; ++i) {
            controls->hide();
            QSaveFile request(logs.path() + QStringLiteral("/show-controls.request"));
            check(request.open(QIODevice::WriteOnly), "launch request opened");
            request.write(QByteArray::number(i));
            check(request.commit(), "launch request committed");
            QElapsedTimer timer; timer.start();
            while (!controls->isVisible() && timer.elapsed() < 2500) { settle(); QThread::msleep(10); }
            check(controls->isVisible(), "relaunch request restores hidden controls");
            int count = 0;
            for (auto window : QApplication::topLevelWidgets())
                if (window->objectName() == QLatin1String("unboundPanelTest")) ++count;
            check(count == 1, "relaunch reuses controls without duplicate sessions");
        }
    }
    settle();
    QFile events(logs.path() + QStringLiteral("/events.jsonl"));
    check(events.open(QIODevice::ReadOnly), "diagnostic log created");
    int buttonEvents = 0, arrangementEvents = 0, verifiedDocks = 0;
    while (!events.atEnd()) {
        const auto record = QJsonDocument::fromJson(events.readLine()).object();
        check(record.contains("time") && record.contains("pid") && record.contains("build"), "diagnostic event identity");
        if (record["event"] == "button.click") ++buttonEvents;
        if (record["event"] == "window.move" || record["event"] == "window.resize") ++arrangementEvents;
        if (record["event"] == "panel.dock.verify" && record["passed"].toBool()) ++verifiedDocks;
    }
    check(buttonEvents >= 3 && arrangementEvents > 0 && verifiedDocks > 0, "button, arrangement and verified-dock events logged");
    if (!failures) std::puts("PASS: docking cycles, layout restoration, independent viewers, stack selection, panel buttons, close, visibility and diagnostic events");
    return failures ? 1 : 0;
}
