/*
 * Intel ACPI Component Architecture
 * AML/ASL+ Disassembler version 20250404 (64-bit version)
 * Copyright (c) 2000 - 2025 Intel Corporation
 * 
 * Disassembly of dmar.dat
 *
 * ACPI Data Table [DMAR]
 *
 * Format: [HexOffset DecimalOffset ByteLength]  FieldName : FieldValue (in hex)
 */

[000h 0000 004h]                   Signature : "DMAR"    [DMA Remapping Table]
[004h 0004 004h]                Table Length : 000000D4
[008h 0008 001h]                    Revision : 01
[009h 0009 001h]                    Checksum : 20
[00Ah 0010 006h]                      Oem ID : "INTEL"
[010h 0016 008h]                Oem Table ID : "NUC5i5RY"
[018h 0024 004h]                Oem Revision : 00000182
[01Ch 0028 004h]             Asl Compiler ID : "INTL"
[020h 0032 004h]       Asl Compiler Revision : 00000001

[024h 0036 001h]          Host Address Width : 26
[025h 0037 001h]                       Flags : 03
[026h 0038 00Ah]                    Reserved : 00 00 00 00 00 00 00 00 00 00

[030h 0048 002h]               Subtable Type : 0000 [Hardware Unit Definition]
[032h 0050 002h]                      Length : 0018

[034h 0052 001h]                       Flags : 00
[035h 0053 001h]        Size (decoded below) : 00
                          Size (pages, log2) : 0
[036h 0054 002h]          PCI Segment Number : 0000
[038h 0056 008h]       Register Base Address : 00000000FED90000

[040h 0064 001h]           Device Scope Type : 01 [PCI Endpoint Device]
[041h 0065 001h]                Entry Length : 08
[042h 0066 001h]                       Flags : 00
[043h 0067 001h]                    Reserved : 00
[044h 0068 001h]              Enumeration ID : 00
[045h 0069 001h]              PCI Bus Number : 00

[046h 0070 002h]                    PCI Path : 02,00


[048h 0072 002h]               Subtable Type : 0000 [Hardware Unit Definition]
[04Ah 0074 002h]                      Length : 0028

[04Ch 0076 001h]                       Flags : 01
[04Dh 0077 001h]        Size (decoded below) : 00
                          Size (pages, log2) : 0
[04Eh 0078 002h]          PCI Segment Number : 0000
[050h 0080 008h]       Register Base Address : 00000000FED91000

[058h 0088 001h]           Device Scope Type : 03 [IOAPIC Device]
[059h 0089 001h]                Entry Length : 08
[05Ah 0090 001h]                       Flags : 00
[05Bh 0091 001h]                    Reserved : 00
[05Ch 0092 001h]              Enumeration ID : 02
[05Dh 0093 001h]              PCI Bus Number : F0

[05Eh 0094 002h]                    PCI Path : 1F,00


[060h 0096 001h]           Device Scope Type : 04 [Message-capable HPET Device]
[061h 0097 001h]                Entry Length : 08
[062h 0098 001h]                       Flags : 00
[063h 0099 001h]                    Reserved : 00
[064h 0100 001h]              Enumeration ID : 00
[065h 0101 001h]              PCI Bus Number : F0

[066h 0102 002h]                    PCI Path : 0F,00


[068h 0104 001h]           Device Scope Type : 05 [Namespace Device]
[069h 0105 001h]                Entry Length : 08
[06Ah 0106 001h]                       Flags : 00
[06Bh 0107 001h]                    Reserved : 00
[06Ch 0108 001h]              Enumeration ID : 02
[06Dh 0109 001h]              PCI Bus Number : 00

[06Eh 0110 002h]                    PCI Path : 17,00


[070h 0112 002h]               Subtable Type : 0001 [Reserved Memory Region]
[072h 0114 002h]                      Length : 0028

