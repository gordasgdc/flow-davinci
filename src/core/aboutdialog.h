// SPDX-License-Identifier: MIT
#pragma once
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QProcess>
#include <QProcessEnvironment>
#include <QUrl>
#include "flowlocale.h"
#include "productidentity.h"
namespace Unbound {
// Text „Despre Flow DaVinci”: identitate, versiune/build (sursa unică: release-manifest.json), stare, licență, atribuiri.
inline QString updateHelperPath() {
#ifdef Q_OS_MACOS
    return qEnvironmentVariable("FLOW_DAVINCI_TEST_UPDATE_HELPER", QStringLiteral("/Applications/Flow DaVinci.app/Contents/MacOS/flow-update"));
#else
    return QString();
#endif
}
inline bool updatesSupported() { const auto path = updateHelperPath(); return !path.isEmpty() && QFileInfo::exists(path); }
inline QString aboutText() {
    const auto info = productVersion();
    QString version = info.known ? info.version : FD("unknown");
    QString build = info.buildNumber > 0 ? QString::number(info.buildNumber) : QStringLiteral("—");
    if (!info.build.isEmpty()) build += QStringLiteral(" (") + info.build + QStringLiteral(")");
    return QStringLiteral("<b>%1</b><br>%2 %3 · %4 %5<br><b>%6</b><br><br>%7<br><br>%8<br><br>%9<br><br>%10")
        .arg(productName().toHtmlEscaped(), FD("Version").toHtmlEscaped(), version.toHtmlEscaped(), FD("build").toHtmlEscaped(), build.toHtmlEscaped(),
             FD("Test version").toHtmlEscaped(),
             FD("Adapted and coordinated by Cristi Gordas (GDC). Free software, MIT license; copyright notices of the original authors are kept in the package.").toHtmlEscaped(),
             productCredit().toHtmlEscaped().replace('\n', QStringLiteral("<br>")),
             FD("Independent project, not affiliated with Blackmagic Design. DaVinci Resolve is a trademark of Blackmagic Design.").toHtmlEscaped(),
             updatesSupported() ? FD("Updates: the Check for Updates… command compares this version with the signed Flow DaVinci release information.").toHtmlEscaped()
                                : FD("Updates: not available for this platform yet.").toHtmlEscaped());
}
// Rulează helperul de actualizare (verificare/descărcare) și întoarce JSON-ul lui; nu blochează mai mult de `timeoutMs`.
inline QJsonObject runUpdateHelper(const QString &command, int timeoutMs) {
    QProcess process;
    process.start(updateHelperPath(), {command});
    if (!process.waitForFinished(timeoutMs)) { process.kill(); return {{"status","failed"},{"reason","timeout"}}; }
    const auto parsed = QJsonDocument::fromJson(process.readAllStandardOutput());
    if (!parsed.isObject()) return {{"status","failed"},{"reason","helper"}};
    return parsed.object();
}
// Mesajele către utilizator pentru fiecare rezultat; testabile fără interfață (vezi tests/core/about).
inline QString updateOutcomeText(const QJsonObject &result) {
    const auto status = result.value("status").toString();
    if (status == "up_to_date") return FD("Flow DaVinci is up to date.");
    if (status == "available") {
        QString text = FD("A new version is available: %1 (build %2).").arg(result.value("version").toString()).arg(result.value("build").toInt());
        const auto notes = result.value("notes").toString();
        if (!notes.isEmpty()) text += QStringLiteral("\n\n") + notes;
        return text;
    }
    const auto reason = result.value("reason").toString();
    if (reason == "offline" || reason == "timeout") return FD("Could not check for updates: no connection to the update server.");
    return FD("Could not check for updates: the server response is not valid.");
}
inline void checkForUpdates() {
    if (!updatesSupported()) {
        QMessageBox::information(nullptr, FD("Check for Updates…"), FD("Updates are not available for this platform yet."));
        return;
    }
    const auto result = runUpdateHelper(QStringLiteral("check"), 40000);
    if (result.value("status").toString() != "available") {
        const bool failed = result.value("status").toString() == "failed";
        QMessageBox box(failed ? QMessageBox::Warning : QMessageBox::Information, FD("Check for Updates…"), updateOutcomeText(result), QMessageBox::Ok);
        box.exec();
        return;
    }
    QMessageBox box(QMessageBox::Question, FD("Check for Updates…"), updateOutcomeText(result), QMessageBox::NoButton);
    box.setInformativeText(FD("The package is downloaded, verified (SHA-256, signature, notarization) and then opened. Close DaVinci Resolve before installing: the installer does not replace components while Resolve runs, and your settings and presets are kept."));
    auto download = box.addButton(FD("Download and verify"), QMessageBox::AcceptRole);
    box.addButton(FD("Later"), QMessageBox::RejectRole);
    box.exec();
    if (box.clickedButton() != download) return;
    const auto downloaded = runUpdateHelper(QStringLiteral("download"), 600000);
    if (downloaded.value("status").toString() != "downloaded") {
        QMessageBox::warning(nullptr, FD("Check for Updates…"), downloaded.value("message").toString().isEmpty() ? FD("Download failed.") : downloaded.value("message").toString());
        return;
    }
    QMessageBox ready(QMessageBox::Information, FD("Check for Updates…"), FD("The update was downloaded and verified. Close DaVinci Resolve, then open the installer."), QMessageBox::NoButton);
    auto open = ready.addButton(FD("Open installer"), QMessageBox::AcceptRole);
    ready.addButton(FD("Later"), QMessageBox::RejectRole);
    ready.exec();
    if (ready.clickedButton() == open) QDesktopServices::openUrl(QUrl::fromLocalFile(downloaded.value("path").toString()));
}
inline void showAbout() {
    QMessageBox box(QMessageBox::NoIcon, FD("About OpenGate"), aboutText(), QMessageBox::Ok);
    box.setTextFormat(Qt::RichText);
    QPushButton *check = updatesSupported() ? box.addButton(FD("Check for Updates…"), QMessageBox::ActionRole) : nullptr;
    box.exec();
    if (check && box.clickedButton() == check) checkForUpdates();
}
}
