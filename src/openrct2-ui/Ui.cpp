#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include <openrct2/audio/AudioContext.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/ui/UiContext.h>
#include "audio/AudioContext.h"
#include "Ui.h"
#include "UiContext.h"

extern "C"
{
    #include <openrct2/platform/platform.h>
}

#if defined(__vita__)
#include <psp2/kernel/threadmgr.h>
#endif

using namespace OpenRCT2;
using namespace OpenRCT2::Audio;
using namespace OpenRCT2::Ui;

#if defined(__vita__)

extern "C"
{
    unsigned int sleep(unsigned int seconds)
    {
        sceKernelDelayThread(seconds*1000*1000);
        return 0;
    }

    int usleep(useconds_t usec)
    {
        sceKernelDelayThread(usec);
        return 0;
    }

    void __sinit(struct _reent *);
}

#endif


__attribute__((constructor(101)))
void pthread_setup(void)
{
    pthread_init();
    __sinit(_REENT);
}

/**
 * Main entry point for non-Windows sytems. Windows instead uses its own DLL proxy.
 */
#ifdef _MSC_VER
int NormalisedMain(int argc, char * * argv)
#else
int main(int argc, char * * argv)
#endif
{
    core_init();
#if !defined(__vita__)
    int runGame = cmdline_run((const char * *)argv, argc);
    if (runGame == 1)
    {
        if (gOpenRCT2Headless)
        {
            // Run OpenRCT2 with a plain context
            auto context = CreateContext();
            context->RunOpenRCT2(argc, argv);
            delete context;
        }
        else
        {
#endif
            // Run OpenRCT2 with a UI context
            auto env = CreatePlatformEnvironment();
            auto audioContext = CreateAudioContext();
            auto uiContext = CreateUiContext(env);
            auto context = CreateContext(env, audioContext, uiContext);
            context->RunOpenRCT2(argc, argv);

            delete context;
            delete uiContext;
            delete audioContext;
#if !defined(__vita__)
        }
    }
#endif
    return gExitCode;
}


#ifdef __ANDROID__
extern "C" {
int SDL_main(int argc, char *argv[])
{
    return main(argc, argv);
}
}
#endif
