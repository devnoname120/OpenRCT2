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

#pragma once

#include <string>
#include "../common.h"

extern "C"
{
    #include "../interface/window.h"
}

namespace OpenRCT2
{
    namespace Ui
    {
        /**
         * Manager of in-game windows and widgets.
         */
        interface IWindowManager
        {
            virtual ~IWindowManager() = default;
            virtual rct_window * OpenWindow(rct_windowclass wc) abstract;

            virtual void HandleKeyboard(bool isTitle) abstract;
            virtual std::string GetKeyboardShortcutString(sint32 shortcut) abstract;
        };

        IWindowManager * CreateDummyWindowManager();
    }
}
