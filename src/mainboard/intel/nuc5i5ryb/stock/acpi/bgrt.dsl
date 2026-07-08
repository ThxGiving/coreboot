/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Disassembly of bgrt.dat
 *
 * ACPI Data Table [BGRT]
 *
 * Format: [HexOffset DecimalOffset ByteLength]  FieldName : FieldValue (in hex)
 */

[000h 0000 004h]                   Signature : "BGRT"    [Boot Graphics Resource Table]
[004h 0004 004h]                Table Length : 00000038
[008h 0008 001h]                    Revision : 01
[009h 0009 001h]                    Checksum : 27
[00Ah 0010 006h]                      Oem ID : "INTEL"
[010h 0016 008h]                Oem Table ID : "NUC5i5RY"
[018h 0024 004h]                Oem Revision : 00000182
[01Ch 0028 004h]             Asl Compiler ID : "AMI "
[020h 0032 004h]       Asl Compiler Revision : 00010013

[024h 0036 002h]                     Version : 0001
[026h 0038 001h]      Status (decoded below) : 01
                                   Displayed : 1
                          Orientation Offset : 0
[027h 0039 001h]                  Image Type : 00
[028h 0040 008h]               Image Address : 000000009FEC7018
[030h 0048 004h]               Image OffsetX : 000002F3
[034h 0052 004h]               Image OffsetY : 000000F9

Raw Table Data: Length 56 (0x38)

    0000: 42 47 52 54 38 00 00 00 01 27 49 4E 54 45 4C 00  // BGRT8....'INTEL.
    0010: 4E 55 43 35 69 35 52 59 82 01 00 00 41 4D 49 20  // NUC5i5RY....AMI 
    0020: 13 00 01 00 01 00 01 00 18 70 EC 9F 00 00 00 00  // .........p......
    0030: F3 02 00 00 F9 00 00 00                          // ........
