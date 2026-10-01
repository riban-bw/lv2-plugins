# Monophonic

A LV2 MIDI plugin that limits MIDI to monophonic. Only one MIDI note is active, based on the mode (see parameters). Per channel filtering is enabled. Sustain or hold pedal works as expected, maintinaing the currently playing note.


## Parameters

### Priority
Defines the priority mode of the plugin thus:

Value | Description 
---- | ------------
Bypass | Plugin is bypassed and all incoming events are passed throught to output.
Highest | The highest held note is played.
Lowest | The lowest held note is played.
Last | The last held note is played.
First | The first held note is played.

### Hold Mode
Operation of hold or sustain pedal.

Value | Description
----- | -----------
Reset | The last held key will be sustained. Resets when new chord held.
Cont  | Ignores released keys. 

### n-Trig
Events relate to the quantity of keys held, e.g. if value = 2, the first key is ignored.

## Presets

The plugin does not support presets.
