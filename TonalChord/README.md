# TonalChord

A LV2 MIDI plugin to trigger chords from single MIDI notes. Chords have tonal simulatiry to each other.

The left hand selects the root/tonic of the chord and a single key press of the right hand plays a chord within the selected palet of tonal harmony chords in that key.

## Operation

Use the "Split Point" parameter to set the split point. The 12 keys below the split point become the modifier keys. Any key below this resets to bypass (no chords created). Keys above the split point produce MIDI note on/off, i.e. create sound.

Play chord with root/tonic of the selected modifier key. Different, tonally related chords play for each key within the octave (C..B). Keys played in higher octave ranges play at (octave separated) higher pitches.

## Parameters
Parameter | Description | Type | Minimum | Maximum
--------- | ----------- | ---- | ------- | -------
[C..B] Chord | Select the chord type triggered by this key | List | N/A | N/A
Split Point | Select the keyboard split between modifiers and play keys | Integer | 12 | 115
