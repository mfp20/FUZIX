# FuzixOS: Because Small Is Beautiful, and Pico Is Realtime!

This is a fork of [David Given's FUZIX Raspberry Pi Pico port](https://github.com/davidgiven/FUZIX), which in turn is a fork of [Alan Cox's Fuzix](https://github.com/EtchedPixels/FUZIX).
Look at those repositories for the README.md. Raspberry Pi Pico specific README and HOWTO is in [its own code directory](https://github.com/mfp20/FUZIX/tree/rpipico/Kernel/platform-rpipico_rt).

This fork adds a realtime thin layer to the original port in order to have all time-sensitive basic peripherals (ie: flash and usb) running on core0 and have core1 available for realtime user apps.

It adds:
* Power and cpu clock management
* Multiple USB interfaces ready to use
* Seamless handover between uart0 and USB on USB connect/disconnect
* Log macros with levels and colors
* ...

Currently the "thin" layer is pretty fat. No optimization or profiling has been done, as well as no DMA leveraging, but system boots on virtual devices and looks snappy.
