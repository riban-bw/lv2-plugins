# lv2

A selection of LV2 plugins for Zynthian (and other LV2 hosts).

## Multi-Chord
```
Framework: DISTRHO
Targets: LV2, VST, jackd standalone
```
Multi-chord is a MIDI processor. It will create a chord from each MIDI note received. Each incoming note C,C#,D,D#,E,F,F#,G,G#,A,A#,B can trigger any chord, individually configurable, e.g. incoming MIDI 'C' may trigger a C major chord whilst incoming MIDI 'C#' could trigger A minor. Each chord may have up to 4 notes. Any note configured the same as the first will not sound, e.g. setting C1='C', C2='E', C3='C', C4='C' will play C + E when a MIDI 'C' is received.

The default is to pass all MIDI events uneffected. Some presets are included for some common chord patters, e.g. major triad, minor triad, etc.

Each note for each chord may be selected from an octave either side of the root (played) note. Chord patterns repeat for each octave, e.g. incoming MIDI 'C' will trigger the same chord but in different registers when played across the keyboard.

Velocity for each note of the chord may be set relative to the MIDI trigger note in the range x0.5 .. x2.

The plugin may be recompiled with fewer or greater quantity of notes in each chord. This will change the parameter layout and invalidate any presets.

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
