/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Disassembling to symbolic ASL+ operators
 *
 * Disassembly of ssdt1.dat
 *
 * Original Table Header:
 *     Signature        "SSDT"
 *     Length           0x00000495 (1173)
 *     Revision         0x01
 *     Checksum         0xD3
 *     OEM ID           "INTEL"
 *     OEM Table ID     "NUC5i5RY"
 *     OEM Revision     0x00000182 (386)
 *     Compiler ID      "INTL"
 *     Compiler Version 0x20120913 (538052883)
 */
DefinitionBlock ("", "SSDT", 1, "INTEL", "NUC5i5RY", 0x00000182)
{
    External (_SB_.PCI0.SAT0, DeviceObj)
    External (_SB_.PCI0.SAT1, DeviceObj)
    External (DSSP, UnknownObj)
    External (FHPP, UnknownObj)

    Scope (\)
    {
        Name (STFE, Buffer (0x07)
        {
             0x10, 0x06, 0x00, 0x00, 0x00, 0x00, 0xEF         // .......
        })
        Name (STFD, Buffer (0x07)
        {
             0x90, 0x06, 0x00, 0x00, 0x00, 0x00, 0xEF         // .......
        })
        Name (FZTF, Buffer (0x07)
        {
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF5         // .......
        })
        Name (DCFL, Buffer (0x07)
        {
             0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB1         // .......
        })
        Name (SCBF, Buffer (0x15){})
        Name (CMDC, Zero)
        Method (GTFB, 2, Serialized)
        {
            Local0 = (CMDC * 0x38)
            CreateField (SCBF, Local0, 0x38, CMDX)
            Local0 = (CMDC * 0x07)
            CreateByteField (SCBF, (Local0 + One), A001)
            CMDX = Arg0
            A001 = Arg1
            CMDC++
        }
    }

    Scope (\_SB.PCI0.SAT0)
    {
        Name (REGF, One)
        Method (_REG, 2, NotSerialized)  // _REG: Region Availability
        {
            If ((Arg0 == 0x02))
            {
                REGF = Arg1
            }
        }

        Name (TMD0, Buffer (0x14){})
        CreateDWordField (TMD0, Zero, PIO0)
        CreateDWordField (TMD0, 0x04, DMA0)
        CreateDWordField (TMD0, 0x08, PIO1)
        CreateDWordField (TMD0, 0x0C, DMA1)
        CreateDWordField (TMD0, 0x10, CHNF)
        Device (CHN0)
        {
            Name (_ADR, Zero)  // _ADR: Address
            Method (_GTM, 0, NotSerialized)  // _GTM: Get Timing Mode
            {
                PIO0 = 0x78
                DMA0 = 0x14
                PIO1 = 0x78
                DMA1 = 0x14
                CHNF = 0x05
                Return (TMD0) /* \_SB_.PCI0.SAT0.TMD0 */
            }

            Method (_STM, 3, NotSerialized)  // _STM: Set Timing Mode
            {
            }

            Device (DRV0)
            {
                Name (_ADR, Zero)  // _ADR: Address
                Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                {
                    CMDC = Zero
                    If ((DSSP || FHPP))
                    {
                        GTFB (STFD, 0x06)
                    }
                    Else
                    {
                        GTFB (STFE, 0x06)
                    }

                    GTFB (DCFL, Zero)
                    GTFB (FZTF, Zero)
                    Return (SCBF) /* \SCBF */
                }
            }

            Device (DRV1)
            {
                Name (_ADR, One)  // _ADR: Address
                Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                {
                    CMDC = Zero
                    If ((DSSP || FHPP))
                    {
                        GTFB (STFD, 0x06)
                    }
                    Else
                    {
                        GTFB (STFE, 0x06)
                    }

                    GTFB (DCFL, Zero)
                    GTFB (FZTF, Zero)
                    Return (SCBF) /* \SCBF */
                }
            }
        }

        Device (CHN1)
        {
            Name (_ADR, One)  // _ADR: Address
            Method (_GTM, 0, NotSerialized)  // _GTM: Get Timing Mode
            {
                PIO0 = 0x78
                DMA0 = 0x14
                PIO1 = 0x78
                DMA1 = 0x14
                CHNF = 0x05
                Return (TMD0) /* \_SB_.PCI0.SAT0.TMD0 */
            }

            Method (_STM, 3, NotSerialized)  // _STM: Set Timing Mode
            {
            }

            Device (DRV0)
            {
                Name (_ADR, Zero)  // _ADR: Address
                Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                {
                    CMDC = Zero
                    If ((DSSP || FHPP))
                    {
                        GTFB (STFD, 0x06)
                    }
                    Else
                    {
                        GTFB (STFE, 0x06)
                    }

                    GTFB (DCFL, Zero)
                    GTFB (FZTF, Zero)
                    Return (SCBF) /* \SCBF */
                }
            }

            Device (DRV1)
            {
                Name (_ADR, One)  // _ADR: Address
                Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                {
                    CMDC = Zero
                    If ((DSSP || FHPP))
                    {
                        GTFB (STFD, 0x06)
                    }
                    Else
                    {
                        GTFB (STFE, 0x06)
                    }

                    GTFB (DCFL, Zero)
                    GTFB (FZTF, Zero)
                    Return (SCBF) /* \SCBF */
                }
            }
        }
    }

    Scope (\_SB.PCI0.SAT1)
    {
        Name (REGF, One)
        Method (_REG, 2, NotSerialized)  // _REG: Region Availability
        {
            If ((Arg0 == 0x02))
            {
                REGF = Arg1
            }
        }

        Name (TMD0, Buffer (0x14){})
        CreateDWordField (TMD0, Zero, PIO0)
        CreateDWordField (TMD0, 0x04, DMA0)
        CreateDWordField (TMD0, 0x08, PIO1)
        CreateDWordField (TMD0, 0x0C, DMA1)
        CreateDWordField (TMD0, 0x10, CHNF)
        Device (CHN0)
        {
            Name (_ADR, Zero)  // _ADR: Address
            Method (_GTM, 0, NotSerialized)  // _GTM: Get Timing Mode
            {
                PIO0 = 0x78
                DMA0 = 0x14
                PIO1 = 0x78
                DMA1 = 0x14
                CHNF = One
                Return (TMD0) /* \_SB_.PCI0.SAT1.TMD0 */
            }

            Method (_STM, 3, NotSerialized)  // _STM: Set Timing Mode
            {
            }

            Device (DRV0)
            {
                Name (_ADR, Zero)  // _ADR: Address
                Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                {
                    CMDC = Zero
                    If ((DSSP || FHPP))
                    {
                        GTFB (STFD, 0x06)
                    }
                    Else
                    {
                        GTFB (STFE, 0x06)
                    }

                    GTFB (DCFL, Zero)
                    GTFB (FZTF, Zero)
                    Return (SCBF) /* \SCBF */
                }
            }
        }

        Device (CHN1)
        {
            Name (_ADR, One)  // _ADR: Address
            Method (_GTM, 0, NotSerialized)  // _GTM: Get Timing Mode
            {
                PIO0 = 0x78
                DMA0 = 0x14
                PIO1 = 0x78
                DMA1 = 0x14
                CHNF = One
                Return (TMD0) /* \_SB_.PCI0.SAT1.TMD0 */
            }

            Method (_STM, 3, NotSerialized)  // _STM: Set Timing Mode
            {
            }

            Device (DRV0)
            {
                Name (_ADR, One)  // _ADR: Address
                Method (_GTF, 0, NotSerialized)  // _GTF: Get Task File
                {
                    CMDC = Zero
                    If ((DSSP || FHPP))
                    {
                        GTFB (STFD, 0x06)
                    }
                    Else
                    {
                        GTFB (STFE, 0x06)
                    }

                    GTFB (DCFL, Zero)
                    GTFB (FZTF, Zero)
                    Return (SCBF) /* \SCBF */
                }
            }
        }
    }
}

