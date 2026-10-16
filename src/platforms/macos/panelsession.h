// SPDX-License-Identifier: MIT
#pragma once
#include <QBoxLayout>
#include <QCloseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QSplitter>
#include <QWidget>

namespace Unbound {
// Experimental Mac session. Each instance owns one host and preserves one slot.
class MacPanelSession : public QObject
{
public:
    explicit MacPanelSession(QObject *owner = nullptr) : QObject(owner) {}
    ~MacPanelSession() override { dock(); }
    bool active() const { return !host.isNull(); }
    QWidget *window() const { return host; }

    QString undock(QWidget *widget, QWidget *page, const QString &title)
    {
        if (active()) return QStringLiteral("This panel is already floating.");
        if (!widget || !page || !page->isAncestorOf(widget) || !widget->isVisible() || widget->isWindow())
            return QStringLiteral("Show this panel on the Edit page first.");
        QWidget *parent = widget->parentWidget();
        auto splitter = qobject_cast<QSplitter *>(parent);
        QLayout *layout = parent->layout();
        if (splitter) layout = nullptr;
        if (layout && !qobject_cast<QBoxLayout *>(layout) && !qobject_cast<QGridLayout *>(layout))
            return QStringLiteral("This panel's layout is not supported yet.");
        if ((layout && layout->indexOf(widget) < 0) || (splitter && splitter->indexOf(widget) < 0))
            return QStringLiteral("The panel is not in its expected layout.");

        content = widget;
        originalParent = parent;
        originalLayout = layout;
        originalSplitter = splitter;
        originalPage = page;
        geometry = widget->geometry();
        flags = widget->windowFlags();
        hidden = widget->isHidden();
        if (splitter) splitterSizes = splitter->sizes();
        // QSplitter automatically inserts new children; replaceWidget needs an outsider.
        auto marker = new QLabel(title + QStringLiteral(" is floating."), splitter ? nullptr : parent);
        marker->setAlignment(Qt::AlignCenter);
        marker->setSizePolicy(widget->sizePolicy());
        marker->setMinimumSize(widget->minimumSize());
        marker->setMaximumSize(widget->maximumSize());
        placeholder = marker;
        if (layout) {
            auto item = layout->replaceWidget(widget, marker, Qt::FindDirectChildrenOnly);
            if (!item) { delete marker; clear(); return QStringLiteral("Could not preserve the panel slot."); }
            delete item;
        } else if (splitter) {
            const int index = splitter->indexOf(widget);
            if (splitter->replaceWidget(index, marker) != widget) {
                delete marker; clear(); return QStringLiteral("Could not preserve the splitter slot.");
            }
            splitter->setSizes(splitterSizes);
        } else {
            marker->setGeometry(geometry);
        }
        host = new QWidget(nullptr, Qt::Window);
        host->setAttribute(Qt::WA_QuitOnClose, false);
        host->setWindowTitle(title);
        host->setObjectName(QStringLiteral("unboundFloating_") + widget->objectName());
        host->resize(widget->size().expandedTo(QSize(320, 240)));
        auto box = new QVBoxLayout(host);
        box->setContentsMargins(0, 0, 0, 0);
        box->addWidget(widget);
        marker->show();
        widget->show();
        host->installEventFilter(this);
        page->installEventFilter(this);
        parentDestroyed = connect(parent, &QObject::destroyed, this, [this] {
            // The original owner is gone; its displaced content ends with it.
            if (originalPage) originalPage->removeEventFilter(this);
            if (host) host->deleteLater();
            clear();
        });
        host->show();
        return {};
    }

    QString dock()
    {
        if (!active()) return {};
        if (!content || !originalParent || !placeholder || content->parentWidget() != host)
            return QStringLiteral("Resolve changed the panel hierarchy; restoration was stopped.");
        if (originalSplitter) {
            const int index = originalSplitter->indexOf(placeholder);
            if (index < 0 || originalSplitter->replaceWidget(index, content) != placeholder)
                return QStringLiteral("The original splitter slot changed.");
            originalSplitter->setSizes(splitterSizes);
        } else if (originalLayout) {
            if (originalParent->layout() != originalLayout || originalLayout->indexOf(placeholder) < 0)
                return QStringLiteral("The original layout changed.");
            auto item = originalLayout->replaceWidget(placeholder, content, Qt::FindDirectChildrenOnly);
            if (!item) return QStringLiteral("Could not restore the panel slot.");
            delete item;
            content->setParent(originalParent);
        } else {
            if (originalParent->layout()) return QStringLiteral("Resolve added a layout to this panel slot.");
            content->setParent(originalParent);
        }
        content->setWindowFlags(flags);
        content->setGeometry(geometry);
        content->setVisible(!hidden);
        if (originalLayout) originalLayout->activate();
        disconnect(parentDestroyed);
        if (originalPage) originalPage->removeEventFilter(this);
        auto oldHost = host.data();
        oldHost->removeEventFilter(this);
        delete placeholder.data();
        clear();
        oldHost->hide();
        oldHost->deleteLater();
        return {};
    }
protected:
    bool eventFilter(QObject *object, QEvent *event) override
    {
        if (object == host && event->type() == QEvent::Close) {
            static_cast<QCloseEvent *>(event)->setAccepted(dock().isEmpty());
            return true;
        }
        if (object == originalPage && host) {
            if (event->type() == QEvent::Hide) host->hide();
            if (event->type() == QEvent::Show) host->show();
        }
        return false;
    }
private:
    void clear()
    {
        host = nullptr; content = nullptr; placeholder = nullptr;
        originalParent = nullptr; originalLayout = nullptr;
        originalSplitter = nullptr; originalPage = nullptr;
        splitterSizes.clear();
    }
    QPointer<QWidget> host, content, placeholder, originalParent, originalPage;
    QPointer<QLayout> originalLayout;
    QPointer<QSplitter> originalSplitter;
    QMetaObject::Connection parentDestroyed;
    QList<int> splitterSizes;
    QRect geometry;
    Qt::WindowFlags flags;
    bool hidden = false;
};
}
