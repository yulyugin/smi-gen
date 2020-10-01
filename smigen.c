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

#include "smigen-ioctl.h"

#ifdef __linux__
typedef int file_t;

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#else /* !__linux__ */
#include <windows.h>
#include <winioctl.h>

typedef HANDLE file_t;
#endif /* !__linux__ */

static void
smigen_perror(const char *s)
{
#ifdef __linux__
    perror(s);
#else /* !__linux__ */
    LPTSTR error_string = NULL;
    DWORD ret;
    ret = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&error_string, 0, NULL);
    if (ret == 0 || error_string == NULL)
        return;

    printf("%s: %s", s, error_string);
    LocalFree((HLOCAL)error_string);
#endif /* !__linux__ */
}

static int
smigen_open(file_t *fd)
{
#ifdef __linux__
    if ((*fd = open("/dev/smigen", O_RDWR)) < 0) {
#else /* !__linux__ */
    if ((*fd = CreateFile("\\\\.\\SmiGen", GENERIC_READ | GENERIC_WRITE, 0,
                    NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE) {
#endif
        return -1;
    }
    return 0;
}

static void
smigen_close(file_t fd)
{
#ifdef __linux__
    close(fd);
#else /* !__linux__ */
    CloseHandle(fd);
#endif /* !__linux__ */
}

static int
smigen_ioctl(file_t fd)
{
#ifdef __linux__
    return ioctl(fd, SMIGEN_TRIGGER_SMI);
#else /* !__linux__ */
    DWORD bytes;
    if (!DeviceIoControl(fd, SMIGEN_TRIGGER_SMI, NULL, 0, NULL, 0, &bytes, NULL))
        return -1;
    return 0;
#endif /* !__linux__ */
}

int
main(int argc, char **argv)
{
    file_t fd;
    if (smigen_open(&fd) < 0) {
        smigen_perror("open");
        return 0;
    }

    if (smigen_ioctl(fd))
        smigen_perror("ioctl");

    smigen_close(fd);
    return 0;
}
