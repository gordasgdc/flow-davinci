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
        if (original) {
            wasHidden=original->isHidden(); minimum=original->minimumSize(); policy=original->sizePolicy();
            original->setMinimumSize(0,0); original->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored); original->hide();
        }
    }
    void restore() {
        if (original) { original->setSizePolicy(policy); original->setMinimumSize(minimum); original->setHidden(wasHidden); }
        original=nullptr;
    }
private:
    QPointer<QWidget> original; QSize minimum; QSizePolicy policy; bool wasHidden=false;
};
}
