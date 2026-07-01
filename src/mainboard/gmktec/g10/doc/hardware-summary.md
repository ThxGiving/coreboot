# GMKtec NucBox G10 - hardware summary (from stock Linux captures)

- SoC: AMD Ryzen 5 3500U (Picasso/Raven2, 4C/8T, Vega 8), socket FP5
- BIOS: AMI "Nucbox G10 1.04" (2025-05-29)
- RAM: 2x 8 GB DDR4 SO-DIMM, dual-channel, 2400 MT/s configured, non-ECC
- LAN: 1x Realtek RTL8125 2.5GbE [10ec:8125]   -> GPP bridge 00:01.3
- WiFi/BT: Realtek RTL8822CE 802.11ac [10ec:c822] -> GPP bridge 00:01.2
- NVMe: MAXIO MAP1202 (DRAM-less) [1e4b:1202]  -> GPP bridge 00:01.5
- iGPU/USB/audio/PSP behind 00:08.1: Vega [1002:15d8], 2x XHCI, HDA, PSP
- USB: 2x AMD Raven USB 3.1 XHCI (00:04:00.3 / .4)
- EC: ITE IT5570E @ 0x4e (embedded controller: power button / fan; NOT a HWM SIO)
- TPM: TPM2 ACPI (AMI) -> likely AMD fTPM (PSP)
- Sensors: temps via k10temp/amdgpu/nvme/r8169; no board voltage HWM

## GPIO findings (derived from pinctrl-amd + Picasso pinmux, no schematic)
- **EC bus = LPC** (not eSPI): LPC_RST(32), LPCCLK0/1(74/75), SERIRQ(87),
  LAD0-3(104-107), LDRQ0(108), LFRAME(109), LPC_CLKRUN(88), LPC_PD(21).
  => TODO: switch the devicetree/Kconfig from eSPI to LPC for the IT5570E EC.
- Console: UART0_TXD on GPIO_138 (matches uart_0 on).
- Fan: FANIN0(84) tach + FANOUT0(85) PWM  -> SoC fan control present.
- PCIe clk reqs: CLK_REQ0..6 on GPIO 92/115/116/131/132/120/121.
- SPI TPM: SPI_TPM_CS_L on GPIO_29 (+ TPM2 ACPI table) -> possible SPI TPM header.
- USB OC: USB_OC0_L on GPIO_16.  Power button: PWR_BTN_L on GPIO_0.
- I2C: I2C3 on GPIO_19/20; SMBus0 (SCL0/SDA0) on GPIO_113/114.
- Misc: SPI_CS2/3 (30/31), SATA_ACT (130), KBRST (129), SPKR (91), AC_PRES (23).
