#include "./multi_chord.h"

#include "lv2/atom/atom.h"
#include "lv2/atom/util.h"
#include "lv2/core/lv2.h"
#include "lv2/core/lv2_util.h"
#include "lv2/log/log.h"
#include "lv2/log/logger.h"
#include "lv2/midi/midi.h"
#include "lv2/urid/urid.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NOTES 3

enum {
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
	MultiChordURIs uris;
} MultiChord;

static void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
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
	for(uint32_t i=0; i<12; ++i)
		for(uint32_t j=0; j<MAX_NOTES; ++j)
			if(port == 2 + i * MAX_NOTES + j)
				self->offset_map[i][j] = (float*)data;
}

static LV2_Handle instantiate(const LV2_Descriptor*     descriptor,
	double                    rate,
	const char*               path,
	const LV2_Feature* const* features)
{
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

	map_multi_chord_uris(self->map, &self->uris);
	return (LV2_Handle)self;
}

static void cleanup(LV2_Handle instance)
{
	free(instance);
}

static void run(LV2_Handle instance, uint32_t sample_count)
{
	MultiChord*     self = (MultiChord*)instance;
	MultiChordURIs* uris = &self->uris;

	// Struct for a 3 byte MIDI event, used for writing notes
	typedef struct {
		LV2_Atom_Event event;
		uint8_t        msg[3];
	} MIDINoteEvent;

	// Initially self->midi_out contains a Chunk with size set to capacity

	// Get the capacity
	const uint32_t out_capacity = self->midi_out->atom.size;

	// Write an empty Sequence header to the output
	lv2_atom_sequence_clear(self->midi_out);
	self->midi_out->atom.type = self->midi_in->atom.type;

	// Read incoming events
	uint8_t base_note;
	LV2_ATOM_SEQUENCE_FOREACH (self->midi_in, ev) {
		if (ev->body.type == uris->midi_Event) {
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
						MIDINoteEvent midi_note;
						midi_note.event = *ev;					
						midi_note.msg[0] = msg[0];
						midi_note.msg[1] = msg[1] + offset; // Transpose
						midi_note.msg[2] = msg[2];
						lv2_atom_sequence_append_event(self->midi_out, out_capacity, &midi_note.event);
					}
				break;
			  default:
				// Forward all other MIDI events directly
				lv2_atom_sequence_append_event(self->midi_out, out_capacity, ev);
				break;
			 }
		}
	}
}

static const void* extension_data(const char* uri)
{
	return NULL;
}

static const LV2_Descriptor descriptor = {
	MULTI_CHORD_URI,
	instantiate,
	connect_port,
	NULL, // activate,
	run,
	NULL, // deactivate,
	cleanup,
	extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return index == 0 ? &descriptor : NULL;
}