# GMKtec NucBox G10 - stock hardware reference

Captures from the running stock Linux on the G10 (AMD Ryzen 5 3500U / Picasso),
used to fill in gpio.c, port_descriptors.c and devicetree.cb during bring-up.

| file | source command |
|------|----------------|
| gpio-debugfs.txt | cat /sys/kernel/debug/gpio |
| gpio-iomux.txt   | FCH IOMUX dump (0xFED80D00) |
| lspci-vvv.txt    | lspci -nnvvv |
| lspci-tree.txt   | lspci -tv |
| lsusb-tree.txt   | lsusb -tv |
| dmi-memory.txt   | dmidecode -t memory |
| dmidecode.txt    | dmidecode |
| acpi/*.dsl       | acpidump -b + iasl -d |
| superiotool.txt  | superiotool -d -V |
| interrupts.txt   | cat /proc/interrupts |
| iomem.txt        | cat /proc/iomem |
| dmesg.txt        | dmesg |
