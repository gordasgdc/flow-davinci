// SPDX-License-Identifier: MIT
#pragma once
#include <QRect>
namespace Unbound {
// Only expanded content uses these gates. Normal floating placement is free.
inline QRect workspaceBandBounds(QRect available,const QRect &header,const QRect &footer) {
    if(!header.isEmpty() && available.intersects(header))available.setTop(qMax(available.top(),header.bottom()+1));
    if(!footer.isEmpty() && available.intersects(footer))available.setBottom(qMin(available.bottom(),footer.top()-1));
    return available;
}
}
