/* riban Chordulator plugin built on DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2025 Brian Walton <brian@riban.co.uk>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO
#define MAX_CHORD_NOTES 6
// Some constants
uint8_t chordBase = 48; // MIDI note number of start of left hand keys
uint8_t splitPoint = 60; // MIDI note number of start of right hand keys
uint8_t chords[][MAX_CHORD_NOTES] = {
        {0, 255}, // Single note (no chord selected)
        {0, 4, 7}, // Major triad
        {0, 3, 7}, // Minor triad
        {0, 6, 4, 9, 2} // Mystical chord
    };
uint8_t numChords = sizeof(chords)/ MAX_CHORD_NOTES;
uint8_t heldNotes[128]; // Index of chord type pressed for each note indexed by MIDI note number. 1 if pressed or zero if not pressed (released)

// Plugin that creates different chords for each note of an octave played
class Chordulator : public Plugin {
  public:
    Chordulator()
        : Plugin(0, // Quantity of parameters
                 0, // Quantity of internal presets (enable DISTRHO_PLUGIN_WANT_PROGRAMS)
                 0  // Quantity of internal states
          ) {
            for (uint8_t i = 0; i < 128; ++i)
                heldNotes[i] = 0;
          }

  protected:
    // Get the plugin label. Short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    const char* getLabel() const override { return "Chordulator"; }

    // Get an full description about the plugin.
    const char* getDescription() const override { return "Plugin that creates different chords for each note of the octave based on modulator notes"; }

    // Get the plugin author.
    const char* getMaker() const override { return "riban"; }

    // Get the plugin homepage.
    const char* getHomePage() const override { return "https://github.com/riban-bw/lv2-plugins"; }

    // Get the plugin license name (a single line of text).
    const char* getLicense() const override { return "ISC"; }

    // Get the plugin version, in hexadecimal.
    uint32_t getVersion() const override { return d_version(1, 0, 0); }

    // Get the plugin unique Id. Used by LADSPA, DSSI and VST plugin formats.
    int64_t getUniqueId() const override {
        int64_t nValue = ('r' << 24) | ('i' << 16) | ('b' << 8) | ('a' << 0);
        return (nValue << 32) | ('n' << 24) | 2;
    }

    // Process audio and MIDI input.
    void run(const float**, float**, uint32_t, const MidiEvent* midiEvents, uint32_t midiEventCount) override {
        uint8_t status, note, velocity, noteOn, offset, prevChord, chordNote;
        for (uint32_t j = 0; j < midiEventCount; ++j) {
            if (midiEvents[j].kDataSize > 2 && (midiEvents[j].data[0] & 0xE0) == 0x80) {
                // Note on/off
                status = midiEvents[j].data[0];
                note = midiEvents[j].data[1];
                velocity = midiEvents[j].data[2];
                noteOn = ((status & 0x90) == 0x90) && (velocity > 0);
                if (note < chordBase)
                {
                    // Ignore low notes
                } else if (note < splitPoint) {
                    // Modifier notes
                    offset = note - chordBase;
                    if (offset > numChords)
                        continue;
                    heldNotes[note] = noteOn;
                    m_chord = 0;
                    for (uint8_t i = 1; i < numChords; ++i) {
                        if (heldNotes[chordBase + i] != 0) {
                            m_chord = i;
                            break;
                        }
                    }
                } else {
                    // Play notes
                    if (noteOn) {
                        if (m_chord < numChords) {
                            if (heldNotes[note]) {
                                // Send MIDI note-off for each previously sent chord
                                prevChord = heldNotes[note];
                                for (uint8_t i = 0; i < MAX_CHORD_NOTES; ++i) {
                                    uint8_t offset = chords[prevChord][i];
                                    if (offset == 255)
                                        break; // A note entry of 255 indicates end of chord
                                    chordNote = note + offset;
                                    if (chordNote > 127)
                                        continue;
                                    MidiEvent chordEvent;
                                    memcpy(&chordEvent, &midiEvents[j], sizeof(MidiEvent));
                                    chordEvent.data[0] &= 0x8F;
                                    chordEvent.data[1] = chordNote;
                                    chordEvent.data[2] = 0;
                                    writeMidiEvent(chordEvent);
                                }
                            }
                            heldNotes[note] = m_chord;
                            for (uint8_t i = 0; i < MAX_CHORD_NOTES; ++i) {
                                offset = chords[m_chord][i];
                                    if (offset == 255)
                                        break; // A note entry of 255 indicates end of chord
                                chordNote = note + offset;
                                if (chordNote > 127)
                                    continue;
                                MidiEvent chordEvent;
                                memcpy(&chordEvent, &midiEvents[j], sizeof(MidiEvent));
                                chordEvent.data[1] = chordNote;
                                chordEvent.data[2] = velocity;
                                writeMidiEvent(chordEvent);
                            }
                        }
                    } else {
                        // Release note - send associated MIDI note-off messages
                        prevChord = heldNotes[note];
                        heldNotes[note] = m_chord;
                        if (prevChord < numChords) {
                            // Send MIDI note-off for each previously sent chord
                            for (uint8_t i = 0; i < MAX_CHORD_NOTES; ++i) {
                                uint8_t offset = chords[prevChord][i];
                                if (offset == 255)
                                    break; // A note entry of 255 indicates end of chord
                                chordNote = note + offset;
                                if (chordNote > 127)
                                    continue;
                                MidiEvent chordEvent;
                                memcpy(&chordEvent, &midiEvents[j], sizeof(MidiEvent));
                                chordEvent.data[0] &= 0x8F;
                                chordEvent.data[1] = chordNote;
                                chordEvent.data[2] = 0;
                                writeMidiEvent(chordEvent);
                            }
                        }
                    }
                }
            } else
                writeMidiEvent(midiEvents[j]); // Pass through unprocessed MIDI data
        }
    }

  private:
    uint8_t m_chord = 0; // Currently selected chord

    // Set our plugin class as non-copyable and add a leak detector just in case.
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chordulator)
};

// Plugin entry point, called by DPF to create a new plugin instance.
Plugin* createPlugin() { return new Chordulator(); }

END_NAMESPACE_DISTRHO
