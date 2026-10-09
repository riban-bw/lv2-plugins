# CCSend

A LV2 MIDI plugin to send MIDI CC, PC & BS commands.

## Operation

This plugin simply creates some control ports that can be driven between 0..127. Each sends a MIDI continuous control command for each change of value. There are also ports that send program change and bank select (MSB & LSB).

There are further control ports to allow setting the CC number for each of the send ports and the MIDI channel. There is a global MIDI channel control and individual MIDI channel controls for each CC control which may be set to the global channel or any specific channel, 1..16.

There is a MIDI input and MIDI output. All MIDI traffic is passed without processing (MIDI thru). We could create this plugin without MIDI input but zynthian (its primary target host) expects a MIDI input for its MIDI plugins.

By default there are 8 CC controls. This can be changed at compile time by setting NUM_CC preprocessor macro.

## Parameters

Parameter | Description | Type | Minimum | Maximum
--------- | ----------- | ---- | ------- | -------
Program   | Program change number to send | Integer | 0 | 127
Bank MSB  | Bank Select MSB number to send | Integer | 0 | 127
Bank LSB  | Bank Select LSB number t sened | Integer | 0 | 127
Ctrl x    | CC value to send to port x | Integer | 0 | 127
Global Channel | The default MIDI channel | Integer | 1 | 16
x Channel | The MIDI channel for port x (1-16 or Global) | List ||
x CC#     | CC number to send to port x | Integer | 0 | 127
Bank mode | How to send Bank commands (see below) | List ||

Bank Mode defines what messages are sent when a bank is changed.

Mode | Description
---- | -----------
BS Only    | Send Bank Select only
BS+PC      | Send Bank Select and Program Change
LSB+MSB    | Send LSB and MSB CC
LSB+MSB+PC | Send LSB and MSB CC and Program Change
