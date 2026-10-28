// SPDX-License-Identifier: MIT
#include "../../../src/core/aboutdialog.h"
#include <QApplication>
#include <cstdio>
static int failures = 0;
static void check(bool ok, const char *what) { if (!ok) { ++failures; std::fprintf(stderr, "FAIL: %s\n", what); } }
int main(int argc, char **argv) {
    qunsetenv("FLOW_DAVINCI_LANG"); // verifică textele RO; matricea rulează restul testelor cu LANG=en
    QApplication app(argc, argv);
    using namespace Unbound;
    check(productName() == QStringLiteral("Flow DaVinci"), "product name is Flow DaVinci");
    check(!productName().contains(QStringLiteral("OpenGate")) && !productVendor().contains(QStringLiteral("CineWaltz")), "no OpenGate identity");
    const auto version = productVersion();
    check(version.known && version.version == QStringLiteral("0.1.0") && version.buildNumber == 42 && version.build == QStringLiteral("abc1234"), "version and build come from the single release manifest");
    const auto text = aboutText();
    check(text.contains(QStringLiteral("Flow DaVinci")) && text.contains(QStringLiteral("0.1.0")) && text.contains(QStringLiteral("42 (abc1234)")), "About shows version and build");
    check(text.contains(QStringLiteral("Versiune pentru testare")), "About shows test status");
    check(text.contains(QStringLiteral("Cristi Gordaș")) && text.contains(QStringLiteral("MIT")), "About shows identity and license");
    check(text.contains(QStringLiteral("OpenGate")) && text.contains(QStringLiteral("Unbound")) && text.contains(QStringLiteral("Wolfgang Woehl")), "About keeps the mandatory credits");
    check(text.contains(QStringLiteral("Blackmagic Design")), "About states non-affiliation");
    check(updateOutcomeText({{"status","up_to_date"}}) == QStringLiteral("Flow DaVinci este la zi."), "up-to-date text");
    const auto available = updateOutcomeText({{"status","available"},{"version","0.2.0"},{"build",7},{"notes","Noutăți."}});
    check(available.contains(QStringLiteral("0.2.0")) && available.contains(QStringLiteral("Noutăți.")), "new-version text with notes");
    check(updateOutcomeText({{"status","failed"},{"reason","offline"}}).contains(QStringLiteral("conexiune")), "offline text");
    check(updateOutcomeText({{"status","failed"},{"reason","invalid-response"}}).contains(QStringLiteral("nu este valid")), "invalid-response text");
    if (updatesSupported()) {
        const auto result = runUpdateHelper(QStringLiteral("check"), 30000);
        check(result.value("status").toString() == QStringLiteral("failed") && result.value("reason").toString() == QStringLiteral("offline"), "helper reports offline through the UI bridge");
    }
    if (failures) return 1;
    std::puts("PASS: Despre Flow DaVinci și verificarea actualizărilor (identitate, versiune, texte, punte helper)");
    return 0;
}
