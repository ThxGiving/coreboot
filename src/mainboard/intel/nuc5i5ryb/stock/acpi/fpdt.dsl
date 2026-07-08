/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Disassembly of fpdt.dat
 *
 * ACPI Data Table [FPDT]
 *
 * Format: [HexOffset DecimalOffset ByteLength]  FieldName : FieldValue (in hex)
 */

[000h 0000 004h]                   Signature : "FPDT"    [Firmware Performance Data Table]
[004h 0004 004h]                Table Length : 00000044
[008h 0008 001h]                    Revision : 01
[009h 0009 001h]                    Checksum : F6
[00Ah 0010 006h]                      Oem ID : "INTEL"
[010h 0016 008h]                Oem Table ID : "NUC5i5RY"
[018h 0024 004h]                Oem Revision : 00000182
[01Ch 0028 004h]             Asl Compiler ID : "AMI "
[020h 0032 004h]       Asl Compiler Revision : 00010013


[024h 0036 002h]               Subtable Type : 0001
[026h 0038 001h]                      Length : 10
[027h 0039 001h]                    Revision : 01
[028h 0040 004h]                    Reserved : 00000000
[02Ch 0044 008h]         S3PT Record Address : 00000000A2EE3330

[034h 0052 002h]               Subtable Type : 0000
[036h 0054 001h]                      Length : 10
[037h 0055 001h]                    Revision : 01
[038h 0056 004h]                    Reserved : 00000000
[03Ch 0060 008h]    FPDT Boot Record Address : 00000000A2EE3350

Raw Table Data: Length 68 (0x44)

    0000: 46 50 44 54 44 00 00 00 01 F6 49 4E 54 45 4C 00  // FPDTD.....INTEL.
    0010: 4E 55 43 35 69 35 52 59 82 01 00 00 41 4D 49 20  // NUC5i5RY....AMI 
    0020: 13 00 01 00 01 00 10 01 00 00 00 00 30 33 EE A2  // ............03..
    0030: 00 00 00 00 00 00 10 01 00 00 00 00 50 33 EE A2  // ............P3..
    0040: 00 00 00 00                                      // ....
