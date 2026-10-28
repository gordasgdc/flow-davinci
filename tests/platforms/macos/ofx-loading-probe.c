#include <ofxCore.h>
#include <dlfcn.h>
#include <dispatch/dispatch.h>
#include <mach-o/dyld.h>
#include <CoreFoundation/CoreFoundation.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>

static void record_state(const char *phase, int inspect_windows)
{
    const char *home = getenv("HOME");
    if (!home) return;
    char path[4096];
    int length = snprintf(path, sizeof(path), "%s/Library/Logs/FlowDaVinci/ofx-loading-probe.log", home);
    if (length < 0 || (size_t)length >= sizeof(path)) return;
    FILE *log = fopen(path, "a");
    if (!log) return;
    char executable[4096] = "unknown";
    uint32_t size = sizeof(executable);
    if (_NSGetExecutablePath(executable, &size) != 0)
        snprintf(executable, sizeof(executable), "path exceeds buffer");
    /* Read the exported singleton pointer without linking another Qt runtime. */
    void **application = (void **)dlsym(RTLD_DEFAULT, "_ZN16QCoreApplication4selfE");
    fprintf(log, "time=%lld phase=%s pid=%d main_thread=%d executable=%s qt_symbol=%s qt_application=%s",
            (long long)time(NULL), phase, getpid(), pthread_main_np(), executable,
            application ? "found" : "absent",
            application && *application ? "present" : "absent");
    if (inspect_windows && pthread_main_np() && application && *application) {
        void *(*active_window)(void) = (void *(*)(void))dlsym(RTLD_DEFAULT, "_ZN12QApplication12activeWindowEv");
        fprintf(log, " active_window=%s", active_window && active_window() ? "present" : "absent");
    }
    fputc('\n', log);
    fclose(log);
}


/* Flow DaVinci nu porneste alaturi de OpenGate/Unbound in aceeasi instanta Resolve: ambele ar gestiona aceleasi widgeturi Qt.
   Detectam imaginile lor deja incarcate; nu modificam si nu dezactivam nimic. */
static const char *other_product_image(void)
{
    static const char *markers[] = {"/Unbound.dvcp.bundle/", "UnboundWidgetProbe.dylib", "UnboundLoadingProbe.ofx", "/OpenGate.dvcp.bundle/"};
    for (uint32_t i = 0; i < _dyld_image_count(); ++i) {
        const char *name = _dyld_get_image_name(i);
        if (!name) continue;
        for (size_t m = 0; m < sizeof(markers) / sizeof(markers[0]); ++m)
            if (strstr(name, markers[m])) return name;
    }
    return NULL;
}

static void show_conflict_notice(void *context)
{
    (void)context;
    CFStringRef title = CFStringCreateWithCString(NULL, "Flow DaVinci nu a pornit", kCFStringEncodingUTF8);
    CFStringRef text = CFStringCreateWithCString(NULL,
        "În această instanță DaVinci Resolve este deja încărcat OpenGate sau Unbound. Două produse nu pot gestiona simultan aceeași interfață.\n\n"
        "Nimic nu a fost șters sau modificat. Pentru a folosi Flow DaVinci: închide Resolve și rulează «Flow DaVinci Tools → Alege produsul activ» (reversibil).",
        kCFStringEncodingUTF8);
    CFUserNotificationDisplayNotice(0, kCFUserNotificationCautionAlertLevel, NULL, NULL, NULL, title, text, CFSTR("OK"));
    CFRelease(title); CFRelease(text);
}

static void inspect_on_main(void *context)
{
    void **application = (void **)dlsym(RTLD_DEFAULT, "_ZN16QCoreApplication4selfE");
    if (!application || !*application) {
        uintptr_t attempt = (uintptr_t)context;
        if (attempt < 120) {
            dispatch_after_f(dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC / 2),
                dispatch_get_main_queue(), (void *)(attempt + 1), inspect_on_main);
        } else {
            record_state("qt-startup-timeout", 0);
        }
        return;
    }
    record_state("main-queue", 1);
    Dl_info info;
    if (!dladdr((const void *)&inspect_on_main, &info)) return;
    if (other_product_image()) {
        record_state("other-product-loaded", 0);
        dispatch_async_f(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), NULL, show_conflict_notice);
        return;
    }
    char path[4096];
    int length = snprintf(path, sizeof(path), "%s", info.dli_fname);
    if (length < 0 || (size_t)length >= sizeof(path)) return;
    char *name = strrchr(path, '/');
    if (!name) return;
    *name = '\0';
    const char *suffix = "/FlowDaVinciWorkspace.dylib";
    if (strlen(path) + strlen(suffix) >= sizeof(path)) return;
    strcat(path, suffix);
    if (access(path, R_OK) != 0) return;
    // The optional Qt inspector is loaded only inside the Qt host, on main.
    void *library = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!library) { record_state("widget-library-load-failed", 0); return; }
    void (*inspect)(void) = (void (*)(void))dlsym(library, "unboundInspectWidgets");
    if (inspect) { record_state("widget-inspection-start", 0); inspect(); }
    // Retain the library while its bounded Qt timer callbacks are pending.
}

/* Discovery only: advertise no effects and never change a project or widget. */
__attribute__((visibility("default"))) int OfxGetNumberOfPlugins(void)
{
    record_state("discovery", 0);
    static atomic_flag scheduled = ATOMIC_FLAG_INIT;
    if (dlsym(RTLD_DEFAULT, "_ZN16QCoreApplication4selfE") &&
        !atomic_flag_test_and_set(&scheduled)) {
        /* Keep this callback's code loaded after the host releases the probe. */
        Dl_info info;
        if (dladdr((const void *)&inspect_on_main, &info) &&
            dlopen(info.dli_fname, RTLD_NOW | RTLD_LOCAL)) {
            dispatch_async_f(dispatch_get_main_queue(), NULL, inspect_on_main);
        } else {
            record_state("callback-not-scheduled", 0);
        }
    }
    return 0;
}

__attribute__((visibility("default"))) OfxPlugin *OfxGetPlugin(int index)
{
    (void)index;
    return NULL;
}
