// SPDX-License-Identifier: MIT
#pragma once
#include <QLibrary>
#include <QPointer>
#include <windows.h>
// Reuse the assembled flow's assertions, but obtain its controller through the
// shipped DLL's real launcher entry. No fixture-only production export is used.
class ManagedBootstrapFixture {
public:
    explicit ManagedBootstrapFixture(const QString &path):library(path) {
        library.setLoadHints(QLibrary::PreventUnloadHint);
        if(!library.load()){error=library.errorString();return;}
        using Start=DWORD(WINAPI *)(void *);
        const auto start=reinterpret_cast<Start>(library.resolve("UnboundStart"));
        if(!start||start(nullptr)!=1||start(nullptr)!=1){error="Windows bootstrap failed or was not idempotent";return;}
        QElapsedTimer timer;timer.start();
        while(timer.elapsed()<4000){
            QApplication::processEvents();
            service=qApp->findChild<QObject *>("openGateManagedWorkspaceService",Qt::FindDirectChildrenOnly);
            if(service)for(auto widget:QApplication::topLevelWidgets())
                if(widget->objectName()=="unboundPanelTest")controls=dynamic_cast<PanelControls *>(widget);
            if(controls)break;
            QThread::msleep(1);
        }
        if(!controls)error="Shipped DLL did not create the shared controller on the GUI thread";
        if(qApp->findChildren<QObject *>("openGateManagedWorkspaceService",Qt::FindDirectChildrenOnly).size()!=1)
            error="Repeated bootstrap created duplicate services";
    }
    ~ManagedBootstrapFixture(){delete service.data();}
    PanelControls *controller()const{return error.isEmpty()?controls:nullptr;}
    QString error;
private:
    QLibrary library;
    QPointer<QObject> service;
    QPointer<PanelControls> controls;
};
