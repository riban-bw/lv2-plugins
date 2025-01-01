# Chordulator

A LV2 MIDI plugin to trigger chords from single MIDI notes, with chord configuration defined by other MIDI notes.

The MIDI keyboard is split at middle C. Notes below the split point define the chord to be triggered. Notes above the split point define the chord root note.

There are 12 keys below the split point that define which chord type is played when any key above the split point is pressed. The lowest pressed chord specifier key is used. Each chord type key has a parameter to allow the chord type to be selected. There is a parameter to select the split point (default is middle C, MIDI note 60).
