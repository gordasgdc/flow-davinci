#include "fixtures.h"
#include "mainwindowsnap.h"

void initializePlatformTests()
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
}
std::function<unsigned long(void *)> resolveBootstrap(QLibrary &helper)
{
    auto start=reinterpret_cast<DWORD (WINAPI *)(void *)>(helper.resolve("UnboundStart"));
    if (!start) return {};
    return [start](void *argument) { return start(argument); };
}
QRect nativeWindowBounds(QWidget &window)
{
    RECT rect{};
    GetWindowRect(reinterpret_cast<HWND>(window.winId()), &rect);
    return QRect(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
}
void prepareNativePlacementTest(QWidget &main, Unbound::PaneSettings &settings)
{
    if (QGuiApplication::platformName()==QLatin1String("windows")) {
        // Model Resolve's mismatched Qt frame cache: the Qt rectangle is
        // deliberately wrong, while the recorded native placement is exact.
        auto record=QJsonDocument::fromJson(settings.geometry("MainWindow/WStackPage_Conform")).object();
        check(record.value("format").toString()=="unbound-main-placement-1","Native placement was not recorded");
        record.insert("qt",QString::fromLatin1(main.saveGeometry().toBase64()));
        settings.save("MainWindow/WStackPage_Conform",QJsonDocument(record).toJson(QJsonDocument::Compact));
    }
}
void verifyNativePlacementTest(QWidget &main, const QRect &expectedNative)
{
    if (QGuiApplication::platformName()==QLatin1String("windows")) {
        const auto actualNative=nativeWindowBounds(main);
        check(expectedNative==actualNative,"Native restoration lost exact rectangle or used mismatched Qt geometry");
    }
}
bool runPlatformDiscoveryTests(QApplication &app)
{
    if (app.arguments().contains("--discovery-test")) {
        QTemporaryDir directory;
        check(directory.isValid(),"Cannot create discovery settings directory");
        qputenv("UNBOUND_SETTINGS_FILE",directory.filePath("Unbound.json").toUtf8());
        QLibrary helper(app.arguments().last()); helper.setLoadHints(QLibrary::PreventUnloadHint);
        check(helper.load(),qPrintable(helper.errorString()));
        auto start=reinterpret_cast<DWORD (WINAPI *)(void *)>(helper.resolve("UnboundStart"));
        check(start && start(reinterpret_cast<void *>(1))==0,"Normal helper accepted a startup mode argument");
        check(start(nullptr)==1,"Discovery bootstrap failed");
        app.processEvents();
        auto settle=[] { for (int i=0;i<8;++i) QCoreApplication::processEvents(); };
        // The project main window and its panes do not exist at injection time.
        QWidget main; main.setObjectName("UiMainWindow"); main.show(); settle();
        auto page=new QWidget(&main); page->setObjectName("frameEditPageWidgetsContainer"); page->show();
        auto makePane=[&] {
            auto pane=new QWidget(page); pane->setObjectName("frameEditPageMediaPool");
            auto layout=new QVBoxLayout(pane); layout->addWidget(new QPushButton("Live content",pane));
            pane->show(); return pane;
        };
        auto pane=makePane(); settle();
        check(pane->findChildren<QToolButton *>("resolvePaneUndockButton").size()==1,"Late pane not discovered");
        auto toggle=new QPushButton("Native pool",page); toggle->setObjectName("btnEditToggleMediaPoolInSSM");
        toggle->setCheckable(true); toggle->setChecked(true); toggle->show(); settle();
        auto button=pane->findChild<QToolButton *>("resolvePaneUndockButton");
        button->click(); settle();
        check(button->toolTip()=="Dock" && !toggle->isChecked(),"Late toggle not cached or manual detach failed");
        button->click(); settle();
        check(button->toolTip()=="Undock" && toggle->isChecked(),"Dock state not synchronized without polling");
        delete pane; delete toggle; settle();
        pane=makePane(); settle();
        check(pane->findChildren<QToolButton *>("resolvePaneUndockButton").size()==1,"Replacement pane not discovered");
        // A hidden lazy page is indexed when constructed, without selecting it.
        auto color=new QWidget(&main); color->setObjectName("WStackPage_Color"); color->hide();
        auto nodes=new QWidget(color); nodes->setObjectName("m_pFrameNodeGraph");
        settle();
        check(nodes->findChildren<QToolButton *>("resolvePaneUndockButton").isEmpty(),"Layout-less pane decorated");
        auto grid=new QGridLayout(nodes); grid->addWidget(new QPushButton("Graph",nodes)); settle();
        check(nodes->findChildren<QToolButton *>("resolvePaneUndockButton").size()==1,"Late hidden layout not discovered");
        page->hide(); color->show(); settle(); color->hide(); page->show(); settle();
        auto controller=app.findChild<QObject *>("UnboundPaneController");
        check(controller,"Controller missing");
        QEventLoop idle; QTimer::singleShot(1250,&idle,&QEventLoop::quit); idle.exec();
        for (auto timer:controller->findChildren<QTimer *>())
            check(!timer->isActive() || timer->isSingleShot(),"Recurring controller timer remains active");
        check(pane->findChildren<QToolButton *>("resolvePaneUndockButton").size()==1,"Page switching duplicated decoration");
        delete controller;
        std::puts("PASS: injection before project, late pane/toggle/layout, replacement, page switching and no recurring discovery timer");
        return true;
    }
    return false;
}
bool runPlatformWindowTests(QApplication &app)
{
    if (app.arguments().contains("--snap-native")) {
        QWidget main;
        main.setObjectName("UiMainWindow");
        main.setGeometry(100, 100, 400, 300);
        Unbound::PaneFloatingWindow peer;
        peer.setObjectName("gammarayFloating_snapTest");
        peer.setGeometry(950, 120, 250, 300);
        main.show(); peer.show(); app.processEvents();
        const auto handle = reinterpret_cast<HWND>(main.winId());
        const auto peerHandle = reinterpret_cast<HWND>(peer.winId());
        DwmFlush();
        RECT original;
        GetWindowRect(handle, &original);
        auto contact = Unbound::paneContactBounds(handle);
        auto other = Unbound::paneContactBounds(peerHandle);
        Unbound::MainWindowSnap filter;
        filter.setWindow(&main);
        SendMessage(handle, WM_ENTERSIZEMOVE, 0, 0);
        RECT moving = original;
        OffsetRect(&moving, other.x() - contact.x() - contact.width() + 6, 0);
        check(SendMessage(handle, WM_MOVING, 0, reinterpret_cast<LPARAM>(&moving)) == TRUE, "Main native move message not handled");
        const int insetRight = original.right - contact.x() - contact.width();
        check(moving.right - insetRight == other.x(), "Main movement did not join contact edges exactly");
        RECT sizing = original;
        sizing.right = other.x() + insetRight + 6;
        check(SendMessage(handle, WM_SIZING, WMSZ_RIGHT, reinterpret_cast<LPARAM>(&sizing)) == TRUE,
            "Main native sizing message not handled");
        check(sizing.left == original.left && sizing.right - insetRight == other.x(), "Main resize snapped wrong edge or left a gap");
        SendMessage(handle, WM_EXITSIZEMOVE, 0, 0);
        filter.setWindow(nullptr);
        Unbound::PaneSnapWindows snap;
        snap.begin(handle);
        MINMAXINFO limits{};
        RECT bypass = original;
        bypass.right = other.x() + insetRight + 6;
        const auto unchanged = bypass;
        snap.apply(bypass, HTRIGHT, limits, true);
        check(bypass.right == unchanged.right, "Explicit free-drag bypass snapped");
        peer.hide(); app.processEvents();
        snap.apply(bypass, HTRIGHT, limits);
        check(bypass.right == unchanged.right, "Hidden target participated in snapping");
        peer.show(); app.processEvents(); DwmFlush();
        // Exercise the actual nonmodal floating resize path against main.
        const auto peerBounds = Unbound::paneContactBounds(peerHandle);
        RECT peerRect;
        GetWindowRect(peerHandle, &peerRect);
        SendMessage(peerHandle, WM_NCLBUTTONDOWN, HTLEFT, 0);
        const auto cursor = peer.dragStartPosition();
        const int desiredLeft = contact.x() + contact.width() - (peerBounds.x() - peerRect.left) + 6;
        peer.updateInteractiveDrag(POINT{cursor.x + desiredLeft - peerRect.left, cursor.y});
        SendMessage(peerHandle, WM_LBUTTONUP, 0, 0);
        DwmFlush();
        check(Unbound::paneContactBounds(peerHandle).x() == contact.x() + contact.width(),
            "Floating resize path did not snap to main without a seam");
        if (app.arguments().contains("--restore-probe")) {
            app.processEvents();
            const auto saved=peer.saveGeometry();
            const auto originalContact=Unbound::paneContactBounds(peerHandle);
            peer.hide();
            Unbound::PaneFloatingWindow restored;
            restored.winId();
            restored.restoreGeometry(saved); restored.show(); app.processEvents(); DwmFlush();
            const auto restoredContact=Unbound::paneContactBounds(reinterpret_cast<HWND>(restored.winId()));
            std::fprintf(stderr,"Saved contact %d %d %d %d; restored %d %d %d %d\n",
                originalContact.x(),originalContact.y(),originalContact.width(),originalContact.height(),
                restoredContact.x(),restoredContact.y(),restoredContact.width(),restoredContact.height());
        }
        std::puts("PASS: native main move/resize routing, floating resize, exact contact alignment, bypass and hidden-window exclusion");
        return true;
    }
    if (app.arguments().contains("--taskbar-native")) {
        QWidget source;
        IPropertyStore *properties = nullptr;
        check(SUCCEEDED(SHGetPropertyStoreForWindow(reinterpret_cast<HWND>(source.winId()), IID_PPV_ARGS(&properties))), "Source property store unavailable");
        PROPVARIANT id;
        PropVariantInit(&id);
        id.vt = VT_LPWSTR;
        id.pwszVal = const_cast<wchar_t *>(L"ResolvePane.NativeTest");
        check(SUCCEEDED(properties->SetValue(PKEY_AppUserModel_ID, id)), "Setting test identity failed");
        properties->Release();
        Unbound::PaneFloatingWindow host;
        auto dragLayout = new QVBoxLayout(&host);
        dragLayout->addWidget(new QWidget(&host));
        Unbound::inheritPaneTaskbarIdentity(&host, &source);
        auto handle = reinterpret_cast<HWND>(host.winId());
        check(GetWindow(handle, GW_OWNER) == nullptr, "Native pane has an owner");
        check(!(GetWindowLongPtr(handle, GWL_EXSTYLE) & WS_EX_TOOLWINDOW), "Native pane is a tool window");
        check(SUCCEEDED(SHGetPropertyStoreForWindow(handle, IID_PPV_ARGS(&properties))), "Destination property store unavailable");
        PROPVARIANT result;
        PropVariantInit(&result);
        check(SUCCEEDED(properties->GetValue(PKEY_AppUserModel_ID, &result)) && result.vt == VT_LPWSTR
            && wcscmp(result.pwszVal, L"ResolvePane.NativeTest") == 0, "Window taskbar grouping identity not copied");
        PropVariantClear(&result);
        properties->Release();
        check(Unbound::applyPaneDarkCaption(&host, &source), "Windows rejected dark caption colors");
        RECT bounds{0, 0, 500, 400};
        SendMessage(handle, WM_SIZING, WMSZ_RIGHT, reinterpret_cast<LPARAM>(&bounds));
        check(!dragLayout->isEnabled(), "Native sizing did not defer layout");
        SendMessage(handle, WM_EXITSIZEMOVE, 0, 0);
        check(dragLayout->isEnabled(), "Native exit did not resume layout");
        POINT cursor;
        GetCursorPos(&cursor);
        SendMessage(handle, WM_NCLBUTTONDOWN, HTRIGHT, 0);
        check(host.interactiveDragActive() && !dragLayout->isEnabled(), "Custom resize did not capture input");
        RECT before, after;
        GetWindowRect(handle, &before);
        cursor = host.dragStartPosition();
        host.updateInteractiveDrag(POINT{cursor.x + 80, cursor.y});
        GetWindowRect(handle, &after);
        check(after.right > before.right && after.left == before.left, "Custom right-edge resize failed");
        SendMessage(handle, WM_LBUTTONUP, 0, 0);
        check(!host.interactiveDragActive() && dragLayout->isEnabled() && GetCapture() != handle, "Custom resize release failed");
        GetCursorPos(&cursor);
        SendMessage(handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        check(host.interactiveDragActive() && dragLayout->isEnabled(), "Moving unnecessarily deferred content");
        GetWindowRect(handle, &before);
        cursor = host.dragStartPosition();
        host.updateInteractiveDrag(POINT{cursor.x + 40, cursor.y + 20});
        GetWindowRect(handle, &after);
        check(after.left == before.left + 40 && after.top == before.top + 20, "Custom move failed");
        SendMessage(handle, WM_KEYDOWN, VK_ESCAPE, 0);
        GetWindowRect(handle, &after);
        check(!host.interactiveDragActive() && after.left == before.left && after.top == before.top, "Escape did not cancel movement");
        SendMessage(handle, WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, 0);
        ReleaseCapture();
        check(!host.interactiveDragActive() && dragLayout->isEnabled(), "Capture loss left resize active");
        std::puts("PASS: nonmodal move/resize, mouse release, Escape cancellation and capture-loss cleanup");
        std::puts("PASS: native resize message routing");
        std::puts("PASS: dark caption colors accepted; native unowned window, taskbar-compatible style, explicit AppUserModelID copied");
        return true;
    }
    return false;
}
