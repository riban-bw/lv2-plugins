/* riban MultiChord plugin built on DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2021-2025 Brian Walton <brian@riban.co.uk>
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

#define MAX_NOTES 4 // Maximum notes in a chord
#define NUM_PRESETS sizeof(CHORDS) / MAX_NOTES // Quantity of preset chords
#define NUM_PARAMS 24 * MAX_NOTES + 1

START_NAMESPACE_DISTRHO

// Some constant strings used in naming elements
String m_saNoteNames[] = {String("C"),  String("C#"), String("D"),  String("D#"), String("E"),  String("F"),
                          String("F#"), String("G"),  String("G#"), String("A"),  String("A#"), String("B")};

char CHORDS[][MAX_NOTES] = {
    {0, 0, 0, 0}, // No chord
    // Triads
    {0, 4, 7, 0}, // Major triad
    {0, 3, 7, 0}, // Minor triad
    {0, 3, 6, 0}, // Diminishsed
    {0, 4, 8, 0}, // Augmented
    // Seventh chords
    {0, 4, 7, 11}, // Major 7th
    {0, 3, 7, 10}, // Minor 7th
    {0, 4, 7, 10}, // Dominant 7th
    {0, 3, 6, 10}, // Half diminished 7th
    {0, 3, 6, 9}, // Diminished 7th
    {0, 3, 7, 11}, // Minor-Major 7th
    {0, 4, 8, 11}, // Augmented Major 7th
    {0, 4, 8, 10}, // Augmented 7th
    // Suspended chords
    {0, 2, 7, 0}, // Suspended 2nd
    {0, 5, 7, 0}, // Suspended 4nd
    {0, 5, 7, 10}, // 7sus4
    // Add chords
    {0, 4, 7, 14}, // Add9
    {0, 3, 7, 14}, // Minor Add9
    // 6th chords
    {0, 4, 7, 9}, // Major 6th
    {0, 3, 7, 9}, // Minor 6th
    // Altered 7th chords
    {0, 4, 6, 10} // Half-Diminished Dominant
};

// Plugin that creates different chords for each note of an octave played
class MultiChord : public Plugin {
  public:
    MultiChord()
        : Plugin(NUM_PARAMS,  // Quantity of parameters
                 NUM_PRESETS, // Quantity of internal presets (enable DISTRHO_PLUGIN_WANT_PROGRAMS)
                 0            // Quantity of internal states
          ) {}

  protected:
    // Get the plugin label. Short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    const char* getLabel() const override { return "MultiChord"; }

    // Get an full description about the plugin.
    const char* getDescription() const override { return "Plugin that creates different chords for each note of the octave"; }

    // Get the plugin author.
    const char* getMaker() const override { return "riban"; }

    // Get the plugin homepage.
    const char* getHomePage() const override { return "https://github.com/riban-bw/lv2-plugins"; }

    // Get the plugin license name (a single line of text).
    const char* getLicense() const override { return "ISC"; }

    // Get the plugin version, in hexadecimal.
    uint32_t getVersion() const override { return d_version(1, 2, 0); }

    // Get the plugin unique Id. Used by LADSPA, DSSI and VST plugin formats.
    int64_t getUniqueId() const override {
        int64_t nValue = ('r' << 24) | ('i' << 16) | ('b' << 8) | ('a' << 0);
        return (nValue << 32) | ('n' << 24) | 1;
    }

    // Inititialise controls and parameters.
    void initPortGroup(const uint32_t groupId, PortGroup& portGroup)
    {
        if (groupId < 12) {
            portGroup.name = String("Offset ") + m_saNoteNames[groupId];
            portGroup.symbol = String("offset_") + m_saNoteNames[groupId];
        } else if (groupId < 24) {
            portGroup.name = String("Velocity ") + m_saNoteNames[groupId - 12];
            portGroup.symbol = String("velocity_") + m_saNoteNames[groupId - 12];
        }
    }

    void initParameter(uint32_t index, Parameter& parameter) override {
        if (index == m_nWet) {
            // Implement wet/dry control
            parameter.name                          = "Wet";
            parameter.symbol                        = "wet";
            parameter.hints                         = kParameterIsAutomatable;
            parameter.ranges.min                    = 0.0f;
            parameter.ranges.max                    = 1.0f;
            parameter.ranges.def                    = 1.0f;
            parameter.enumValues.restrictedMode     = true;
            return;
        } else if (index >= NUM_PARAMS)
            return;
        uint32_t nNote  = index % MAX_NOTES;
        uint32_t nChord = (index / MAX_NOTES) % 12;
        String sName;
        if (index < 12 * MAX_NOTES) {
            // Note map
            sName                                   = String("Offset ") + m_saNoteNames[nChord] + String(nNote + 1);
            parameter.hints                         = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.min                    = -24.0f;
            parameter.ranges.max                    = 24.0f;
            parameter.ranges.def                    = 0.0f;
            parameter.enumValues.count              = 49;
            parameter.enumValues.restrictedMode     = true;
            parameter.groupId                       = nChord;
            ParameterEnumerationValue* const values = new ParameterEnumerationValue[49];
            for (int i = -24; i < 25; ++i) {
                String sPrefix("");
                if (i < -23)
                    sPrefix = "-3";
                else if (i < -11)
                    sPrefix = "-2";
                else if (i < 0)
                    sPrefix = "-";
                else if (i > 23)
                    sPrefix = "+3";
                else if (i > 11)
                    sPrefix = "+2";
                else if (i > 0)
                    sPrefix = "+";
                values[i + 24].label = sPrefix + m_saNoteNames[(nChord + 24 + i) % 12];
                values[i + 24].value = i;
            }
            parameter.enumValues.values = values;
            m_fParamValues[index]       = 0.0f;
        } else {
            // Velocity map
            sName                 = String("Velocity ") + m_saNoteNames[nChord] + String(nNote + 1);
            parameter.hints       = kParameterIsAutomatable;
            parameter.ranges.min  = 0.0f;
            parameter.ranges.max  = 2.0f;
            parameter.ranges.def  = 1.0f;
            m_fParamValues[index] = 1.0f;
            parameter.groupId     = nChord + 12;
        }
        parameter.name   = sName;
        parameter.symbol = sName.replace('#', 's').replace(' ', '_').toLower();
    }

    // Initialise presets.
    void initProgramName(uint32_t index, String& programName) {
        if (index >= NUM_PRESETS)
            return;
        const char* names[] = {"No chord", "Major triad", "Minor triad", "Diminishsed", "Augmented", "Major 7th", "Minor 7th", "Dominant 7th", "Half diminished 7th", "Diminished 7th", "Minor-Major 7th", "Augmented Major 7th", "Augmented 7th", "Suspended 2nd", "Suspended 4nd", "7sus4", "Add9", "Minor Add9", "Major 6th", "Minor 6th", "Half-Diminished Dominant"};
        programName = names[index];
    }

    // Get a value from a control or parameter
    float getParameterValue(uint32_t index) const override {
        if (index < NUM_PARAMS)
            return m_fParamValues[index];
        return 0.0f;
    }

    // Set a control or parameter value
    void setParameterValue(uint32_t index, float value) override {
        if (index < NUM_PARAMS)
            m_fParamValues[index] = value;
    }

    // Load a preset (called program in DPF).
    void loadProgram(uint32_t index) {
        if (index >= NUM_PRESETS)
            return;
        for (int i = 0; i < 12; ++i) {
            for (int j = 0; j < MAX_NOTES; ++j) {
                m_fParamValues[i * MAX_NOTES + j] = CHORDS[index][j];
                m_fParamValues[MAX_NOTES * 12 + i * MAX_NOTES + j] = 1.0f;
            }
        }
        m_fParamValues[m_nWet] = 1.0f;
    }

    // Process audio and MIDI input.
    void run(const float**, float**, uint32_t, const MidiEvent* midiEvents, uint32_t midiEventCount) override {
        int note, velocity;
        for (uint32_t j = 0; j < midiEventCount; ++j) {
            if (midiEvents[j].kDataSize > 2 && (midiEvents[j].data[0] & 0xE0) == 0x80) {
                uint8_t base_note;
                // Note on or note off
                for (int i = 0; i < MAX_NOTES; ++i) {
                    // Iterate through each note of chord
                    base_note  = midiEvents[j].data[1] % 12;
                    int nParam = base_note * MAX_NOTES + i;
                    int offset = m_fParamValues[nParam];
                    if (i > 0 && offset == (int)m_fParamValues[base_note * MAX_NOTES])
                        continue; // Skip if note not configured (same as root note)
                    note = midiEvents[j].data[1] + offset;
                    if (note > 127 || note < 0)
                        continue; // Transposed note is out of range
                    if (i)
                        velocity = m_fParamValues[nParam + 12 * MAX_NOTES] * midiEvents[j].data[2] * m_fParamValues[m_nWet];
                    else
                        velocity = m_fParamValues[nParam + 12 * MAX_NOTES] * midiEvents[j].data[2];
                    if (velocity < 0)
                        //continue; // Don't play extremely quite notes
                    if (velocity > 127)
                        velocity = 127;
                    MidiEvent chordEvent;
                    memcpy(&chordEvent, &midiEvents[j], sizeof(MidiEvent));
                    chordEvent.data[1] = note;
                    chordEvent.data[2] = velocity;
                    writeMidiEvent(chordEvent);
                }
            } else
                writeMidiEvent(midiEvents[j]); // Pass through unprocessed MIDI data
        }
    }

  private:
    float m_fParamValues[NUM_PARAMS];
    uint32_t m_nWet = NUM_PARAMS - 1;

    // Set our plugin class as non-copyable and add a leak detector just in case.
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiChord)
};

// Plugin entry point, called by DPF to create a new plugin instance.
Plugin* createPlugin() { return new MultiChord(); }

END_NAMESPACE_DISTRHO
