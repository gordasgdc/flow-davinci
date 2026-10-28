// SPDX-License-Identifier: MIT
#pragma once
#include <QString>
#include "flowlocale.h"
namespace Unbound {
inline QString productName() { return QStringLiteral("OpenGate"); }
inline QString productVendor() { return QStringLiteral("CineWaltz"); }
inline QString productCredit() {
    return FD("OpenGate by CineWaltz\nBuilt on Unbound by Wolfgang Woehl (MIT license).");
}
}
