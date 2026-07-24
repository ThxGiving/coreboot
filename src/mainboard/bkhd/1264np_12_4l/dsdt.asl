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
	 * CNVi Bluetooth (AX211) reset: the BT reaches the host as USB2 port 10
	 * (HS10) with no PCI function and no reset GPIO. is_intel_bluetooth on
	 * usb2_port10 (see devicetree.cb) makes drivers/usb/acpi emit the reset
	 * _DSM + _PRR/_RST (CNVi sideband PLDR) into the SSDT, so no hand-rolled
	 * ACPI is needed here. (CB:93839)
	 *
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
