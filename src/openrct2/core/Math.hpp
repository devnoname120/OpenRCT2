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

#include <algorithm>

/**
 * Common mathematical functions.
 */
namespace Math
{
    template<typename T>
    static T Min(T a, T b)
    {
        return (std::min)(a, b);
    }
    // Add specific templates with casting for vita
#ifdef __vita__
    template<typename T, typename T2>
    static T2 Min(T a, T2 b)
    {
        return (std::min)((T2)a, b);
    }
#endif

    template<typename T>
    static T Max(T a, T b)
    {
        return (std::max)(a, b);
    }
#ifdef __vita__
    template<typename T, typename T2>
    static T2 Max(T a, T2 b)
    {
        return (std::max)((T2)a, b);
    }

#endif

    template<typename T>
    static T Clamp(T low, T x, T high)
    {
        return (std::min)((std::max)(low, x), high);
    }

    // leave these as non-templates, there's 5 possibilites if we
    // want all templates, just leave it as the 3 that break compilation
    // for now
#ifdef __vita__
    static long int Clamp(int low, long int x, int high)
    {
        return (std::min)((std::max)((long int)low, x), (long int)high);
    }

    static long int Clamp(long int low, int x, int high)
    {
        return (std::min)((std::max)(low, (long int)x), (long int)high);
    }

    static long int Clamp(long int low, long int x, int high)
    {
        return (std::min)((std::max)(low, x), (long int)high);
    }
#endif

    template<typename T>
    static T Sign(T x)
    {
        if (x < 0) return -1;
        if (x > 0) return 1;
        return 0;
    }
}
