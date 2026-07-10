/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Intel PTT firmware-TPM 2.0 via ACPI CRB "Start Method 2" (Broadwell /
 * Wildcat Point-LP), replicating the stock AMI \_SB.TPM. This silicon has no
 * fixed-MMIO CRB at 0xFED40000 (LPC decode for a discrete TIS TPM the board
 * lacks); the ME CRB is at 0xFED70000 (control area + data buffer, see ptt.c).
 * The OS stages a command in the buffer and calls _DSM fn 1 ("Start"), which
 * rings the ME doorbell; the ME runs it and clears Start.
 */

Scope (\_SB)
{
	Device (TPM)
	{
		Name (_HID, "MSFT0101")
		Name (_CID, "MSFT0101")
		Name (_UID, One)

		Method (_STA, 0, NotSerialized)
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite,
				0xFED70000,	// ME command doorbell window
				0x00001000)
		})

		/* CRB control area: ctrl_start (+0x0C) runs the staged command, HCMD
		   (+0x40) is the Intel doorbell, HSTS (+0x44) the status (0x03=ready).
		   The ME disarms the execute path after ~1s idle; writing HCMD=0 first
		   re-arms it, otherwise ctrl_start=1 hangs (RE'd, not in TCG PTP). */
		OperationRegion (CRBD, SystemMemory, 0xFED70000, 0x48)
		Field (CRBD, AnyAcc, NoLock, Preserve)
		{
			Offset (0x0C),
			CSTR,   32,	// ctrl_start
			Offset (0x40),
			HCMD,   32,	// Intel host-command doorbell
			HSTS,   32	// Intel host status
		}

		/* CRB "Start" (_DSM fn 1): re-arm, then execute the staged command. */
		Method (STRT, 3, Serialized)
		{
			If ((ToInteger (Arg1) == One))
			{
				If (((HSTS & 0x03) == 0x03))
				{
					HCMD = Zero
					CSTR = One
				}
			}

			Return (Zero)
		}

		Method (_DSM, 4, Serialized)
		{
			/* TCG "Start Method" CRB interface. */
			If ((Arg0 == ToUUID ("6bbf6cab-5463-4714-b7cd-f0203c0368d4")))
			{
				/* Function 0: supported-function bitmap (fn 0 + fn 1). */
				If ((ToInteger (Arg2) == Zero))
				{
					Return (Buffer (One)
					{
						0x03
					})
				}

				Return (STRT (Arg1, Arg2, Arg3))
			}

			Return (Buffer (One)
			{
				0x00
			})
		}
	}
}
