/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Endpoint companions for the populated root ports. Without a child node the
 * PCI device has no ACPI handle, so a driver _DSM (e.g. iwlwifi's product-reset
 * GUID on the 7265 at RP04) fails with AE_BAD_PARAMETER instead of the silenced
 * AE_NOT_FOUND, which Linux logs. Mirrors stock AMI. Kept board-local because
 * the shared pcie.asl already carries _ADR=0 companions on some boards.
 */

Scope (\_SB.PCI0.RP01) { Device (PXSX) { Name (_ADR, Zero) } }	/* 1c.0 */
Scope (\_SB.PCI0.RP04) { Device (PXSX) { Name (_ADR, Zero) } }	/* 1c.3 WLAN */
Scope (\_SB.PCI0.RP05) { Device (PXSX) { Name (_ADR, Zero) } }	/* 1c.4 NVMe */
