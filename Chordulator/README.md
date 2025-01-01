# Chordulator

A LV2 MIDI plugin to trigger chords from single MIDI notes, with chord configuration defined by other MIDI notes.

The MIDI keyboard is split at middle C. Notes below the split point define the chord to be triggered. Notes from and above the split point define the root note.

Keys from C3 (MIDI note 48, C below middle C) define what type of chord is played when keys above the split point (MIDI note 60 and above) are pressed.

Key | MIDI Note | Chord type 
--- | --------- | ----------
C3  | 48 | Major
C#3 | 49 | Minor