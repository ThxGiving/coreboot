# NUC5i5RYB coreboot port — WIP scaffold

Copied from `hp/elitebook_820_g2` (same Broadwell-U + Wildcat Point-LP PCH `8086:9cc3`),
HP EC removed. **SKELETON — does NOT build/boot yet.** The technical files still carry
elitebook data and must be replaced with the NUC's real data in `stock/`.

## Confirmed HW (stock/)
- i5-5250U, CPUID `0x306d4` (Broadwell-U ULT); NB `8086:1604`; PCH `8086:9cc3` (Wildcat Point-LP Premium)
- SPI: 8 MB, SOIC-8, locked (ME active) -> **external flashing required**
- No coreboot-manageable EC (ectool = 0xff), no LPC SuperIO
- TPM = Intel PTT (fTPM via ME), CRB @ 0xa2fff000 (stock/acpi/tpm2.dsl). Stock BIOS's
  **missing RMRR for device 00:16.7** is the VT-d bug we can fix here (stock/acpi/dmar.dsl)
- Thermal: single zone TZ00 = CPU DTS (stock/acpi/ssdt2.dsl); no board temp / voltages

## TODO (replace elitebook data with NUC's)
- [x] gpio.c            <- DONE (first pass): 95 LP pads decoded from inteltool. Owner/route/reset TODO
- [x] devicetree.cb     <- DONE: devices on/off from lspci, gen_dec via setpci, ec_present=false, SATA off (stock), M.2 slots. USB OC-map TODO
- [x] pei_data.c        <- DONE (first pass): SPD 0x50/0x52, USB all-on/OC-skip. USB OC routing TODO
- [ ] hda_verb.c        <- NUC audio codec verbs (elitebook's are wrong)
- [x] data.vbt          <- DONE: real NUC VBT pulled from live i915 debugfs (6144B, $VBT). stock/vbt.bin
- [ ] gma-mainboard.ads <- display ports (NUC5: HDMI + mini-DP)
- [ ] dsdt.asl/acpi/    <- reference stock/acpi/dsdt.dsl + ssdt*.dsl
- [ ] Kconfig           <- add TPM2/PTT once booting; emit correct RMRR for 00:16.7 -> fTPM works under VT-d
- [ ] flash             <- keep vendor IFD descriptor + ME region; replace BIOS region only (external)

## Reference boards
hp/elitebook_820_g2 (template), purism/librem_bdw, google/jecht — all SOC_INTEL_BROADWELL.
