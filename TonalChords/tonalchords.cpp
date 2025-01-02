/* riban TonalChords plugin built on DISTRHO Plugin Framework (DPF)
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

    // Tonally / harmonically related - comments relate to C tonic/root
    {"I", {0, 4, 7, 255, 255, 255, 255, 255}}, // C major
    {"C Major Seventh", {0, 4, 7, 11, 255, 255, 255, 255}}, // C major seventh
    {"III", {4, 7, 11, 255, 255, 255, 255, 255}}, //E minor
    {"D Minor Seventh", {2, 5, 9, 12, 255, 255, 255, 255}}, // D minor seventh
    {"V", {7, 11, 14, 255, 255, 255, 255, 255}}, // G major
    {"VII", {11, 14, 17, 255, 255, 255, 255, 255}}, // B dimished
    {"E Minor Seventh", {4, 7, 11, 14, 255, 255, 255, 255}}, // E minor seventh
    {"II", {2, 5, 9, 255, 255, 255, 255, 255}}, //D minor
    {"F Major Seventh", {5, 9, 12, 16, 255, 255, 255, 255}}, // F major seventh
    {"IV", {5, 9, 12, 255, 255, 255, 255, 255}}, // F major
    {"G Dominant Seventh", {7, 11, 14, 17, 255, 255, 255, 255}}, // G dominant seventh
    {"VI", {9, 12, 16, 255, 255, 255, 255, 255}}, // A minor
};

uint8_t numChords = sizeof(chords)/ sizeof(struct chord_type);

// Plugin that creates different chords for each note of an octave played
class TonalChords : public Plugin {
  public:
    TonalChords()
        : Plugin(13, // Quantity of parameters
                 0, // Quantity of internal presets (enable DISTRHO_PLUGIN_WANT_PROGRAMS)
                 0  // Quantity of internal states
          ) {
            for (uint8_t i = 0; i < 128; ++i)
                for (uint8_t j = 0; j < MAX_CHORD_NOTES; ++j)
                    m_heldNotes[i][j] = 255;
          }

  protected:
    // Get the plugin label. Short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    const char* getLabel() const override { return "Tonal Chord"; }

    // Get an full description about the plugin.
    const char* getDescription() const override { return "Plugin that creates different chords for each note of the octave based on tonal harmony"; }

    // Get the plugin author.
    const char* getMaker() const override { return "riban"; }

    // Get the plugin homepage.
    const char* getHomePage() const override { return "https://github.com/riban-bw/lv2-plugins"; }

    // Get the plugin license name (a single line of text).
    const char* getLicense() const override { return "ISC"; }

    // Get the plugin version, in hexadecimal.
    uint32_t getVersion() const override { return d_version(0, 1, 0); }

    // Get the plugin unique Id. Used by LADSPA, DSSI and VST plugin formats.
    int64_t getUniqueId() const override {
        int64_t nValue = ('r' << 24) | ('i' << 16) | ('b' << 8) | ('a' << 0);
        return (nValue << 32) | ('n' << 24) | 3;
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
            m_tonalChord[index] = index + 1;
        }
    }

    // Get a value from a control or parameter
    float getParameterValue(uint32_t index) const override {
        if (index < 12)
            return m_tonalChord[index + 1];
        else if (index == 12)
            return m_splitPoint;
        return 0.0f;
    }

    // Set a control or parameter value
    void setParameterValue(uint32_t index, float value) override {
        //!@todo All notes off then reassert relevant (held play note) chords
        if (index < 12)
            m_tonalChord[index + 1] = value;
        else if (index == 12 && value > 11 && value < 127 - 12)
            m_splitPoint = value;
    }

    // Process audio and MIDI input.
    void run(const float**, float**, uint32_t, const MidiEvent* midiEvents, uint32_t midiEventCount) override {
        uint8_t status, note, velocity, noteOn, offset, chordNote, chordIndex;

        for (uint32_t j = 0; j < midiEventCount; ++j) {
            // Iterate through each MIDI message
            if (midiEvents[j].kDataSize > 2 && (midiEvents[j].data[0] & 0xE0) == 0x80) {
                // Note on/off
                status = midiEvents[j].data[0];
                note = midiEvents[j].data[1];
                velocity = midiEvents[j].data[2];
                noteOn = ((status & 0x90) == 0x90) && (velocity > 0); // 0 if note-off

                if (note < m_splitPoint) {
                    // Modifier notes
                    if (note < m_splitPoint - 12)
                        ; //!@todo Bypass
                    else if (noteOn)
                        m_tonic = note - (m_splitPoint - 12);
                } else {
                    // Play notes
                    if (noteOn) {
                        if (m_tonic >= 12)
                            continue; //!@todo Implement bypass
                        // Send MIDI note-off for each previously sent chord
                        for (uint8_t i = 0; i < MAX_CHORD_NOTES; ++i) {
                            chordNote = m_heldNotes[note][i];
                            m_heldNotes[note][i] = 255;
                            if (chordNote > 127)
                                continue;
                            MidiEvent chordEvent;
                            memcpy(&chordEvent, &midiEvents[j], sizeof(MidiEvent));
                            chordEvent.data[0] &= 0x8F;
                            chordEvent.data[1] = chordNote;
                            chordEvent.data[2] = 0;
                            writeMidiEvent(chordEvent);
                        }
                        chordIndex = m_tonalChord[(note % 12) + 1];
                        for (uint8_t i = 0; i < MAX_CHORD_NOTES; ++i) {
                            offset = chords[chordIndex].notes[i];
                            if (offset == 255)
                                chordNote = 255;
                            else
                                chordNote = (note / 12) * 12 + offset + m_tonic;
                            m_heldNotes[note][i] = chordNote;
                            if (chordNote > 127)
                                continue;
                            MidiEvent chordEvent;
                            memcpy(&chordEvent, &midiEvents[j], sizeof(MidiEvent));
                            chordEvent.data[1] = chordNote;
                            chordEvent.data[2] = velocity;
                            writeMidiEvent(chordEvent);
                        }
                    } else {
                        // Release note - send associated MIDI note-off messages
                        for (uint8_t i = 0; i < MAX_CHORD_NOTES; ++i) {
                            chordNote = m_heldNotes[note][i];
                            m_heldNotes[note][i] = 255;
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
            } else
                writeMidiEvent(midiEvents[j]); // Pass through unprocessed MIDI data
        }
    }

  private:
    uint8_t m_tonic = 0; // The root note of the chord - defines the key the chord harmonies are within
    uint8_t m_splitPoint = 60; // MIDI note number of start of right hand (play) keys
    uint8_t m_tonalChord[13]; // Index of the chord for each ocatave key. Index 0 is bypass (no chord)
    uint8_t m_heldNotes[128][MAX_CHORD_NOTES]; // Currently held notes, indexed by MIDI note number. For play keys this holds the index of chord type when the key was pressed 

    // Set our plugin class as non-copyable and add a leak detector just in case.
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TonalChords)
};

// Plugin entry point, called by DPF to create a new plugin instance.
Plugin* createPlugin() { return new TonalChords(); }

END_NAMESPACE_DISTRHO
