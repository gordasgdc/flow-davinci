// SPDX-License-Identifier: MIT
#pragma once
#include "flowlocale.h"
#include <QString>
#include "droppolicy.h"
namespace Unbound {
inline QString dropCaption(DropPlacement placement, const QString &title, bool wholeGroup)
{
    const auto destination = wholeGroup ? FD("entire group") : Flow::label(title);
    if (placement == DropPlacement::Tab) return FD("Add as tab to %1").arg(destination);
    const auto direction = placement == DropPlacement::Left ? FD("left of") :
        placement == DropPlacement::Right ? FD("right of") :
        placement == DropPlacement::Above ? FD("above") :
        placement == DropPlacement::Below ? FD("below") : QString();
    return direction.isEmpty() ? QString() : FD("Dock %1 %2").arg(direction, destination);
}
}
