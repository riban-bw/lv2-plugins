/*	LV2 plugin to create chords when a single MIDI note is received
*	Different chords may be configured for each note within the octave (12 different chords)
*
*	Copyright Brian Walton (brian@riban.co.uk) 2021
*	Derived from fifths example but little remains!
*/

#include "lv2/atom/util.h"
#include "lv2/core/lv2_util.h"
#include "lv2/log/logger.h"
#include "lv2/midi/midi.h"

#include <stdlib.h> //provides calloc, free

#define URI "urn:riban.multi_chord"
#define MAX_NOTES 3 // Quantity of notes in chord - need to change ttl if changing this value

enum
{
	MIDI_IN = 0,
	MIDI_OUT = 1,
};

typedef struct {
  // Features
	LV2_URID_Map*  map;
	LV2_Log_Logger logger;

	// Ports
	const LV2_Atom_Sequence*	midi_in;
	LV2_Atom_Sequence*			midi_out;
	float*						offset_map[12][MAX_NOTES];

	// URIs
	LV2_URID uri_midi;
} MultiChord;

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
	MultiChord* self = (MultiChord*)instance;
	switch (port) {
		case MIDI_IN:
			self->midi_in = (const LV2_Atom_Sequence*)data;
			break;
		case MIDI_OUT:
			self->midi_out = (LV2_Atom_Sequence*)data;
			break;
		default:
			break;
	}
	// Connect control ports starting from index 2
	// If ttl file does not list all ports then the map will not be fully populated which will trigger segfault when corresponding note is played
	for(uint32_t i=0; i<12; ++i)
		for(uint32_t j=0; j<MAX_NOTES; ++j)
			if(port == 2 + i * MAX_NOTES + j)
				self->offset_map[i][j] = (float*)data;
}

static LV2_Handle instantiate(const LV2_Descriptor*     descriptor,
	double                    rate,
	const char*               path,
	const LV2_Feature* const* features) {
	// Allocate and initialise instance structure.
	MultiChord* self = (MultiChord*)calloc(1, sizeof(MultiChord));
	if (!self) {
		return NULL;
	}

	// Initialise map with no offsets
	for(uint32_t i=0; i<12; ++i)
		for(uint32_t j=0; j<MAX_NOTES; ++j)
			self->offset_map[i][j] = 0;
	
	
	// Scan host features for URID map
	// clang-format off
	const char*  missing = lv2_features_query(
		features,
		LV2_LOG__log,  &self->logger.log, false,
		LV2_URID__map, &self->map,        true,
		NULL);
	// clang-format on

	lv2_log_logger_set_map(&self->logger, self->map);
	if (missing) {
		lv2_log_error(&self->logger, "Missing feature <%s>\n", missing);
		free(self);
		return NULL;
	}

	self->uri_midi = self->map->map(self->map->handle, LV2_MIDI__MidiEvent);
	return (LV2_Handle)self;
}

static void cleanup(LV2_Handle instance) {
	free(instance);
}

static void run(LV2_Handle instance, uint32_t sample_count) {
	MultiChord* self = (MultiChord*)instance;

	// Struct for a 3 byte MIDI event, used for writing notes
	typedef struct {
		LV2_Atom_Event event;
		uint8_t msg[3];
	} MIDINoteEvent;

	// Get the capacity
	const uint32_t out_capacity = self->midi_out->atom.size;

	// Write an empty Sequence header to the output
	lv2_atom_sequence_clear(self->midi_out);
	self->midi_out->atom.type = self->midi_in->atom.type;

	// Read incoming events
	uint8_t base_note;
	int note, velocity;
	LV2_ATOM_SEQUENCE_FOREACH (self->midi_in, ev) {
		if (ev->body.type == self->uri_midi) {
			const uint8_t* const msg = (const uint8_t*)(ev + 1);
			switch (lv2_midi_message_type(msg)) {
				case LV2_MIDI_MSG_NOTE_ON:
				case LV2_MIDI_MSG_NOTE_OFF:
					base_note = msg[1] % 12;
					for(int i=0; i < MAX_NOTES; ++i)
					{
						int offset = (int)*(self->offset_map[base_note][i]);
						if(i > 0 && offset == (int)*(self->offset_map[base_note][0]))
							continue; // Skip if note not configured (same as root note)
						note = msg[1] + offset;
						if(note > 127 || note < 0)
							continue; // Transposed note is out of range
						velocity = msg[2];
						if(velocity > 127)
							velocity = 127; // Not too loud
						if (velocity < 1)
							velocity = 1; // Not too quiet (just right!)
						MIDINoteEvent midi_note;
						midi_note.event = *ev; // Does not do a deep copy so need to set msg values
						midi_note.msg[0] = msg[0]; // Same status
						midi_note.msg[1] = note;
						midi_note.msg[2] = velocity;
						lv2_atom_sequence_append_event(self->midi_out, out_capacity, &midi_note.event);
					}
				break;
			  default:
				// Pass unprocessed MIDI events
				lv2_atom_sequence_append_event(self->midi_out, out_capacity, ev);
				break;
			 }
		}
	}
}

static const void* extension_data(const char* uri) {
	return NULL;
}

static const LV2_Descriptor descriptor = {
	URI,
	instantiate,
	connect_port,
	NULL, // Don't handle activate,
	run,
	NULL, // Don't handle deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) {
	return index == 0 ? &descriptor : NULL;
}