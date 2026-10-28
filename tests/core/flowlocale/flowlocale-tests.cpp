#include "../../../src/core/flowlocale.h"
#include <cstdio>
#include <cstdlib>
#include <QSet>
static int failures = 0;
static void check(bool ok, const char *what) { if (!ok) { ++failures; std::fprintf(stderr, "FAIL: %s\n", what); } }
int main() {
    check(FD("Undock") == QString::fromUtf8("Detașează"), "RO translation");
    check(FD("Not in catalog") == QStringLiteral("Not in catalog"), "missing key falls back to source");
    QSet<QByteArray> seen; bool dup = false, empty = false, brand = false;
    for (const auto &e : Flow::kRomanianCatalog) {
        dup |= seen.contains(e.source); seen.insert(e.source);
        empty |= !*e.target;
        brand |= QString::fromUtf8(e.target).contains(QStringLiteral("OpenGate")) && !QString::fromUtf8(e.source).startsWith(QStringLiteral("OpenGate by CineWaltz"));
    }
    check(!dup, "no duplicate keys"); check(!empty, "no empty translations"); check(!brand, "no OpenGate in visible RO text");
    if (failures) return 1;
    std::puts("PASS: Flow DaVinci RO catalog (translation, fallback, unique keys, branding)");
    return 0;
}
