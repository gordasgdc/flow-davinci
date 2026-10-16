#include <QApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "panelsession.h"

class PanelControls : public QWidget
{
public:
    PanelControls() : QWidget(nullptr, Qt::Tool)
    {
        setObjectName(QStringLiteral("unboundPanelTest"));
        setWindowTitle(QStringLiteral("Unbound — panel tests"));
        setAttribute(Qt::WA_QuitOnClose, false);
        auto layout = new QVBoxLayout(this);
        status = new QLabel(QStringLiteral("Use the Edit page. Enable dual viewers for Source Monitor."), this);
        status->setWordWrap(true);
        layout->addWidget(status);
        struct Target { const char *label; const char *name; };
        const Target targets[] = {
            {"Inspector", "widgetEditInspector"},
            {"Timeline", "frameConformTimelines"},
            {"Source Monitor", "stackedEditSourceOfflinePlayers"},
            {"Program Monitor", "widgetEditTimelinePlayer"}
        };
        for (const auto &target : targets) {
            auto session = new Unbound::MacPanelSession(this);
            sessions.append(session);
            auto row = new QHBoxLayout;
            auto undock = new QPushButton(QStringLiteral("Undock %1").arg(target.label), this);
            auto dock = new QPushButton(QStringLiteral("Dock"), this);
            row->addWidget(undock); row->addWidget(dock);
            layout->addLayout(row);
            connect(undock, &QPushButton::clicked, this, [this, session, target] {
                QWidget *page = nullptr;
                for (QWidget *window : QApplication::topLevelWidgets()) {
                    if (window->objectName() == QLatin1String("UiMainWindow")) {
                        page = window->findChild<QWidget *>(QStringLiteral("WStackPage_Conform"));
                        break;
                    }
                }
                auto panel = page ? page->findChild<QWidget *>(QLatin1String(target.name)) : nullptr;
                const QString error = session->undock(panel, page, QLatin1String(target.label));
                status->setText(error.isEmpty() ? QStringLiteral("%1 is floating.").arg(target.label) : error);
            });
            connect(dock, &QPushButton::clicked, this, [this, session] {
                const QString error = session->dock();
                status->setText(error.isEmpty() ? QStringLiteral("Docked.") : error);
            });
        }
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this] { dockAll(); });
    }
protected:
    void closeEvent(QCloseEvent *event) override { event->setAccepted(dockAll()); }
private:
    bool dockAll()
    {
        bool success = true;
        for (auto session : sessions) {
            const QString error = session->dock();
            if (!error.isEmpty()) { status->setText(error); success = false; }
        }
        return success;
    }
    QLabel *status;
    QList<Unbound::MacPanelSession *> sessions;
};

static void captureWidgets(int capture)
{
    if (!qApp || QThread::currentThread() != qApp->thread()) return;
    QJsonArray widgets;
    for (QWidget *widget : QApplication::allWidgets()) {
        const QString name = widget->objectName();
        if (name.isEmpty() && !widget->isWindow()) continue;
        const QRect geometry = widget->geometry();
        widgets.append(QJsonObject{
            {"name", name}, {"class", widget->metaObject()->className()},
            {"parent", widget->parentWidget() ? widget->parentWidget()->objectName() : QString()},
            {"window", widget->isWindow()}, {"visible", widget->isVisible()},
            {"x", geometry.x()}, {"y", geometry.y()},
            {"width", geometry.width()}, {"height", geometry.height()}});
    }
    const QString directory = QDir::homePath() + QStringLiteral("/Library/Logs/Unbound");
    if (!QDir().mkpath(directory)) return;
    QSaveFile file(directory + QStringLiteral("/widgets-%1.json").arg(capture));
    if (!file.open(QIODevice::WriteOnly)) return;
    const QByteArray data = QJsonDocument(QJsonObject{
        {"qt", qVersion()}, {"main_thread", true}, {"widgets", widgets}}).toJson();
    if (file.write(data) == data.size()) file.commit();
}

extern "C" __attribute__((visibility("default"))) void unboundInspectWidgets()
{
    // Read only, bounded snapshots let Resolve finish creating its lazy panels.
    if (qstrcmp(qVersion(), "5.15.2") != 0 || !qApp ||
        QThread::currentThread() != qApp->thread()) return;
    QTimer::singleShot(2000, qApp, [] { captureWidgets(1); });
    QTimer::singleShot(10000, qApp, [] { captureWidgets(2); });
    QTimer::singleShot(30000, qApp, [] { captureWidgets(3); });
    QTimer::singleShot(2000, qApp, [] {
        auto controls = new PanelControls;
        controls->show();
    });
}
