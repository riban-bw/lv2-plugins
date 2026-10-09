# Monophonic

A LV2 MIDI plugin that limits MIDI to monophonic. Only one MIDI note is active, based on the priority (see parameters). Per channel filtering is enabled. Sustain or hold pedal works as expected, maintinaing the currently playing note.

# Operation

Play multiple notes or chords. Only one note will sound, based on the priority mode, e.g. "Highest" priority will only sound the highest note being held. CC64 (hold/sustain pedal) can be ignored or act in different modes, either resetting when all notes are released or acting continuously until the pedal is released. Instead of the highest or lowest notes being played, you may chose the 2nd, 3rd, etc. highest or lowest note to play.

Each MIDI channel operates seperately, allowing the plugin to be used across all 16 MIDI channels multitimbrally.

All other MIDI commands are passed through to output unmodified.


## Parameters

### Priority
Defines the priority mode of the plugin thus:

Value | Description 
---- | ------------
Bypass | Plugin is bypassed and all incoming events are passed throught to output.
Highest | The highest held note is played. [Default]
Lowest | The lowest held note is played.
Last | The last held note is played.
First | The first held note is played.

### Hold Mode
Operation of CC64 hold or sustain pedal.

Value | Description
----- | -----------
None  | CC64 is ignored (passed through)
Reset | The last released note of a chord will be sustained. Resets when new chord held, e.g. with "Highest" priority, the highest note played of a chord will sustain until all keys are released and then next note or chord is played. [Default]
Cont  | Ignores released keys, e.g. with "Highest" priority, the highest note played whilst CC64 is asserted will sound.

### n-Trig
Events relate to the quantity of keys held, e.g. if value = 2, the first key is ignored. [Default: 1]

## Presets

The plugin does not support presets.
