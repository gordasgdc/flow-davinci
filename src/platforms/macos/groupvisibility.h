// SPDX-License-Identifier: MIT
#pragma once
#include <QHash>
#include <QMap>
#include <QPointer>
#include <QSplitter>
#include <QTabWidget>
namespace Unbound {
// Remember divider sizes for each open/closed combination, rather than rescaling repeatedly.
class GroupVisibility
{
public:
    void reset() { splitters.clear(); returnTabs.clear(); }
    void remember(QWidget *root)
    {
        auto items = root->findChildren<QSplitter *>();
        if (auto splitter = qobject_cast<QSplitter *>(root)) items.prepend(splitter);
        for (auto splitter : items) splitters[splitter][mask(splitter)] = splitter->sizes();
    }
    void restore(QWidget *root)
    {
        auto items = root->findChildren<QSplitter *>();
        if (auto splitter = qobject_cast<QSplitter *>(root)) items.prepend(splitter);
        for (auto splitter : items) {
            splitter->refresh();
            const auto sizes = splitters.value(splitter).value(mask(splitter));
            if (sizes.size() == splitter->count()) splitter->setSizes(sizes);
        }
    }
    void tabVisibility(QTabWidget *tabs, QWidget *panel, bool visible, bool select)
    {
        const int index = tabs->indexOf(panel); if (index < 0) return;
        if (visible && select && tabs->currentWidget() != panel) returnTabs[tabs] = tabs->currentWidget();
        const bool closingSelected = !visible && tabs->currentWidget() == panel;
        tabs->setTabVisible(index, visible);
        if (visible && select) tabs->setCurrentWidget(panel);
        if (closingSelected) {
            auto prior = returnTabs.take(tabs);
            if (prior && tabs->indexOf(prior) >= 0 && tabs->isTabVisible(tabs->indexOf(prior))) tabs->setCurrentWidget(prior);
        }
    }
private:
    static QString mask(QSplitter *splitter)
    {
        QString result;
        for (int i = 0; i < splitter->count(); ++i) result += splitter->widget(i)->isHidden() ? QLatin1Char('0') : QLatin1Char('1');
        return result;
    }
    QHash<QSplitter *, QMap<QString, QList<int>>> splitters;
    QHash<QTabWidget *, QPointer<QWidget>> returnTabs;
};
}
