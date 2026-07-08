# NUC5i5RYB coreboot port — WIP scaffold

Copied from `hp/elitebook_820_g2` (same Broadwell-U + Wildcat Point-LP PCH `8086:9cc3`),
HP EC removed. **SKELETON — does NOT build/boot yet.** The technical files still carry
elitebook data and must be replaced with the NUC's real data in `stock/`.

## Confirmed HW (stock/)
- i5-5250U, CPUID `0x306d4` (Broadwell-U ULT); NB `8086:1604`; PCH `8086:9cc3` (Wildcat Point-LP Premium)
- SPI: 8 MB, SOIC-8, locked (ME active) -> **external flashing required**
- EC-less; HW-monitor/fan = **Nuvoton NCT5577D SuperIO** (NCT6776-compatible) @ 0x4e, HWM LDN 0x0b @ 0xa00 -> wired in devicetree (chip superio/nuvoton/nct6776). Linux: nct6775 binds with acpi_enforce_resources=lax -> fan RPM/PWM + board temps + voltages + Vbat
- TPM = Intel PTT (fTPM via ME), CRB @ 0xa2fff000 (stock/acpi/tpm2.dsl). Stock BIOS's
  **missing RMRR for device 00:16.7** is the VT-d bug we can fix here (stock/acpi/dmar.dsl)
- Thermal: TZ00=CPU DTS; PLUS board temps/voltages/fan-RPM/PWM via the NCT5577D SuperIO (see above)

## TODO (replace elitebook data with NUC's)
- [x] gpio.c            <- DONE (first pass): 95 LP pads decoded from inteltool. Owner/route/reset TODO
- [x] devicetree.cb     <- DONE: devices on/off from lspci, gen_dec via setpci, ec_present=false, SATA off (stock), M.2 slots. USB OC-map TODO
- [x] pei_data.c        <- DONE (first pass): SPD 0x50/0x52, USB all-on/OC-skip. USB OC routing TODO
- [x] hda_verb.c        <- DONE: Realtek ALC283 pin configs from live codec dump
- [x] data.vbt          <- DONE: real NUC VBT pulled from live i915 debugfs (6144B, $VBT). stock/vbt.bin
- [x] gma-mainboard.ads <- DONE: HDMI + mini-DP ports, no eDP
- [x] dsdt.asl/acpi/    <- DONE (first pass): stripped HP EC0 from platform.asl; standard wildcatpoint includes
- [~] TPM              <- NOT feasible: coreboot Broadwell has no CRB/PTT support (all BDW boards use memory-mapped TPM1). fTPM stays out; VMs use swtpm. Would need CRB support added first.
- [ ] fan curve       <- optional: program NCT6776 SmartFan curve in coreboot (like BKHD it87). HWM is wired; control is a follow-up.
- [ ] flash             <- keep vendor IFD descriptor + ME region; replace BIOS region only (external)

## Reference boards
hp/elitebook_820_g2 (template), purism/librem_bdw, google/jecht — all SOC_INTEL_BROADWELL.
