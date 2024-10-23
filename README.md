# lv2

A selection of LV2 plugins for Zynthian (and other LV2 hosts).

## Multi-Chord
```
Framework: DISTRHO
Targets: LV2 (other targets may be enabled in each Makefile but are not tested or supported)
```
MIDI processor to create a chord from each MIDI note received.

[Documentation](https://github.com/riban-bw/lv2/tree/main/MultiChord)

## CC-Send
```
Framework: DISTRHO
Targets: LV2 (other targets may be enabled in each Makefile but are not tested or supported)
```
Simple plugin to send MIDI CC commands when a control port is adjusted.

[Documentation](https://github.com/riban-bw/lv2/tree/main/CCSend)

## Installation

To install riban lv2 plugins.

Clone repository recursively and build using make:
```
git clone --recursive https://github.com/riban-bw/lv2-plugins.git riban-lv2
cd riban-lv2
make
sudo make install
```

Note: [DPF issue 348](https://github.com/DISTRHO/DPF/issues/348) must be fixed. My current recommendation is to change the penultimate line in `DPF/utils/generate-ttl.sh` from `cd ..` to `cd -`.

DISTRHO plugin framework is built then each of the riban plugins. The plugins are placed in the `bin` directory with the LV2 versions within `bin/lv2`. By default `make install` will install the LV2 plugins in `/usr/lv2`. This behaviour may be overriden by setting environmental variable LV2_INSTALL_PREFIX.

To create a Debian package (arm64 LV2 only):

```
make deb
```

The package is created in the bin directory.
