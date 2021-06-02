# lv2

A selection of LV2 plugins for Zynthian (and other LV2 hosts).

## Multi-Chord
```
Framework: DISTRHO
Targets: LV2, VST, jackd standalone
```
MIDI processor to create a chord from each MIDI note received.
[Documentation](https://github.com/riban-bw/lv2/tree/main/MultiChord)

## Installation

To install riban lv2 plugins.

Clone repository and build using make:
```
git clone --recursive https://github.com/riban-bw/lv2.git riban
cd riban
make
sudo make install
```

DISTRHO plugin framework is built then each of the riban plugins. The plugins are placed in the `bin` directory with the LV2 versions within `bin/lv2`. By default `make install` will install the LV2 plugins in `/usr/local/lv2`. This behaviour may be overriden by setting environmental variable LV2_INSTALL_PREFIX.

To create a Debian package (currently armhf LV2 only):

```
make deb
```

The pacakge is created in the bin directory.
