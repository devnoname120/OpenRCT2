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

#if defined(__vita__)

#include "platform.h"
#include "../config/Config.h"
#include "../localisation/date.h"
#include "../localisation/language.h"
#include "../util/util.h"
#include <wchar.h>
#include <SDL2/SDL.h>
#include <psp2/types.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/rtc.h>

// TODO: make this configurable?
#define VITA_DATA_DIR "ux0:/data/openrct2/"

static utf8 _userDataDirectoryPath[MAX_PATH] = {0};
static utf8 _openrctDataDirectoryPath[MAX_PATH] = {0};

typedef struct {
	bool active;
	char *pattern[MAX_PATH];
	struct SceIoDirent **fileListTemp;
	char **paths;
	sint32 cnt;
	SceUID handle;
} enumerate_file_info;

static enumerate_file_info _enumerateFileInfoList[8] = {0};

void platform_get_date_utc(rct2_date *out_date)
{
	if (out_date == NULL)
		return;

	SceDateTime dt;

	if (sceRtcGetCurrentClock(&dt, 0) < 0)
		return;

	out_date->day = dt.day;
	out_date->month = dt.month;
	out_date->year = dt.year;
	out_date->day_of_week = sceRtcGetDayOfWeek(dt.year, dt.month, dt.day);
}

void platform_get_time_utc(rct2_time *out_time)
{
	if (out_time == NULL)
		return;

	SceDateTime dt;

	if (sceRtcGetCurrentClock(&dt, 0) < 0)
		return;

	out_time->hour = dt.hour;
	out_time->minute = dt.minute;
	out_time->second = dt.second;
}

void platform_get_date_local(rct2_date *out_date)
{
	if (out_date == NULL)
		return;

	SceDateTime dt;

	if (sceRtcGetCurrentClockLocalTime(&dt) < 0)
		return;

	out_date->day = dt.day;
	out_date->month = dt.month;
	out_date->year = dt.year;
	out_date->day_of_week = sceRtcGetDayOfWeek(dt.year, dt.month, dt.day);
}

void platform_get_time_local(rct2_time *out_time)
{
	if (out_time == NULL)
		return;

	SceDateTime dt;

	if (sceRtcGetCurrentClockLocalTime(&dt) < 0)
		return;

	out_time->hour = dt.hour;
	out_time->minute = dt.minute;
	out_time->second = dt.second;
}

datetime64 platform_get_datetime_now_utc()
{
	SceDateTime dt;
	SceUInt64 datetime;

	if (sceRtcGetCurrentClock(&dt, 0) < 0)
		return 0;

	sceRtcGetTime64_t(&dt, &datetime);

	return datetime;
}

sint32 platform_get_drives()
{
	return 0;
}

utf8* platform_get_username()
{
	// Seems fitting
	static char *username = "VITA";
	return username;
}
void platform_sleep(uint32 ms)
{
	sceKernelDelayThread(ms);
}


int has_end_slash(const char *path) {
	return path[strlen(path)-1] == '/';
}


int remove_path(const char *path)
{
	SceUID dfd = sceIoDopen(path);
	if (dfd >= 0) {
		int res = 0;

		do {
			SceIoDirent dir;
			memset(&dir, 0, sizeof(SceIoDirent));

			res = sceIoDread(dfd, &dir);
			if (res > 0) {
				char *new_path = malloc(strlen(path) + strlen(dir.d_name) + 2);
				snprintf(new_path, MAX_PATH, "%s%s%s", path, has_end_slash(path) ? "" : "/", dir.d_name);

				if (SCE_S_ISDIR(dir.d_stat.st_mode)) {
					int ret = remove_path(new_path);
					if (ret <= 0) {
						free(new_path);
						sceIoDclose(dfd);
						return ret;
					}
				} else {
					int ret = sceIoRemove(new_path);
					if (ret < 0) {
						free(new_path);
						sceIoDclose(dfd);
						return ret;
					}
				}

				free(new_path);
			}
		} while (res > 0);

		sceIoDclose(dfd);

		int ret = sceIoRmdir(path);
		if (ret < 0)
			return ret;
	} else {
		int ret = sceIoRemove(path);
		if (ret < 0)
			return ret;
	}

	return 1;
}

bool platform_ensure_directory_exists(const utf8 *path)
{
	// messy, should stat properly
	//debugNetPrintf(1, "%s: %s\n", __FUNCTION__, path);
	int mkdir = sceIoMkdir(path, 0777);

	bool ret;
	// already exists
	if (mkdir == 0x80010011)
		ret = true;
	else
		ret = false;

	//bool ret = mkdir >= 0;
	//debugNetPrintf(1, "%s: %x, %d", __FUNCTION__, mkdir, ret);
	return ret;
}

// What are we supposed to do about overwrites?
bool platform_file_move(const utf8 *srcPath, const utf8 *dstPath)
{
	return sceIoRename(srcPath, dstPath);
}

bool platform_file_copy(const utf8 *srcPath, const utf8 *dstPath, bool overwrite)
{
	SceUID sf = sceIoOpen(srcPath, SCE_O_RDONLY, 0777);
	if (!sf)
		return false;

	SceUID df = sceIoOpen(dstPath, SCE_O_WRONLY|SCE_O_CREAT, 0777);
	if (!df)
		return false;

	char buffer[4096];
	size_t read, written;
	unsigned int offset = 0;
	do {
		read = sceIoPread(sf, buffer, 4096, offset);
		written = sceIoPwrite(df, buffer, read, offset);
		offset += read;
	} while (read > 0 && written > 0);


	sceIoClose(sf);
	sceIoClose(df);

	return true;
}

time_t platform_file_get_modified_time(const utf8 *path)
{
	static time_t ret;
	SceIoStat stat;

	if (sceIoGetstat(path, &stat) < 0)
		return 0;

	sceRtcGetTime_t(&stat.st_mtime, &ret);
	return ret;
}

