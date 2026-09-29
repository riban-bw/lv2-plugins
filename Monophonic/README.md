# Monophonic

A LV2 MIDI plugin that limits MIDI to monophonic. Only one MIDI note is active, based on the mode (see parameters). Per channel filtering is enabled. Sustain or hold pedal works as expected, maintinaing the currently playing note.


## Parameters

There is a single parameter: mode. This defines the operating mode of the plugin thus:

Mode | Description 
---- | -----------
Bypass | Plugin is bypassed and all incoming events are passed throught to output.
Highest | The highest held note is played.
Lowest | The lowest held note is played.
Last | The last held note is played.
First | The first held note is played.

## Presets

The plugin does not support presets.
