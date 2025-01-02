# Chordulator

A LV2 MIDI plugin to trigger chords from single MIDI notes, with chord configuration defined by other MIDI notes.

Set the split point using the last controller (Split Point). The 12 keys below the split point become the modifier keys. Keys above the split point produce MIDI note on/off, i.e. create sound.

Hold down one of the 12 modifier keys with the left hand. Play individual notes with the right hand to hear the corresponding chord. If the "latched" parameter is on then the chord type latches when pressed and does not need to be held. If multiple modifier keys are pressed, the lowest pressed key is used.

Adjust the 12 chord parameters to select which chord type will be triggered when each modifier key is pressed.

## Parameters
Parameter | Description | Type | Minimum | Maximum
--------- | ----------- | ---- | ------- | -------
[C..B] Chord | Select the chord type triggered by this modifier key | List | N/A | N/A
Split Point | Select the keyboard split between modifiers and play keys | Integer | 12 | 115
Latched | Enable modifier key latched mode | boolean | off | on

