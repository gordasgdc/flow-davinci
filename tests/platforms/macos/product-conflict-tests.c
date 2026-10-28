/* Flow DaVinci: detectia OpenGate/Unbound deja incarcate in proces (nu modifica nimic). */
#include "ofx-loading-probe.c"
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (other_product_image()) { fprintf(stderr, "FAIL: false positive before loading another product\n"); return 1; }
    if (!dlopen(argv[1], RTLD_NOW | RTLD_LOCAL)) { fprintf(stderr, "FAIL: fixture dylib: %s\n", dlerror()); return 1; }
    const char *found = other_product_image();
    if (!found || !strstr(found, "/Unbound.dvcp.bundle/")) { fprintf(stderr, "FAIL: other product not detected\n"); return 1; }
    puts("PASS: OpenGate/Unbound image detected; no false positive for Flow DaVinci");
    return 0;
}
