/** @file UnlockRollback.c

  UEFI Application that calls QCOM_VERIFIEDBOOT_PROTOCOL.VBRwDeviceState
  with op=WRITE_CONFIG (1), 16-byte buffer: byte0=1, byte8=1.

  Copyright (c) 2026. All rights reserved.

**/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DeviceInfo.h>
#include <Protocol/EFIVerifiedBoot.h>

#define VB_RW_BUF_SIZE  3344
#define PrintA AsciiPrint

/**
  Entry point: Locate QCOM_VERIFIEDBOOT_PROTOCOL and call VBRwDeviceState
  with op=WRITE_CONFIG (1), 16-byte buffer with buf[0]=1, buf[8]=1.

  @param[in] ImageHandle    Image handle for this application.
  @param[in] SystemTable    Pointer to the EFI System Table.

  @retval EFI_SUCCESS      Call succeeded.
  @retval other            LocateProtocol or VBRwDeviceState failed.
**/
EFI_STATUS
EFIAPI
UefiMain (
	IN EFI_HANDLE        ImageHandle,
	IN EFI_SYSTEM_TABLE  *SystemTable
	)
{
	EFI_STATUS                    Status;
	QCOM_VERIFIEDBOOT_PROTOCOL   *VbProtocol;
	UINT8                         Buf[VB_RW_BUF_SIZE];
	UINT32                        BufLen = VB_RW_BUF_SIZE;
	// EFI_GUID gEfiQcomVerifiedBootProtocolGuid1 =       { 0x8e5eff91, 0x21b6, 0x47d3, { 0xaf, 0x2b, 0xc1, 0x5a, 0x1, 0xe0, 0x20, 0xec } };

	// Locate QCOM Verified Boot Protocol
	Status = gBS->LocateProtocol (
			&gEfiQcomVerifiedBootProtocolGuid,
			NULL,
			(VOID **)&VbProtocol);
	if (EFI_ERROR (Status)) {
		PrintA ("UR: VerifiedBoot failed: %r\n", Status);
		return Status;
	}
	Status = VbProtocol->VBRwDeviceState (
			VbProtocol,
			READ_CONFIG,
			Buf,
			BufLen);
	if (EFI_ERROR (Status)) {
		PrintA ("UR: READ_CONFIG failed: %r\n", Status);
		return Status;
	}

	// unlock
	Buf[13] = 1;
	Buf[14] = 1;

	// read rollback
	for (UINT32 i = 0; i < 10; i++ ) {
		PrintA ("UR: rollback[%d]: %ld,\n", i, *((UINT64 *)&Buf[2200 + i * sizeof(UINT64)]));
	}

	// clear rollback
	for (UINT32 i = 0; i < MAX_VB_PARTITIONS; i++) {
		*(UINT64 *)&Buf[2200 + i * sizeof(UINT64)] = 0;
	}

	Status = VbProtocol->VBRwDeviceState (
			VbProtocol,
			WRITE_CONFIG,
			Buf,
			BufLen);
	if (EFI_ERROR (Status)) {
		PrintA ("UR: WRITE_CONFIG failed: %r\n", Status);
		return Status;
	}
	PrintA ("UR: WRITE_CONFIG success .\n");

	// sleep 5s
	gBS->Stall (5 * 1000 * 1000);

  return EFI_SUCCESS;
}
