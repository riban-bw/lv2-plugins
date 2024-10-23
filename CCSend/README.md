# CCSend

A LV2 MIDI plugin to send MIDI CC, PC & BS commands.

This plugin simply creates some control ports that can be driven between 0..127. Each sends a MIDI continuous control command for each change of value. There are also ports that send program change and bank select (MSB & LSB).

There are further control ports to allow setting the CC number for each of the send ports and the MIDI channel. There is a global MIDI channel control and individual MIDI channel controls for each CC control which may be set to the global channel or any specific channel, 1..16.

There is a MIDI input and MIDI output. All MIDI traffic is passed without processing (MIDI thru). We could create this plugin without MIDI input but zynthian (its primary target host) expects a MIDI input for its MIDI plugins.

By default there are 8 CC controls. This can be changed at compile time by setting NUM_CC preprocessor macro.