bool platform_original_game_data_exists(const utf8 *path)
{
	// debugNetPrintf(1, "%s: %s\n", __FUNCTION__, path);
	// always false on vita
	// this doesn't mean the executable, it means thepp
	// data files, I'm just stupid
	char buffer[MAX_PATH];
    char checkPath[MAX_PATH];
    safe_strcpy(checkPath, path, MAX_PATH);
    safe_strcat_path(checkPath, "Data", MAX_PATH);
    safe_strcat_path(checkPath, "g1.dat", MAX_PATH);
    return platform_file_exists(checkPath);
}

bool platform_directory_delete(const utf8 *path)
{
	return remove_path(path);
}

bool platform_file_delete(const utf8 *path)
{
	return remove_path(path);
}

bool platform_directory_exists(const utf8 *path)
{
	SceUID uid = sceIoDopen(path);
	if (!uid)
		return false;

	// Got something, assume we're good and close up
	sceIoDclose(uid);
	return true;
}

bool platform_file_exists(const utf8 *path)
{
	SceIoStat stat;
	int ret = sceIoGetstat(path, &stat);
	// debugNetPrintf(1, "%s, %s, %d", __FUNCTION__, path, ret);
	return ret >= 0;
}

sint32 list_dirs(const char *dir)
{
	// SceUID dir_uid = sceIoDopen(dir);
	// int ret;
	// SceIoDirent tmp;

	// if (dfd < 0)
	// 	return 0;

	// do {
	// 	ret = sceIoDread(dir_uid, &tmp);

	// 	if (tmp.d_stat & S_IFDIR)
	// 	{

	// 	}


	// } while (ret > 0);

}

sint32 platform_enumerate_directories_begin(const utf8 *directory)
{

	// sint32 cnt;

	// enumerate_file_info *enum_file_info;

	// sint32 length = strlen(directory);

	// for (sint32 i = 0; i < countof(_enumerateFileInfoList); i++) {
	// 	enum_file_info = _enumerateFileInfoList[i];

	// 	if (!enumFileInfo->active)
	// 	{
	// 		safe_strcpy(enumFileInfo->pattern, directory, length);

	// 	}
	// }

}

bool platform_enumerate_directories_next(sint32 handle, utf8 *path)
{

}

void platform_enumerate_directories_end(sint32 handle)
{

}

sint32 platform_enumerate_files_begin(const utf8 *pattern)
{
	log_verbose("begin file search: %s", pattern);

	char *filename = strrchr(pattern, *PATH_SEPARATOR);
	char *dirname;

}

bool platform_enumerate_files_next(sint32 handle, file_info *outFileInfo)
{

}

void platform_enumerate_files_end(sint32 handle)
{
	if (handle < 0)
	{
		return;
	}

	sceIoDclose((SceUID)handle);

}

void platform_get_changelog_path(utf8 *outPath, size_t outSize)
{
	safe_strcpy(outPath, VITA_DATA_DIR, outSize);
}

void platform_resolve_user_data_path()
{
    strcpy(_userDataDirectoryPath, VITA_DATA_DIR);
}

void platform_resolve_openrct_data_path()
{
	strcpy(_openrctDataDirectoryPath, VITA_DATA_DIR);
}

void platform_get_exe_path(utf8 *outPath, size_t outSize)
{
    safe_strcpy(outPath, VITA_DATA_DIR, outSize);
}

void platform_get_user_directory(utf8 *outPath, const utf8 *subDirectory, size_t outSize)
{
	safe_strcpy(outPath, VITA_DATA_DIR, outSize);
}

void platform_posix_sub_user_data_path(char *buffer, size_t size, const char *homedir)
{
    safe_strcpy(buffer, VITA_DATA_DIR, size);
}

void platform_get_openrct_data_path(utf8 *outPath, size_t outSize)
{
	// debugNetPrintf(1, "%s, %s\n", __FUNCTION__, _openrctDataDirectoryPath);
	safe_strcpy(outPath, _openrctDataDirectoryPath, outSize);
}

utf8 *platform_get_absolute_path(const utf8 * relative_path, const utf8 * base_path)
{
	utf8 path[MAX_PATH];

	if (base_path != NULL)
	{
		snprintf(path, MAX_PATH, "%s/%s", base_path, relative_path);
	}
	else
	{
		safe_strcpy(path, base_path, MAX_PATH);
	}

	// alloc memory like realpath does on posix
	size_t path_len = strlen(path);
	utf8 *result = malloc(path_len + 1);
	memcpy(result, path, path_len);
	result[path_len] = '\0';

	return result;
}

bool platform_lock_single_instance()
{
	// Not possible on vita
	return true;
}


uint8 platform_get_locale_temperature_format()
{
	// TODO: we can get this based on console language/locale?
	return TEMPERATURE_FORMAT_C;
}

uint8 platform_get_locale_date_format()
{
	// Same as above
	return DATE_FORMAT_DAY_MONTH_YEAR;
}

#ifndef NO_TTF
bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer, size_t size)
{
    STUB();
    return false;
}
#endif

void platform_posix_sub_resolve_openrct_data_path(utf8 *out, size_t size)
{
    safe_strcpy(out, VITA_DATA_DIR, size);
}


uint16 platform_get_locale_language()
{
	// Pull from console
    return LANGUAGE_ENGLISH_UK;
}

uint8 platform_get_locale_currency()
{
    return platform_get_currency_value(NULL);
}

uint8 platform_get_locale_measurement_format()
{
    return MEASUREMENT_FORMAT_METRIC;
}

bool platform_process_is_elevated()
{
	return false;
}

static mode_t getumask()
{
	return 0777;
}

#endif