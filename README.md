# Features

* Firmware available for modifications
* Unlike standard firmware, the screen does not blink during operation.
* It is possible to work with the screen turned off. (Hold `MODE` button)
* More fair temperature calculation 
* Ability to mute beeper

# How to build
### linux

```
sudo apt install sdcc git make
git clone https://github.com/tarwirdur/open-fixdry-nt1.git
git clone https://github.com/bschwand/STM8-SPL-SDCC.git
cd open-fixdry-nt1 ; make
```

# MCU replacement

To install this firmware, the standard microcontroller must be replaced with `STM8S103F3` or `STM8S003F3` (in TSSOP-20). \
A soldering iron is recommended for this operation.

# Flash

A programmer for stm8 is required. For example `stlink v2`.

# TODO