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

#ifdef __linux__
typedef unsigned long long uint64;
typedef unsigned int uint32;
#else
typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
#endif

#ifndef MSR_SMI_COUNT
#define MSR_SMI_COUNT       0x34
#endif /* ndef MSR_SMI_COUNT */

/* Software SMI triggered by the system software via an I/O access to a location
 * considered special by the motherboard logic (port 0B2h is common). */
#define PORT_SMI_TRIGGER    0xB2

int smigen_printk(const char *fmt, ...);

int smigen_safe_rdmsr(unsigned msr, uint64 *val);

void smigen_port_out(unsigned port, uint32 data);

static int
smigen_trigger_smi(void)
{
    uint64 smi_count_before, smi_count_after;

    if (smigen_safe_rdmsr(MSR_SMI_COUNT, &smi_count_before)) {
        return -1;
    }

    smigen_port_out(PORT_SMI_TRIGGER, 1);

    if (smigen_safe_rdmsr(MSR_SMI_COUNT, &smi_count_after)) {
        return -1;
    }

    if (smi_count_after <= smi_count_before) {
        smigen_printk("SMI in not triggered. %#llx port is not working",
                      PORT_SMI_TRIGGER);
        return -1;
    }

    smigen_printk("SMI triggered successfully\n");
    return 0;
}

#endif /* __SMIGEN_H__ */
