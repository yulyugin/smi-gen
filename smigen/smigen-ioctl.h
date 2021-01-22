/*
 * Copyright (c) 2016 Evgenii Iuliugin
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

#ifndef __SIMGEN_IOCTL_H__
#define __SMIGEN_IOCTL_H__

#ifdef __linux__
#define SMIGEN_TRIGGER_SMI  _IO('X', 1)
#else /* !__linux__ */
#define SMIGEN_TRIGGER_SMI  CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_NEITHER, 0)
#endif /* !__linux__ */

#endif /* __SMIGEN_IOCTL_H__ */
