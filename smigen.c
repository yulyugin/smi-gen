/*
 * Copyright (c) 2016 Evgeny Yulyugin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <windows.h>
#include <winioctl.h>

#include "smigen.h"

static void
smigen_perror(const char *s)
{
    LPTSTR error_string = NULL;
    DWORD ret;
    ret = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&error_string, 0, NULL);
    if (ret == 0 || error_string == NULL)
        return;

    printf("%s: %s", s, error_string);
    LocalFree((HLOCAL)error_string);
}

int
main(int argc, char **argv)
{
    DWORD period = 10;
    HANDLE fd;
    if ((fd = CreateFile("\\\\.\\SmiGen", GENERIC_READ | GENERIC_WRITE, 0, NULL,
                         OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE) {
        smigen_perror("open");
        return 0;
    }

    if (!DeviceIoControl(fd, (DWORD) SMIGEN_START, &period, sizeof(period),
                         NULL, 0, NULL, NULL)) {
            smigen_perror("ioctl");
    }

    if (!DeviceIoControl(fd, (DWORD) SMIGEN_STOP, &period, sizeof(period),
                         NULL, 0, NULL, NULL)) {
            smigen_perror("ioctl");
    }

    CloseHandle(fd);
    return 0;
}