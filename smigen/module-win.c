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

#include <ntddk.h>
#include <stdarg.h>
#include <string.h>

#include "smigen.h"
#include "smigen-ioctl.h"

#define NT_DEVICE_NAME  L"\\Device\\SMIGEN"
#define DOS_DEVICE_NAME L"\\DosDevices\\SmiGen"

DRIVER_INITIALIZE DriverEntry;

DRIVER_DISPATCH SmiGenCreateClose;
DRIVER_DISPATCH SmiGenDeviceControl;

DRIVER_UNLOAD SmiGenUnloadDriver;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#endif /* ALLOC_PRAGMA */

NTSTATUS
DriverEntry(__in PDRIVER_OBJECT DriverObject,
            __in PUNICODE_STRING RegistryPath) {
    NTSTATUS ntStatus;
    UNICODE_STRING ntUnicodeString;
    UNICODE_STRING ntWin32NameString;
    PDEVICE_OBJECT deviceObject = NULL;

    RtlInitUnicodeString(&ntUnicodeString, NT_DEVICE_NAME);

    ntStatus = IoCreateDevice(DriverObject, 0, &ntUnicodeString,
                              FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN,
                              FALSE, &deviceObject);

    if (!NT_SUCCESS(ntStatus)) {
        smigen_printk("Couldn't create the device object\n");
        return ntStatus;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = SmiGenCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = SmiGenCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = SmiGenDeviceControl;
    DriverObject->DriverUnload = SmiGenUnloadDriver;

    RtlInitUnicodeString(&ntWin32NameString, DOS_DEVICE_NAME);
    ntStatus = IoCreateSymbolicLink(&ntWin32NameString, &ntUnicodeString);

    if (!NT_SUCCESS(ntStatus)) {
        smigen_printk("Couldn't create symbolic link\n");
        IoDeleteDevice(deviceObject);
    }

    smigen_printk("Smigen module is loaded successfully\n");
    return ntStatus;
}

NTSTATUS
SmiGenCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

VOID
SmiGenUnloadDriver(__in PDRIVER_OBJECT DriverObject) {
    PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;
    UNICODE_STRING uniWin32NameString;

    RtlInitUnicodeString(&uniWin32NameString, DOS_DEVICE_NAME);
    IoDeleteSymbolicLink(&uniWin32NameString);

    if ( deviceObject != NULL ) {
        IoDeleteDevice(deviceObject);
    }
}

NTSTATUS
SmiGenDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    PIO_STACK_LOCATION  irpSp;
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    ULONG               inBufLength;
    PCHAR               inBuf;
    PMDL                mdl = NULL;
    PCHAR               buffer = NULL;
    uint64              smi_count;

    irpSp = IoGetCurrentIrpStackLocation(Irp);
    inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;

    if (!inBufLength) {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto end;
    }

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode) {
    case SMIGEN_STOP:
        smigen_printk("SMIGEN_STOP: not yet implemented\n");
        break;

    case SMIGEN_START:
        inBuf = irpSp->Parameters.DeviceIoControl.Type3InputBuffer;

        try {
            ProbeForRead(inBuf, inBufLength, sizeof(UCHAR));
        } except(EXCEPTION_EXECUTE_HANDLER) {
            ntStatus = GetExceptionCode();
            smigen_printk("Exception while accessing input buffer 0X%08X in"
                          " SMIGEN_START\n", ntStatus);
            break;
        }

        mdl = IoAllocateMdl(inBuf, inBufLength,  FALSE, TRUE, NULL);
        if (!mdl) {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        try {
            MmProbeAndLockPages(mdl, UserMode, IoReadAccess);
        } except(EXCEPTION_EXECUTE_HANDLER) {
            ntStatus = GetExceptionCode();
            smigen_printk("Exception while locking input buffer 0X%08X in"
                          " SMIGEN_START\n", ntStatus);
            IoFreeMdl(mdl);
            break;
        }

        buffer = MmGetSystemAddressForMdlSafe(mdl, NormalPagePriority );
        if (!buffer) {
                ntStatus = STATUS_INSUFFICIENT_RESOURCES;
                MmUnlockPages(mdl);
                IoFreeMdl(mdl);
                break;
        }

        smigen_printk("SMIGEN_START: %d\n", (int)*buffer);
        smigen_trigger_smi();

        MmUnlockPages(mdl);
        IoFreeMdl(mdl);
        break;

    default:
        smigen_printk("Unrecognised ioctl request %#llx",
                      irpSp->Parameters.DeviceIoControl.IoControlCode);
        ntStatus = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

end:
    Irp->IoStatus.Status = ntStatus;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return ntStatus;
}

int
smigen_printk(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vDbgPrintExWithPrefix("[smigen] ", DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL,
                          fmt, va);
    va_end(va);
    return 0;
}

extern uint64 __rdmsr(unsigned num);

int
smigen_safe_rdmsr(unsigned msr, uint64 *val)
{
    try {
        *val = __rdmsr(msr);
    } except (EXCEPTION_EXECUTE_HANDLER) {
        return -1;
    }

    return 0;
}

void
smigen_port_out(unsigned port, uint32 data)
{
    __outdword((USHORT)port, (ULONG)data);
}
