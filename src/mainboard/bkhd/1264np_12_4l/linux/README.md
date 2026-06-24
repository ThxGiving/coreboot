# bkhd/1264np_12_4l — Linux hardware monitoring (ITE IT8625E)

coreboot programs the IT8625E Environment Controller at boot (via the
`superio/ite/it8625e` `ec` block in `devicetree.cb`): CPU + board temperature
sensors (TMPIN1/2), all voltage rails, and a CPU-fan SmartGuardian curve
(FAN1 = the J2G1 header). To read the values under Linux:

## 1. Driver

The IT8625E is not in the mainline `it87` driver yet, so use the out-of-tree
driver and force the chip ID:

```sh
git clone https://github.com/xdarklight/hwmon-it87 /tmp/it87
cd /tmp/it87
# DKMS so it survives kernel updates (needs the matching kernel headers):
VER=0.0+local
sudo cp -r . /usr/src/it87-$VER
sudo sed -i "s|^PACKAGE_VERSION=.*|PACKAGE_VERSION=\"$VER\"|" /usr/src/it87-$VER/dkms.conf
sudo dkms add -m it87 -v $VER
sudo dkms build -m it87 -v $VER
sudo dkms install -m it87 -v $VER
```

Persist the force_id and autoload (depends on `hwmon-vid`):

```sh
echo 'options it87 force_id=0x8625' | sudo tee /etc/modprobe.d/it87.conf
printf 'hwmon-vid\nit87\n'          | sudo tee /etc/modules-load.d/it87.conf
sudo modprobe it87 force_id=0x8625
```

The chip then appears as `it8625-isa-0a30` (coreboot puts the HWM at I/O 0xa30).

## 2. Labels + calibration

Copy `sensors.d-bkhd.conf` to `/etc/sensors.d/bkhd.conf`, then:

```sh
sudo sensors -s   # apply the chip limits
sensors           # CPU/Board temps, +12V (calibrated), Vcore, VDIMM, 3V3, Vbat
```

`lm-sensors.service` (enable it) re-applies the limits at boot.

## Notes / still-open

- **+12V** is calibrated `x5.994` against a 12.0 V reference. Re-tune the
  `compute in2` factor if your reference differs.
- **in3..in6** (~2 V divided rails) and **5 V** are not yet identified — hidden
  until traced with a multimeter, then add `label`/`compute` lines.
- TMPIN modes are `THERMAL_DIODE` (matches stock). If CPU temp ever looks wrong,
  try `THERMAL_PECI` for TMPIN1 in the coreboot devicetree.
- Only one fan header (J2G1 = FAN1/pwm1); other fan/pwm channels are unrouted.
