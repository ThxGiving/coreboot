/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Disassembly of tpm2.dat
 *
 * ACPI Data Table [TPM2]
 *
 * Format: [HexOffset DecimalOffset ByteLength]  FieldName : FieldValue (in hex)
 */

[000h 0000 004h]                   Signature : "TPM2"    [Trusted Platform Module hardware interface Table]
[004h 0004 004h]                Table Length : 00000034
[008h 0008 001h]                    Revision : 03
[009h 0009 001h]                    Checksum : B9
[00Ah 0010 006h]                      Oem ID : "INTEL"
[010h 0016 008h]                Oem Table ID : "NUC5i5RY"
[018h 0024 004h]                Oem Revision : 00000182
[01Ch 0028 004h]             Asl Compiler ID : "AMI "
[020h 0032 004h]       Asl Compiler Revision : 00000000

[024h 0036 004h]                    Reserved : 00000000
[028h 0040 008h]             Control Address : 00000000A2FFF000
[030h 0048 004h]                Start Method : 00000002
/**** ACPI table terminates in the middle of a data structure! (dump table)
CurrentOffset: 34, TableLength: 34 ***/
Raw Table Data: Length 52 (0x34)

    0000: 54 50 4D 32 34 00 00 00 03 B9 49 4E 54 45 4C 00  // TPM24.....INTEL.
    0010: 4E 55 43 35 69 35 52 59 82 01 00 00 41 4D 49 20  // NUC5i5RY....AMI 
    0020: 00 00 00 00 00 00 00 00 00 F0 FF A2 00 00 00 00  // ................
    0030: 02 00 00 00                                      // ....
