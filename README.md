# FuzixOS: Because Small Is Beautiful, and Pico Is Realtime!

This is a fork of [David Given's FUZIX Raspberry Pi Pico port](https://github.com/davidgiven/FUZIX), which in turn is a fork of [Alan Cox's Fuzix](https://github.com/EtchedPixels/FUZIX).
Look at those repositories for the README.md. Raspberry Pi Pico specific README and HOWTO is in [its own code directory](https://github.com/mfp20/FUZIX/tree/rpipico/Kernel/platform-rp2040_softirq).

This fork adds a realtime thin layer to the original port in order to have all time-sensitive basic peripherals (ie: flash and usb) running on core0 and have core1 available for realtime user apps.

It adds:
* Power and cpu clock management (WiP)
* USB interfaces: 3 CDC ACM class for tty1-3, 1 Vendor class for binary data, extra interfaces for user applications (working)
* Seamless handover between uart0 and USB, ie: uart0 available for user apps (working)
* SD disks for root, swap and scratch (WiP)
* USB disks for root, swap and scratch (WiP)
* Support for /dev/{audio,gpio,kmem,i2c,input,mem,platform,rtc} (WiP)
* Log macros with levels and colors (partially working)
* ...

Currently the "thin" layer is pretty fat. No optimization or profiling has been done, as well as no DMA leveraging, but system boots on virtual devices and looks snappy.

