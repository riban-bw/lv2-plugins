# Chordulator

A LV2 MIDI plugin to trigger chords from single MIDI notes, with chord configuration defined by other MIDI notes.

Set the split point using the last controller (Split Point). The 12 keys below the split point become the modifier keys. Keys above the split point produce MIDI note on/off, i.e. create sound.

Hold down one of the 12 modifier keys with the left hand. Play individual notes with the right hand to hear the corresponding chord.

Adjust the 12 chord parameters to select which chord type will be triggered when each modifier key is pressed.