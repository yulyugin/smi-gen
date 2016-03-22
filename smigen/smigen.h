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

#ifndef __SIMGEN_H__
#define __SMIGEN_H__

#define SMIGEN_START    CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_NEITHER, FILE_WRITE_ACCESS)
#define SMIGEN_STOP     CTL_CODE(FILE_DEVICE_UNKNOWN, 2, METHOD_NEITHER, 0)

typedef unsigned __int64 uint64;

#define MSR_SMI_COUNT   0x34

int smigen_printk(const char *fmt, ...);

int smigen_safe_rdmsr(int msr, uint64 *val);

#endif /* __SMIGEN_H__ */
