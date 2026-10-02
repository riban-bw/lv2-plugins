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
[C..B] Chord | Select the chord type triggered by this modifier key | List | See table below | N/A
Split Point | Select the keyboard split between modifiers and play keys | Integer | 12 | 115
Latched | Enable modifier key latched mode | boolean | off | on
Wet | Relative velocity of chord and  root note | Float | 0 | 1

List of chord types:
Chord name | Notes as semitone offsets from tonic (tonic = 1)
---- | ----
Major | 1, 5, 9
Minor | 1, 4, 9
Diminished | 1, 4, 7
Augmented | 1, 5, 9
Major Seventh | 1, 5, 8, 12
Minor Seventh | 1, 4, 8, 11
Dominant Seventh | 1, 5, 8, 11
Diminished Seventh | 1, 4, 7, 10
Half-Diminished Seventh | 1, 4, 7, 11
Minor Major Seventh | 1, 4, 8, 12
Ninth | 1, 5, 8, 11, 15
Major Ninth | 1, 5, 8, 12, 15
Minor Ninth | 1, 4, 8, 11, 15
Eleventh | 1, 5, 8, 11, 15, 18
Minor Eleventh | 1, 4, 8, 11, 15, 18
Thirteenth | 1, 5, 8, 11, 15, 18, 22
Minor Thirteenth | 1, 4, 8, 11, 15, 18, 22
Suspended Second | 1, 3, 8
Suspended Fourth | 1, 6, 8
Augmented Seventh | 1, 5, 9, 11
Augmented Ninth | 1, 5, 8, 11, 16
Diminished Ninth | 1, 4, 7, 11, 14
Flat Ninth | 1, 5, 8, 11, 14
Sharp Ninth | 1, 5, 8, 11, 16
Flat Fifth | 1, 5, 7
Sharp Fifth | 1, 5, 9
Add Ninth | 1, 5, 8, 15
Add Eleventh | 1, 5, 8, 18
Add Thirteenth | 1, 5, 8, 22
Sixth Ninth | 1, 5, 8, 10, 15
Minor Sixth Ninth | 1, 4, 8, 10, 15
