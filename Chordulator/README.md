# Chordulator

A LV2 MIDI plugin to trigger chords from single MIDI notes, with chord configuration defined by other MIDI notes.

The left hand selects the chord type and a single key press of the right hand plays that chord with root/tonic of the pressed key.

## Operation

Use the "Split Point" parameter to set the split point. The 12 keys below the split point become the modifier keys. Any key below this resets to bypass (no chords created). Keys above the split point produce MIDI note on/off, i.e. create sound.

Play chords with root/tonic of the pressed key.

Hold down one of the 12 modifier keys with the left hand. Play individual notes with the right hand to hear the corresponding chord. If multiple modifier keys are pressed, the lowest pressed key is used.

Adjust the 12 chord parameters to select which chord type will be selected for each key of the modifier range.

Enable the "Latch" parameter to latch the modifier key so there is no need to hold them whilst pressing the play (right hand) keys.

Adjust "Wet" control to adjust relative velocity of the chord to the root note. This allows the chord to be faded or bypassed.

## Parameters
Parameter | Description | Type | Minimum | Maximum
--------- | ----------- | ---- | ------- | -------
[C..B] Chord | Select the chord type triggered by this modifier key | List | N/A | N/A
Split Point | Select the keyboard split between modifiers and play keys | Integer | 12 | 115
Latched | Enable modifier key latched mode | boolean | off | on
Wet | Relative velocity of chord and  root note | Float | 0 | 1
