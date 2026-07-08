/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Disassembly of mcfg.dat
 *
 * ACPI Data Table [MCFG]
 *
 * Format: [HexOffset DecimalOffset ByteLength]  FieldName : FieldValue (in hex)
 */

[000h 0000 004h]                   Signature : "MCFG"    [Memory Mapped Configuration Table]
[004h 0004 004h]                Table Length : 0000003C
[008h 0008 001h]                    Revision : 01
[009h 0009 001h]                    Checksum : 3B
[00Ah 0010 006h]                      Oem ID : "INTEL"
[010h 0016 008h]                Oem Table ID : "NUC5i5RY"
[018h 0024 004h]                Oem Revision : 00000182
[01Ch 0028 004h]             Asl Compiler ID : "MSFT"
[020h 0032 004h]       Asl Compiler Revision : 00000097

[024h 0036 008h]                    Reserved : 0000000000000000

[02Ch 0044 008h]                Base Address : 00000000F8000000
[034h 0052 002h]        Segment Group Number : 0000
[036h 0054 001h]            Start Bus Number : 00
[037h 0055 001h]              End Bus Number : 3F
[038h 0056 004h]                    Reserved : 00000000

Raw Table Data: Length 60 (0x3C)

    0000: 4D 43 46 47 3C 00 00 00 01 3B 49 4E 54 45 4C 00  // MCFG<....;INTEL.
    0010: 4E 55 43 35 69 35 52 59 82 01 00 00 4D 53 46 54  // NUC5i5RY....MSFT
    0020: 97 00 00 00 00 00 00 00 00 00 00 00 00 00 00 F8  // ................
    0030: 00 00 00 00 00 00 00 3F 00 00 00 00              // .......?....
