# TonalChord

A LV2 MIDI plugin to trigger chords from single MIDI notes. Chords have tonal simulatory to each other.

The left hand selects the root/tonic of the chord and a single key press of the right hand plays a chord within the selected pallete of tonal harmony chords in that key.

## Operation

Use the "Split Point" parameter to set the split point. The 12 keys below the split point become the modifier keys. Any key below this resets to bypass (no chords created). Keys above the split point produce MIDI note on/off, i.e. plays the chord.

Play chord with root/tonic of the selected modifier key. Different, tonally related chords play for each key within the octave (C..B). Keys played in higher octave ranges play at (octave separated) higher pitches.

Adjust "Wet" control to adjust relative velocity of the chord to the root note. This allows the chord to be faded or bypassed.

## Parameters
Parameter | Description | Type | Minimum | Maximum
--------- | ----------- | ---- | ------- | -------
[C..B] Chord | Select the chord type triggered by this key | List | N/A | N/A
Split Point | Select the keyboard split between modifiers and play keys | Integer | 12 | 115
Wet | Relative velocity of chord and  root note | Float | 0 | 1

## Presets
There are presets that set the offset values for various common chords.

Preset name | Notes as semitone offsets from tonic (tonic = 1)
---- | ----
No chord | 1
Major triad | 1, 5, 8
Minor triad | 1, 4, 8
Diminishsed | 1, 4, 7
Augmented | 1, 5, 9
Major 7th | 1, 5, 8, 12
Minor 7th | 1, 4, 8, 11
Dominant 7th | 1, 5, 8, 11
Half diminished 7th | 1, 4, 7, 11
Diminished 7th | 1, 4, 7, 10
Minor-Major 7th | 1, 4, 8, 12
Augmented Major 7th | 1, 5, 9, 12
Augmented 7th | 1, 5, 9, 11
Suspended 2nd | 1, 3, 8
Suspended 4nd | 1, 6, 8
7sus4 | 1, 6, 8, 11
Add9 | 1, 5, 8, 15
Minor Add9 | 1, 4, 8, 15
Major 6th | 1, 5, 8, 10
Minor 6th | 1, 4, 8, 10
Half-Diminished Dominant | 1, 5, 7, 11
