// SPDX-License-Identifier: MIT
#pragma once
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
namespace Unbound {
inline QString settingsProfilePath(const QString &file) {
    const auto base=QDir::homePath()+"/Library/Application Support/Unbound";
    QFile marker(qEnvironmentVariable("UNBOUND_PROFILE_FILE",base+"/Profile.json"));
    QString profile;
    if (marker.open(QIODevice::ReadOnly)) profile=QJsonDocument::fromJson(marker.readAll()).object()["active"].toString();
    return base+(profile=="first-install-preview" ? "/Profiles/first-install-preview/" : "/")+file;
}
}
