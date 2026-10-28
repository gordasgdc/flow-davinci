// SPDX-License-Identifier: MIT
#pragma once
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include "flowlocale.h"
namespace Unbound {
inline QString productName() { return QStringLiteral("Flow DaVinci"); }
inline QString productVendor() { return QStringLiteral("Cristi Gordas (GDC)"); }
inline QString productCredit() {
    return FD("OpenGate by CineWaltz\nBuilt on Unbound by Wolfgang Woehl (MIT license).");
}
// Sursa unică a versiunii instalate: release-manifest.json scris de pachet (macOS) sau constantele de compilare (Windows).
struct ProductVersion { QString version; int buildNumber = 0; QString build; bool known = false; };
inline QString installedManifestPath() {
#ifdef Q_OS_MACOS
    return qEnvironmentVariable("FLOW_DAVINCI_TEST_MANIFEST", QStringLiteral("/Library/Application Support/GDC/FlowDaVinci/Current/release-manifest.json"));
#else
    return qEnvironmentVariable("FLOW_DAVINCI_TEST_MANIFEST");
#endif
}
inline ProductVersion productVersion() {
    ProductVersion result;
    const auto path = installedManifestPath();
    QFile file(path);
    if (!path.isEmpty() && file.open(QIODevice::ReadOnly)) {
        const auto object = QJsonDocument::fromJson(file.readAll()).object();
        result.version = object.value("version").toString();
        result.buildNumber = object.value("build_number").toInt();
        result.build = object.value("build").toString();
        result.known = !result.version.isEmpty();
    }
#ifdef FLOW_DAVINCI_VERSION
    if (!result.known) { result.version = QStringLiteral(FLOW_DAVINCI_VERSION); result.buildNumber = FLOW_DAVINCI_BUILD; result.known = true; }
#endif
    return result;
}
}