[074h 0116 002h]                    Reserved : 0000
[076h 0118 002h]          PCI Segment Number : 0000
[078h 0120 008h]                Base Address : 00000000A2EF2000
[080h 0128 008h]         End Address (limit) : 00000000A2F02FFF

[088h 0136 001h]           Device Scope Type : 01 [PCI Endpoint Device]
[089h 0137 001h]                Entry Length : 08
[08Ah 0138 001h]                       Flags : 00
[08Bh 0139 001h]                    Reserved : 00
[08Ch 0140 001h]              Enumeration ID : 00
[08Dh 0141 001h]              PCI Bus Number : 00

[08Eh 0142 002h]                    PCI Path : 1D,00


[090h 0144 001h]           Device Scope Type : 01 [PCI Endpoint Device]
[091h 0145 001h]                Entry Length : 08
[092h 0146 001h]                       Flags : 00
[093h 0147 001h]                    Reserved : 00
[094h 0148 001h]              Enumeration ID : 00
[095h 0149 001h]              PCI Bus Number : 00

[096h 0150 002h]                    PCI Path : 14,00


[098h 0152 002h]               Subtable Type : 0001 [Reserved Memory Region]
[09Ah 0154 002h]                      Length : 0020

[09Ch 0156 002h]                    Reserved : 0000
[09Eh 0158 002h]          PCI Segment Number : 0000
[0A0h 0160 008h]                Base Address : 00000000A3800000
[0A8h 0168 008h]         End Address (limit) : 00000000A7FFFFFF

[0B0h 0176 001h]           Device Scope Type : 01 [PCI Endpoint Device]
[0B1h 0177 001h]                Entry Length : 08
[0B2h 0178 001h]                       Flags : 00
[0B3h 0179 001h]                    Reserved : 00
[0B4h 0180 001h]              Enumeration ID : 00
[0B5h 0181 001h]              PCI Bus Number : 00

[0B6h 0182 002h]                    PCI Path : 02,00


[0B8h 0184 002h]               Subtable Type : 0004 [ACPI Namespace Device Declaration]
[0BAh 0186 002h]                      Length : 001C

[0BCh 0188 003h]                    Reserved : 000000
[0BFh 0191 001h]               Device Number : 02
[0C0h 0192 00Fh]                 Device Name : "\_SB.PCI0.SDHC"

Raw Table Data: Length 212 (0xD4)

    0000: 44 4D 41 52 D4 00 00 00 01 20 49 4E 54 45 4C 00  // DMAR..... INTEL.
    0010: 4E 55 43 35 69 35 52 59 82 01 00 00 49 4E 54 4C  // NUC5i5RY....INTL
    0020: 01 00 00 00 26 03 00 00 00 00 00 00 00 00 00 00  // ....&...........
    0030: 00 00 18 00 00 00 00 00 00 00 D9 FE 00 00 00 00  // ................
    0040: 01 08 00 00 00 00 02 00 00 00 28 00 01 00 00 00  // ..........(.....
    0050: 00 10 D9 FE 00 00 00 00 03 08 00 00 02 F0 1F 00  // ................
    0060: 04 08 00 00 00 F0 0F 00 05 08 00 00 02 00 17 00  // ................
    0070: 01 00 28 00 00 00 00 00 00 20 EF A2 00 00 00 00  // ..(...... ......
    0080: FF 2F F0 A2 00 00 00 00 01 08 00 00 00 00 1D 00  // ./..............
    0090: 01 08 00 00 00 00 14 00 01 00 20 00 00 00 00 00  // .......... .....
    00A0: 00 00 80 A3 00 00 00 00 FF FF FF A7 00 00 00 00  // ................
    00B0: 01 08 00 00 00 00 02 00 04 00 1C 00 00 00 00 02  // ................
    00C0: 5C 5F 53 42 2E 50 43 49 30 2E 53 44 48 43 00 00  // \_SB.PCI0.SDHC..
    00D0: 00 00 00 00                                      // ....
