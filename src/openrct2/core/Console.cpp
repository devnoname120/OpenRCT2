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

extern "C"
{
    #include "../platform/platform.h"
}

#include "Console.hpp"

#ifdef __vita__
namespace Console
{
    void Write(char c)
    {
        debugNetPrintf(1, "%c", c);
    }

    void Write(const utf8 * str)
    {
        debugNetPrintf(1, "%s", stdout);
    }

    void WriteSpace(size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            debugNetPrintf(1, "%c", ' ');
        }
    }

    void WriteFormat(const utf8 * format, ...)
    {
        // let's home this doesn't go over
        char buffer[1024];
        va_list args;

        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);

        debugNetPrintf(1, "%s", buffer);
    }

    void WriteLine()
    {
        debugNetPrintf(1, "\n");
    }

    void WriteLine(const utf8 * format, ...)
    {
        char buffer[1024];
        va_list args;

        va_start(args, format);
        vsprintf(buffer, format, args);
        debugNetPrintf(1, "%s\n", buffer);
        va_end(args);
    }

    namespace Error
    {
        void Write(char c)
        {
            debugNetPrintf(1, "%c", c);
        }

        void Write(const utf8 * str)
        {
            debugNetPrintf(1, "%s", stdout);
        }

        void WriteSpace(size_t count)
        {
            for (size_t i = 0; i < count; i++)
            {
                debugNetPrintf(1, "%c", ' ');
            }
        }

        void WriteFormat(const utf8 * format, ...)
        {
            // let's home this doesn't go over
            char buffer[1024];
            va_list args;

            va_start(args, format);
            vsprintf(buffer, format, args);
            va_end(args);

            debugNetPrintf(1, "%s", buffer);
        }

        void WriteLine()
        {
            debugNetPrintf(1, "\n");
        }

        void WriteLine(const utf8 * format, ...)
        {
            char buffer[1024];
            va_list args;

            va_start(args, format);
            vsprintf(buffer, format, args);
            debugNetPrintf(1, "%s\n", buffer);
            va_end(args);
        }
    }
}
#else
namespace Console
{
    void Write(char c)
    {
        fputc(c, stdout);
    }

    void Write(const utf8 * str)
    {
        fputs(str, stdout);
    }

    void WriteSpace(size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            Write(' ');
        }
    }

    void WriteFormat(const utf8 * format, ...)
    {
        va_list args;

        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
    }

    void WriteLine()
    {
        puts("");
    }

    void WriteLine(const utf8 * format, ...)
    {
        va_list args;

        va_start(args, format);
        vfprintf(stdout, format, args);
        puts("");
        va_end(args);
    }

    namespace Error
    {
        void Write(char c)
        {
            fputc(c, stderr);
        }

        void Write(const utf8 * str)
        {
            fputs(str, stderr);
        }

        void WriteFormat(const utf8 * format, ...)
        {
            va_list args;

            va_start(args, format);
            vfprintf(stderr, format, args);
            va_end(args);
        }

        void WriteLine()
        {
            fputs(PLATFORM_NEWLINE, stderr);
        }

        void WriteLine(const utf8 * format, ...)
        {
            va_list args;
            va_start(args, format);
            WriteLine_VA(format, args);
            va_end(args);
        }

        void WriteLine_VA(const utf8 * format, va_list args)
        {
            vfprintf(stdout, format, args);
            puts("");
        }
    }
}
#endif