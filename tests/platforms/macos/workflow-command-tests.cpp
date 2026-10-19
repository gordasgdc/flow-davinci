// SPDX-License-Identifier: MIT
#include <QApplication>
#include <QSaveFile>
#include <QTemporaryDir>
#include <QElapsedTimer>
#include <QThread>
#include <cstdio>
#include "workflowcommandbridge.h"
int main(int argc,char **argv) {
    QApplication app(argc,argv); QTemporaryDir files; qputenv("UNBOUND_DIAGNOSTICS_DIR",files.path().toUtf8());
    int failures=0; auto check=[&](bool ok,const char *why) { if (!ok) { ++failures; std::fprintf(stderr,"FAIL: %s\n",why); } };
    int accepted=0,attempts=0;
    Unbound::WorkflowCommandBridge bridge(nullptr,files.path(),[&](QString id) { ++attempts; if (id!="panel.toggle.source") return false; ++accepted; return true; });
    auto request=[&](const QString &file,const QByteArray &bytes) { QSaveFile out(files.path()+"/command-requests/"+file); out.open(QIODevice::WriteOnly); out.write(bytes); out.commit(); Unbound::postNativeCommandRequestForTest(); };
    auto settle=[] { QElapsedTimer timer; timer.start(); while (timer.elapsed()<250) { QApplication::processEvents(); QThread::msleep(1); } };
    const auto json=[&](QString id,qint64 time) { return QJsonDocument(QJsonObject{{"command",id},{"issued_ms",double(time)}}).toJson(); };
    request("one.json",json("panel.toggle.source",QDateTime::currentMSecsSinceEpoch())); settle();
    check(accepted==1 && attempts==1,"native workflow request dispatches a whitelisted command on GUI thread");
    settle(); check(accepted==1,"watcher notifications cannot replay a consumed command");
    request("stale.json",json("panel.toggle.source",QDateTime::currentMSecsSinceEpoch()-20000));
    request("future.json",json("panel.toggle.source",QDateTime::currentMSecsSinceEpoch()+20000));
    request("invalid.json","not JSON"); request("oversize.json",QByteArray(5000,'x')); settle();
    check(accepted==1 && attempts==1,"stale future malformed and oversized commands cannot dispatch");
    request("unknown.json",json("arbitrary.code",QDateTime::currentMSecsSinceEpoch())); settle();
    check(attempts==2 && accepted==1,"unknown command IDs cannot bypass registry whitelist");
    check(QDir(files.path()+"/command-requests").entryList({"*.json"},QDir::Files).isEmpty(),"all requests are consumed once including rejected requests");
    std::printf("workflow command failures=%d\n",failures); return failures ? 1 : 0;
}
