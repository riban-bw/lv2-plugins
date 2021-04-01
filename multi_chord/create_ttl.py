#!/usr/bin/python3

note_name=['c','c#','d','d#','e','f','f#','g','g#','a','a#','b']

print('@prefix atom:  <http://lv2plug.in/ns/ext/atom#> .')
print('@prefix doap:  <http://usefulinc.com/ns/doap#> .')
print('@prefix lv2:   <http://lv2plug.in/ns/lv2core#> .')
print('@prefix urid:  <http://lv2plug.in/ns/ext/urid#> .')
print('@prefix midi:  <http://lv2plug.in/ns/ext/midi#> .')
print('@prefix rdfs:  <http://www.w3.org/2000/01/rdf-schema#> .')
print('@prefix rdf:   <http://www.w3.org/1999/02/22-rdf-syntax-ns#> .')
print('')
print('<urn:riban.multi_chord>')
print('	a lv2:Plugin ,')
print('		lv2:MIDIPlugin ;')
print('	doap:name "MIDI Multi-Chord" ;')
print('	lv2:requiredFeature urid:map ;')
print('	lv2:optionalFeature lv2:hardRTCapable ;')
print('')
print('	lv2:port [')
print('		a lv2:InputPort ,')
print('				atom:AtomPort ;')
print('		atom:bufferType atom:Sequence ;')
print('		atom:supports midi:MidiEvent ;')
print('		lv2:index 0 ;')
print('		lv2:symbol "in" ;')
print('		lv2:name "In"')
print('	] , [')
print('		a lv2:OutputPort ,')
print('				atom:AtomPort ;')
print('		atom:bufferType atom:Sequence ;')
print('		atom:supports midi:MidiEvent ;')
print('		lv2:index 1 ;')
print('		lv2:symbol "out" ;')
print('		lv2:name "Out"')
for control in range(12):
	for note in range(4):
		print('	] , [')
		print('		a lv2:InputPort, lv2:ControlPort;')
		print('		lv2:index %d;'%(2 + control * 4 + note))
		print('		lv2:symbol "offset_%s%d";'%(note_name[control].lower().replace('#','s'), note+1))
		print('		lv2:name "Offset %s:%d";'%(note_name[control].upper(), note+1))
		print('		lv2:minimum -12;')
		print('		lv2:maximum 12;')
		print('		lv2:default 0;')
		for key in range(-12,0):
			print('		lv2:scalePoint [ rdfs:label "-%s"; rdf:value %d ; ] ;'%(note_name[(control+key)%12].upper(), key))
		print('		lv2:scalePoint [ rdfs:label "%s"; rdf:value 0 ; ] ;'%(note_name[control].upper()))
		for key in range(1,13):
			print('		lv2:scalePoint [ rdfs:label "+%s"; rdf:value %d ; ] ;'%(note_name[(control+key)%12].upper(), key))
		print('		lv2:portProperty lv2:integer, lv2:enumeration;')


for control in range(12):
	for note in range(4):
		print('	] , [')
		print('		a lv2:InputPort, lv2:ControlPort;')
		print('		lv2:index %d;'%(50 + control * 4 + note))
		print('		lv2:symbol "velocity_%s%d";'%(note_name[control].lower().replace('#','s'), note+1))
		print('		lv2:name "Velocity %s:%d";'%(note_name[control].upper(), note+1))
		print('		lv2:minimum 0.5;')
		print('		lv2:maximum 2;')
		print('		lv2:default 1;')

print('	] .')

