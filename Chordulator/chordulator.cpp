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

#define MAX_CHORD_NOTES 8

struct chord_type {
    const char* name;
    uint8_t notes[MAX_CHORD_NOTES];
};

// Some constants
String m_saNoteNames[] = {String("C"),  String("C#"), String("D"),  String("D#"), String("E"),  String("F"),
                          String("F#"), String("G"),  String("G#"), String("A"),  String("A#"), String("B")};

struct chord_type chords[] = {
    {"None", {0, 255}},

    // Common Triads
    {"Major", {0, 4, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Minor", {0, 3, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Diminished", {0, 3, 6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Augmented", {0, 4, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},

    // Seventh Chords
    {"Major Seventh", {0, 4, 7, 11, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Minor Seventh", {0, 3, 7, 10, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Dominant Seventh", {0, 4, 7, 10, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Diminished Seventh", {0, 3, 6, 9, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Half-Diminished Seventh", {0, 3, 6, 10, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Minor Major Seventh", {0, 3, 7, 11, 0xFF, 0xFF, 0xFF, 0xFF}},

    // Extended Chords
    {"Ninth", {0, 4, 7, 10, 14, 0xFF, 0xFF, 0xFF}},
    {"Major Ninth", {0, 4, 7, 11, 14, 0xFF, 0xFF, 0xFF}},
    {"Minor Ninth", {0, 3, 7, 10, 14, 0xFF, 0xFF, 0xFF}},
    {"Eleventh", {0, 4, 7, 10, 14, 17, 0xFF, 0xFF}},
    {"Minor Eleventh", {0, 3, 7, 10, 14, 17, 0xFF, 0xFF}},
    {"Thirteenth", {0, 4, 7, 10, 14, 17, 21, 0xFF}},
    {"Minor Thirteenth", {0, 3, 7, 10, 14, 17, 21, 0xFF}},

    // Suspended Chords
    {"Suspended Second", {0, 2, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Suspended Fourth", {0, 5, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},

    // Altered Chords
    {"Augmented Seventh", {0, 4, 8, 10, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Augmented Ninth", {0, 4, 7, 10, 15, 0xFF, 0xFF, 0xFF}},
    {"Diminished Ninth", {0, 3, 6, 10, 13, 0xFF, 0xFF, 0xFF}},
    {"Flat Ninth", {0, 4, 7, 10, 13, 0xFF, 0xFF, 0xFF}},
    {"Sharp Ninth", {0, 4, 7, 10, 15, 0xFF, 0xFF, 0xFF}},
    {"Flat Fifth", {0, 4, 6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Sharp Fifth", {0, 4, 8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},

    // Add Chords
    {"Add Ninth", {0, 4, 7, 14, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Add Eleventh", {0, 4, 7, 17, 0xFF, 0xFF, 0xFF, 0xFF}},
    {"Add Thirteenth", {0, 4, 7, 21, 0xFF, 0xFF, 0xFF, 0xFF}},

    // Other Variations
    {"Sixth Ninth", {0, 4, 7, 9, 14, 0xFF, 0xFF, 0xFF}},
    {"Minor Sixth Ninth", {0, 3, 7, 9, 14, 0xFF, 0xFF, 0xFF}},

    // Tonally / harmonically related - comments relate to C tonic/root
    {"I", {0, 4, 7, 255}}, // C major
    {"C Major Seventh", {0, 4, 7, 11, 255}}, // C major seventh
    {"III", {4, 7, 11, 255}}, //E minor
    {"D Minor Seventh", {2, 5, 9, 12, 255}}, // D minor seventh
    {"V", {7, 11, 14, 255}}, // G major
    {"VII", {11, 14, 17, 255}}, // B dimished
    {"E Minor Seventh", {4, 7, 11, 14, 255}}, // E minor seventh
    {"II", {2, 5, 9, 255}}, //D minor
    {"F Major Seventh", {5, 9, 12, 16, 255}}, // F major seventh
    {"IV", {5, 9, 12, 255}}, // F major
    {"G Dominant Seventh", {7, 11, 14, 17, 255}}, // G dominant seventh
    {"VI", {9, 12, 16, 255}} // A minor
};


uint8_t numChords = sizeof(chords)/ sizeof(struct chord_type);

// Plugin that creates different chords for each note of an octave played
class Chordulator : public Plugin {
  public:
    Chordulator()
        : Plugin(14, // Quantity of parameters
                 0, // Quantity of internal presets (enable DISTRHO_PLUGIN_WANT_PROGRAMS)
                 0  // Quantity of internal states
          ) {
            for (uint8_t i = 0; i < 128; ++i)
                m_heldNotes[i] = 0;
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

    void initParameter(uint32_t index, Parameter& parameter) override {
        if (index == 12) {
            parameter.name                          = "Split Point";
            parameter.symbol                        = "split_point";
            parameter.hints                         = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.min                    = 12;
            parameter.ranges.max                    = 127 - 12;
            parameter.ranges.def                    = 60;
            parameter.enumValues.count              = 127 - 24;
            parameter.enumValues.restrictedMode     = true;
            ParameterEnumerationValue* const values = new ParameterEnumerationValue[127 - 24];
            for (uint8_t i = 0; i < 127 - 24; ++i) {
                values[i].label = m_saNoteNames[i % 12] + String(i / 12 - 1);
                values[i].value = i;
            }
            parameter.enumValues.values = values;
        } else if (index == 13){
            parameter.name                          = "Latched";
            parameter.symbol                        = "latched_mode";
            parameter.hints                         = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.min                    = 0;
            parameter.ranges.max                    = 1;
            parameter.ranges.def                    = 0;
            parameter.enumValues.count              = 2;
            parameter.enumValues.restrictedMode     = true;
            ParameterEnumerationValue* const values = new ParameterEnumerationValue[2];
            values[0].label = "off";
            values[0].value = 0;
            values[1].label = "on";
            values[1].value = 1;
            parameter.enumValues.values = values;
        } else if (index < 12) {
            String sName                            = m_saNoteNames[index] + String(" chord ");
            parameter.name                          = sName;
            parameter.symbol                        = sName.replace('#', 's').replace(' ', '_').toLower();
            parameter.hints                         = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.min                    = 1;
            parameter.ranges.max                    = numChords - 1;
            parameter.ranges.def                    = index + 1;
            parameter.enumValues.count              = numChords - 1;
            parameter.enumValues.restrictedMode     = true;
            ParameterEnumerationValue* const values = new ParameterEnumerationValue[numChords - 1];
            for (uint8_t i = 0; i < numChords -1; ++i) {
                values[i].label = chords[i + 1].name;
                values[i].value = i + 1;
            }
            parameter.enumValues.values = values;
            m_selectedChord[index + 1] = index + 1;
        }
    }

    // Get a value from a control or parameter
    float getParameterValue(uint32_t index) const override {
        if (index < 12)
            return m_selectedChord[index + 1];
        else if (index == 12)
            return m_splitPoint;
        else if (index == 13)
            return m_latchMode;
        return 0.0f;
    }

    // Set a control or parameter value
    void setParameterValue(uint32_t index, float value) override {
        if (index < 12)
            m_selectedChord[index + 1] = value;
        else if (index == 12 && value > 11 && value < 127 - 12)
            m_splitPoint = value;
        else if (index == 13)
            m_latchMode = value ? 1:0;
    }

    // Process audio and MIDI input.
    void run(const float**, float**, uint32_t, const MidiEvent* midiEvents, uint32_t midiEventCount) override {
        uint8_t status, note, velocity, noteOn, offset, prevChord, chordNote;

        for (uint32_t j = 0; j < midiEventCount; ++j) {
            // Iterate through each MIDI message
            if (midiEvents[j].kDataSize > 2 && (midiEvents[j].data[0] & 0xE0) == 0x80) {
                // Note on/off
                status = midiEvents[j].data[0];
                note = midiEvents[j].data[1];
                velocity = midiEvents[j].data[2];
                noteOn = ((status & 0x90) == 0x90) && (velocity > 0); // 0 if note-off

                if (note < m_splitPoint - 12)
                {
                    // Ignore low notes
                    //!@todo Maybe this should reset (especially in latched mode)?
                } else if (note < m_splitPoint) {
                    // Modifier notes
                    m_heldNotes[note] = noteOn;
                    if (!m_latchMode)
                        m_chord = 0;
                    for (uint8_t i = 0; i < 12; ++i) {
                        if (m_heldNotes[m_splitPoint - 12 + i] != 0) {
                            m_chord = m_selectedChord[i + 1];
                            break;
                        }
                    }
                } else {
                    // Play notes
                    if (noteOn) {
                        if (m_chord < numChords) {
                            if (m_heldNotes[note]) {
                                // Send MIDI note-off for each previously sent chord
                                prevChord = m_heldNotes[note];
                                for (uint8_t i = 0; i < MAX_CHORD_NOTES; ++i) {
                                    uint8_t offset = chords[prevChord].notes[i];
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
                            m_heldNotes[note] = m_chord;
                            for (uint8_t i = 0; i < MAX_CHORD_NOTES; ++i) {
                                offset = chords[m_chord].notes[i];
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
                        prevChord = m_heldNotes[note];
                        m_heldNotes[note] = m_chord;
                        if (prevChord < numChords) {
                            // Send MIDI note-off for each previously sent chord
                            for (uint8_t i = 0; i < MAX_CHORD_NOTES; ++i) {
                                uint8_t offset = chords[prevChord].notes[i];
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
    uint8_t m_splitPoint = 60; // MIDI note number of start of right hand keys
    uint8_t m_selectedChord[13]; // Index of the chord for each nodifier key. Index 0 is Default - no chord.
    uint8_t m_heldNotes[128]; // Index of chord type pressed for each note indexed by MIDI note number. 1 if pressed or zero if not pressed (released)
    uint8_t m_latchMode = 0; // 1 if releasing a modifier key does not reset mode

    // Set our plugin class as non-copyable and add a leak detector just in case.
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chordulator)
};

// Plugin entry point, called by DPF to create a new plugin instance.
Plugin* createPlugin() { return new Chordulator(); }

END_NAMESPACE_DISTRHO