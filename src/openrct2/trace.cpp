#include <stdio.h>
#include <debugnet.h>

extern "C" {

static int nest = 0;
void __cyg_profile_func_enter(void *fn, void *callsite);
void __cyg_profile_func_enter(void *fn, void *callsite) {
    for (int i = 0; i < nest; i++) {
        debugNetPrintf(99, "  ");
    }
    debugNetPrintf(99, "> %p %p\n", fn, callsite);
    nest++;
}

void __cyg_profile_func_exit(void *fn, void *callsite);
void __cyg_profile_func_exit(void *fn, void *callsite) {
    for (int i = 0; i < nest; i++) {
        debugNetPrintf(99, "  ");
    }
    debugNetPrintf(99, "< %p %p\n", fn, callsite);
    nest--;
}

}