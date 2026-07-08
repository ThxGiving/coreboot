/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Disassembly of apic.dat
 *
 * ACPI Data Table [APIC]
 *
 * Format: [HexOffset DecimalOffset ByteLength]  FieldName : FieldValue (in hex)
 */

[000h 0000 004h]                   Signature : "APIC"    [Multiple APIC Description Table (MADT)]
[004h 0004 004h]                Table Length : 00000084
[008h 0008 001h]                    Revision : 03
[009h 0009 001h]                    Checksum : A3
[00Ah 0010 006h]                      Oem ID : "INTEL"
[010h 0016 008h]                Oem Table ID : "NUC5i5RY"
[018h 0024 004h]                Oem Revision : 00000182
[01Ch 0028 004h]             Asl Compiler ID : "AMI "
[020h 0032 004h]       Asl Compiler Revision : 00010013

[024h 0036 004h]          Local Apic Address : FEE00000
[028h 0040 004h]       Flags (decoded below) : 00000001
                         PC-AT Compatibility : 1

[02Ch 0044 001h]               Subtable Type : 00 [Processor Local APIC]
[02Dh 0045 001h]                      Length : 08
[02Eh 0046 001h]                Processor ID : 01
[02Fh 0047 001h]               Local Apic ID : 00
[030h 0048 004h]       Flags (decoded below) : 00000001
                           Processor Enabled : 1
                      Runtime Online Capable : 0

[034h 0052 001h]               Subtable Type : 04 [Local APIC NMI]
[035h 0053 001h]                      Length : 06
[036h 0054 001h]                Processor ID : 01
[037h 0055 002h]       Flags (decoded below) : 0000
                                    Polarity : 0
                                Trigger Mode : 0
[039h 0057 001h]        Interrupt Input LINT : 00

[03Ah 0058 001h]               Subtable Type : 00 [Processor Local APIC]
[03Bh 0059 001h]                      Length : 08
[03Ch 0060 001h]                Processor ID : 02
[03Dh 0061 001h]               Local Apic ID : 02
[03Eh 0062 004h]       Flags (decoded below) : 00000001
                           Processor Enabled : 1
                      Runtime Online Capable : 0

[042h 0066 001h]               Subtable Type : 04 [Local APIC NMI]
[043h 0067 001h]                      Length : 06
[044h 0068 001h]                Processor ID : 02
[045h 0069 002h]       Flags (decoded below) : 0000
                                    Polarity : 0
                                Trigger Mode : 0
[047h 0071 001h]        Interrupt Input LINT : 00

[048h 0072 001h]               Subtable Type : 00 [Processor Local APIC]
[049h 0073 001h]                      Length : 08
[04Ah 0074 001h]                Processor ID : 03
[04Bh 0075 001h]               Local Apic ID : 01
[04Ch 0076 004h]       Flags (decoded below) : 00000001
                           Processor Enabled : 1
                      Runtime Online Capable : 0

[050h 0080 001h]               Subtable Type : 04 [Local APIC NMI]
[051h 0081 001h]                      Length : 06
[052h 0082 001h]                Processor ID : 03
[053h 0083 002h]       Flags (decoded below) : 0000
                                    Polarity : 0
                                Trigger Mode : 0
[055h 0085 001h]        Interrupt Input LINT : 00

[056h 0086 001h]               Subtable Type : 00 [Processor Local APIC]
[057h 0087 001h]                      Length : 08
[058h 0088 001h]                Processor ID : 04
[059h 0089 001h]               Local Apic ID : 03
[05Ah 0090 004h]       Flags (decoded below) : 00000001
                           Processor Enabled : 1
                      Runtime Online Capable : 0

[05Eh 0094 001h]               Subtable Type : 04 [Local APIC NMI]
[05Fh 0095 001h]                      Length : 06
[060h 0096 001h]                Processor ID : 04
[061h 0097 002h]       Flags (decoded below) : 0000
                                    Polarity : 0
                                Trigger Mode : 0
[063h 0099 001h]        Interrupt Input LINT : 00

[064h 0100 001h]               Subtable Type : 01 [I/O APIC]
[065h 0101 001h]                      Length : 0C
[066h 0102 001h]                 I/O Apic ID : 02
[067h 0103 001h]                    Reserved : 00
[068h 0104 004h]                     Address : FEC00000
[06Ch 0108 004h]                   Interrupt : 00000000

[070h 0112 001h]               Subtable Type : 02 [Interrupt Source Override]
[071h 0113 001h]                      Length : 0A
[072h 0114 001h]                         Bus : 00
[073h 0115 001h]                      Source : 00
[074h 0116 004h]                   Interrupt : 00000002
[078h 0120 002h]       Flags (decoded below) : 0000
                                    Polarity : 0
                                Trigger Mode : 0

[07Ah 0122 001h]               Subtable Type : 02 [Interrupt Source Override]
[07Bh 0123 001h]                      Length : 0A
[07Ch 0124 001h]                         Bus : 00
[07Dh 0125 001h]                      Source : 09
[07Eh 0126 004h]                   Interrupt : 00000009
[082h 0130 002h]       Flags (decoded below) : 000D
                                    Polarity : 1
                                Trigger Mode : 3

Raw Table Data: Length 132 (0x84)

    0000: 41 50 49 43 84 00 00 00 03 A3 49 4E 54 45 4C 00  // APIC......INTEL.
    0010: 4E 55 43 35 69 35 52 59 82 01 00 00 41 4D 49 20  // NUC5i5RY....AMI 
    0020: 13 00 01 00 00 00 E0 FE 01 00 00 00 00 08 01 00  // ................
    0030: 01 00 00 00 04 06 01 00 00 00 00 08 02 02 01 00  // ................
    0040: 00 00 04 06 02 00 00 00 00 08 03 01 01 00 00 00  // ................
    0050: 04 06 03 00 00 00 00 08 04 03 01 00 00 00 04 06  // ................
    0060: 04 00 00 00 01 0C 02 00 00 00 C0 FE 00 00 00 00  // ................
    0070: 02 0A 00 00 02 00 00 00 00 00 02 0A 00 09 09 00  // ................
    0080: 00 00 0D 00                                      // ....
