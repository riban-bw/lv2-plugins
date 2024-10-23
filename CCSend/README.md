# CCSend

A LV2 MIDI plugin to send MIDI CC commands.

This plugin simply creates 4 control ports that can be driven between 0..127. Each sends a MIDI CC command for each change of value.

There are a further 4 control ports to allow setting the CC number for each of the send ports.

There is a MIDI input and MIDI output. All MIDI traffic is passed without processing (MIDI thru). We could create this plugin without MIDI input but zynthian (its primary target host) expects a MIDI input for its MIDI plugins.
