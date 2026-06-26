/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	ACPI_DSDT_REV_2,
	OEM_ID,
	ACPI_TABLE_CREATOR,
	0x20110725
)
{
	#include <acpi/dsdt_top.asl>
	#include <soc/intel/common/block/acpi/acpi/platform.asl>
	#include <soc/intel/common/block/acpi/acpi/globalnvs.asl>
	#include <cpu/intel/common/acpi/cpu.asl>

	Device (\_SB.PCI0) {
		#include <soc/intel/common/block/acpi/acpi/northbridge.asl>
		#include <soc/intel/alderlake/acpi/southbridge.asl>
	}

	/*
	 * CNVi Bluetooth reset (_PRR) for the AX211 CNVi-over-USB BT, which the
	 * kernel enumerates on USB2 port 10 = HS10. (The devicetree usb2 array
	 * index [9] is 0-based, i.e. physical port 10. Live-confirmed: the
	 * 8087:0033 BT's ACPI companion is \_SB.PCI0.XHCI.RHUB.HS10.)
	 *
	 * Without a Power-Resource-for-Reset on the BT's ACPI device Linux btintel
	 * logs "Bluetooth: hci0: No support for _PRR ACPI method". coreboot's
	 * soc/intel/common/block/cnvi implements the Intel Platform-Level Device
	 * Reset (PLDR) for exactly this - but only as PCI drivers (cnvw/cnvb) whose
	 * device-ID lists don't include Alder Lake, and the ADL-N BT is
	 * CNVi-over-USB (no BT PCI function), so none of it runs and HS10 gets no
	 * _PRR. Reproduce the PLDR here for the USB BT.
	 *
	 * The reset is triggered through the P2SB sideband CNVi-abort register
	 * (PID_CNVI 0x73, CNVI_ABORT_PLDR 0x80 on non-PCH-S) by setting
	 * ABORT_REQUEST | ABORT_ENABLE (0x03), serialized against the Wi-Fi via the
	 * shared CNMT mutex - identical to cnvi.c's CNVP._RST. A CNVi reset is by
	 * Intel design block-wide (Wi-Fi + BT share one CNVi). PCRR/PCRO are the
	 * SoC's P2SB sideband helpers (soc/.../alderlake), already in the DSDT.
	 *
	 * Live-validated on this board: PCRR(0x73, 0x80) = 0xfd730080 reads 0x04
	 * (CNVI_READY) at idle; the PCH-S offset 0x44 reads 0xffffffff. HS10 is a
	 * fixed xHCI RHUB port (soc/.../alderlake/acpi/xhci.asl), so this scope
	 * stays valid even if the CNVi module is removed - the _PRR just goes
	 * unused (no hci0 -> _RST is never invoked).
	 */
	Scope (\_SB.PCI0)
	{
		/* Shared CNVi (Wi-Fi + BT) reset serialization. Normally emitted by
		   cnvi.c's cnvb_fill_ssdt, which does not run for the USB BT. */
		Mutex (CNMT, 0)
	}

	Scope (\_SB.PCI0.XHCI.RHUB.HS10)
	{
		PowerResource (CBPR, 0x05, 0x0000)
		{
			Method (_STA, 0, NotSerialized) { Return (One) }
			Method (_ON,  0, NotSerialized) { }
			Method (_OFF, 0, NotSerialized) { }
			Method (_RST, 0, NotSerialized)
			{
				If ((Acquire (\_SB.PCI0.CNMT, 0x03E8) == Zero))
				{
					/* Skip if a CNVi abort/reset is already pending
					   (ABORT_REQUEST, bit 1). */
					If (((\_SB.PCI0.PCRR (0x73, 0x80) & 0x02) == Zero))
					{
						/* ABORT_REQUEST | ABORT_ENABLE -> PLDR */
						\_SB.PCI0.PCRO (0x73, 0x80, 0x03)
						Sleep (0x32)   /* 50 ms, cnvi.c BTDL */
					}
					Release (\_SB.PCI0.CNMT)
				}
			}
		}
		Name (_PRR, Package (0x01) { CBPR })
	}

	/*
	 * Wake-on-LAN for the four discrete Intel i226-V NICs (PCH PCIe root ports
	 * 1c.0/1c.3/1c.6/1d.0 = ACPI RP01/RP04/RP07/RP09) is emitted at runtime by
	 * mainboard_fill_ssdt() in mainboard.c, gated per port on the "Wake on LAN
	 * - LANx" CFR options (default on). It adds the standard PCIe-PME _PRW
	 * (GPE0_PME_B0) so a magic packet resumes the box; doing it from the SSDT
	 * lets the CFR toggle drop the wake source per NIC. Live-verified on this
	 * board (2026-06-26): the RPs appear in /proc/acpi/wakeup and a magic packet
	 * resumes from S5. Stock firmware lacked any NIC _PRW - real better-than-stock.
	 */

	#include <southbridge/intel/common/acpi/sleepstates.asl>
}
