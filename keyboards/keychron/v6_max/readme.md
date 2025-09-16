# Keychron V6 Max

![Keychron V6 Max](https://www.keychron.com/cdn/shop/files/V6-Max-11_2048x.jpg?v=1709002985)

A customizable full-size 100% wireless keyboard.

* Keyboard Maintainer: [Keychron](https://github.com/keychron)
* Hardware Supported: Keychron V6 Max
* Hardware Availability: [Keychron V6 Max QMK/VIA Wireless Custom Mechanical Keyboard](https://www.keychron.com/products/keychron-v6-max-qmk-via-wireless-custom-mechanical-keyboard)

Make example for this keyboard (after setting up your build environment):

    make keychron/v6_max/ansi_encoder:default

Flashing example for this keyboard:

    make keychron/v6_max/ansi_encoder:default:flash

**Reset Key**: Toggle mode switch to "Cable", hold down the *Esc* key or reset button underneath space bar while connecting the USB cable,

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## My Notes
**DFU**: Hold escape when plugging in (says Reset above, but it's actually DFU).  Listen for device connection sound, seems to take about ten seconds before it goes into DFU, after which it can be flashed.

Compile:
```
qmk compile -kb keychron/v6_max/ansi_encoder -km via
```

Flash (in DFU mode):
```
sudo dfu-util --alt 0 --device 0483:df11 --dfuse-address 0x08000000:leave --download $HOME$/qmk_firmware_Keychron-wireless/keychron_v6_max_ansi_encoder_via.bin
```