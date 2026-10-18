// SPDX-License-Identifier: MIT
#pragma once
#include <QPointer>
#include <QWidget>
namespace Unbound {
// The original widget keeps its layout slot/identity, but an empty transferred
// content container must not remain visibly occupying that slot.
class VacatedContents {
public:
    ~VacatedContents() { restore(); }
    void hide(QWidget *slot) {
        restore(); original=slot;
        if (original) { wasHidden=original->isHidden(); original->hide(); }
    }
    void restore() {
        if (original) original->setHidden(wasHidden);
        original=nullptr;
    }
private:
    QPointer<QWidget> original; bool wasHidden=false;
};
}
