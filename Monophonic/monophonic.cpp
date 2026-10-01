/* riban Monophonic plugin built on DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2026 Brian Walton <brian@riban.co.uk>
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

#include "../common.h"
#include <deque>

#define VER_MAJOR 1
#define VER_MINOR 1
#define VER_BUILD 0

#define CC_HOLD 64
#define MAX_N_TRIG 5

enum PARAMS {
    PARAM_PRIORITY,
    PARAM_HOLD_MODE,
    PARAM_N_TRIG,
    NUM_PARAMS
};

enum MODES {
    PRIORITY_BYPASS,
    PRIORITY_HIGH,
    PRIORITY_LOW,
    PRIORITY_LAST,
    PRIORITY_FIRST,
    NUM_PRIORITIES
};

enum HOLD_MODES {
    HOLD_MODE_RESET,
    HOLD_MODE_CONT,
    NUM_HOLD_MODES
};

START_NAMESPACE_DISTRHO

// Plugin that limits MIDI to monophonic
class Monophonic : public Plugin {
  public:
    Monophonic()
        : Plugin(NUM_PARAMS, // Quantity of parameters
                 0, // Quantity of internal presets (enable DISTRHO_PLUGIN_WANT_PROGRAMS)
                 0  // Quantity of internal states
          ) {
            for (uint8_t chan = 0; chan < 16; ++chan) {
                m_anHold[chan] = 255;
                for (uint8_t note = 0; note < 128; ++note)
                    m_anVel[chan][note] = 0;
            }
          }

  protected:
    // Get the plugin label. Short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    const char* getLabel() const override { return "Monophonic"; }

    // Get an full description about the plugin.
    const char* getDescription() const override { return "Plugin that limits MIDI to monophonic"; }

    // Get the plugin author.
    const char* getMaker() const override { return "riban"; }

    // Get the plugin homepage.
    const char* getHomePage() const override { return "https://github.com/riban-bw/lv2-plugins"; }

    // Get the plugin license name (a single line of text).
    const char* getLicense() const override { return "ISC"; }

    // Get the plugin version, in hexadecimal.
    uint32_t getVersion() const override { return d_version(VER_MAJOR, VER_MINOR, VER_BUILD); }

    // Get the plugin unique Id. Used by LADSPA, DSSI and VST plugin formats.
    int64_t getUniqueId() const override {
        int64_t nValue = ('r' << 24) | ('i' << 16) | ('b' << 8) | ('a' << 0);
        return (nValue << 32) | ('n' << 24) | ID_MONOPHONIC;
    }

    void initParameter(uint32_t index, Parameter& parameter) override {
        switch(index) {
        case PARAM_PRIORITY: {
            parameter.name                          = String("Priority");
            parameter.symbol                        = String("priority");
            parameter.hints                         = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.def                    = 1;
            parameter.enumValues.count              = NUM_PRIORITIES;
            parameter.enumValues.restrictedMode     = true;
            ParameterEnumerationValue* const values = new ParameterEnumerationValue[NUM_PRIORITIES];
            values[0].label = "Bypass";
            values[0].value = 0;
            values[1].label = "Highest";
            values[1].value = 1;
            values[2].label = "Lowest";
            values[2].value = 2;
            values[3].label = "Last";
            values[3].value = 3;
            values[4].label = "First";
            values[4].value = 4;
            parameter.enumValues.values = values;
            break;
        }
        case PARAM_HOLD_MODE: {
            parameter.name                          = String("Hold Mode");
            parameter.symbol                        = String("holdmode");
            parameter.hints                         = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.def                    = 0;
            parameter.enumValues.count              = NUM_HOLD_MODES;
            parameter.enumValues.restrictedMode     = true;
            ParameterEnumerationValue* const values = new ParameterEnumerationValue[NUM_PRIORITIES];
            values[0].label                         = "Reset";
            values[0].value                         = HOLD_MODE_RESET;
            values[1].label                         = "Cont";
            values[1].value                         = HOLD_MODE_CONT;
            parameter.enumValues.values             = values;
            break;
        }
        case PARAM_N_TRIG:
            parameter.name                          = String("n-Trig");
            parameter.symbol                        = String("ntrig");
            parameter.hints                         = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.def                    = 1;
            parameter.ranges.min                    = 1;
            parameter.ranges.max                    = MAX_N_TRIG;
            break;
        }
    }

    // Get a value from a control or parameter
    float getParameterValue(uint32_t index) const override {
        switch (index) {
        case PARAM_PRIORITY:
            return m_nPriority;
        case PARAM_HOLD_MODE:
            return m_nHoldMode;
        case PARAM_N_TRIG:
            return m_nTrigMode + 1;
        }
        return 0;
    }

    // Set a control or parameter value
    void setParameterValue(uint32_t index, float value) override {
        switch (index) {
        case PARAM_PRIORITY:
            if (value < NUM_PRIORITIES)
                m_nPriority = value;
            break;
        case PARAM_HOLD_MODE:
            if (value < NUM_HOLD_MODES)
                m_nHoldMode = value;
            break;
        case PARAM_N_TRIG:
            if (value > 0 && value <= MAX_N_TRIG)
                m_nTrigMode = value - 1;
            break;
        }
    }

    // Process audio and MIDI input.
    void run(const float**, float**, uint32_t, const MidiEvent* midiEvents, uint32_t midiEventCount) override {
        for (uint32_t j = 0; j < midiEventCount; ++j) {
            if (m_nPriority != PRIORITY_BYPASS && midiEvents[j].kDataSize > 2) {
                uint8_t chan = midiEvents[j].data[0] & 0x0f;
                if (((midiEvents[j].data[0] & 0xF0) == 0xB0) && midiEvents[j].data[1] == CC_HOLD) {
                    // Hold pedal
                    if (midiEvents[j].data[2] > 63) {
                        // Hold pressed
                        if (m_qNotes[chan].size() > m_nTrigMode)
                            m_anHold[chan] = m_qNotes[chan][m_nTrigMode];
                        else
                            m_anHold[chan] = 254;
                    } else {
                        if (m_anHold[chan] < 128 && m_qNotes[chan].size() <= m_nTrigMode) {
                            // Send note off for held note
                            MidiEvent event;
                            event.size = 3;
                            event.frame = midiEvents[j].frame;
                            event.data[0] = 0x90 | chan;
                            event.data[1] = m_anHold[chan];
                            event.data[2] = 0;
                            writeMidiEvent(event);
                        }
                        m_anHold[chan] = 255;
                    }
                } else if ((midiEvents[j].data[0] & 0xE0) == 0x80) {
                    // Note on or off
                    uint8_t note = midiEvents[j].data[1];
                    uint8_t vel = midiEvents[j].data[2];
                    uint8_t nNextNote = 255;
                    uint8_t nPrevNote = 255;
                    bool bHold = m_anHold[chan] != 255;
                    if (m_nHoldMode == HOLD_MODE_CONT && m_anHold[chan] < 128)
                        nPrevNote = m_anHold[chan];
                    else if (m_qNotes[chan].size() > m_nTrigMode)
                        nPrevNote = m_qNotes[chan][m_nTrigMode];
                    else if (bHold)
                        nPrevNote = m_anHold[chan];

                    if (((midiEvents[j].data[0] & 0xf0) == 0x90) && vel) {
                        // Note on
                        m_anVel[chan][note] = vel;
                        if (std::find(m_qNotes[chan].begin(), m_qNotes[chan].end(), note) == m_qNotes[chan].end()) {
                            // Note is not in the queue
                            switch (m_nPriority) {
                                case PRIORITY_LAST:
                                    m_qNotes[chan].push_front(note);
                                    break;
                                case PRIORITY_FIRST:
                                    m_qNotes[chan].push_back(note);
                                    break;
                                case PRIORITY_LOW:
                                    m_qNotes[chan].push_back(note);
                                    std::sort(m_qNotes[chan].begin(), m_qNotes[chan].end());
                                    break;
                                case PRIORITY_HIGH:
                                    m_qNotes[chan].push_back(note);
                                    std::sort(m_qNotes[chan].begin(), m_qNotes[chan].end(), std::greater<uint8_t>());
                                    break;
                            }
                        }
                    } else {
                        // Note off
                        auto it = std::find(m_qNotes[chan].begin(), m_qNotes[chan].end(), note);
                        if (it != m_qNotes[chan].end())
                            m_qNotes[chan].erase(it);
                    }
                    if (m_qNotes[chan].size() > m_nTrigMode)
                        nNextNote = m_qNotes[chan][m_nTrigMode];
                    if (bHold && m_nHoldMode == HOLD_MODE_CONT && m_anHold[chan] < 128) {
                        if (m_nPriority == PRIORITY_HIGH && m_anHold[chan] > nNextNote)
                            nNextNote = m_anHold[chan];
                        else if (m_nPriority == PRIORITY_LOW && m_anHold[chan] < nNextNote)
                            nNextNote = m_anHold[chan];
                    }

                    if (nNextNote != nPrevNote) {
                        if (nPrevNote < 128 && (!bHold || nNextNote != 255)) {
                            if (bHold && m_anHold[chan] < 128)
                                nPrevNote = m_anHold[chan];
                            // Silence previous sounding note
                            MidiEvent event;
                            event.size = 3;
                            event.frame = midiEvents[j].frame;
                            event.data[0] = 0x90 | chan;
                            event.data[1] = nPrevNote;
                            event.data[2] = 0;
                            writeMidiEvent(event);
                            if (m_anHold[chan] != 255)
                                m_anHold[chan] = 254;
                        }
                        if (nNextNote != 255) {
                            // Play next note
                            MidiEvent event;
                            event.size = 3;
                            event.frame = midiEvents[j].frame;
                            event.data[0] = 0x90 | chan;
                            event.data[1] = nNextNote;
                            event.data[2] = m_anVel[chan][nNextNote];
                            writeMidiEvent(event);
                            if (m_anHold[chan] != 255)
                                m_anHold[chan] = nNextNote;
                        }
                    }
                }
            } else {
                writeMidiEvent(midiEvents[j]); // Pass through unprocessed MIDI data
            }
        }
    }

  private:
    uint8_t m_anHold[16]; // Note number of held note. 255 if not held. 254 if hold pressed but no notes played.
    uint8_t m_nPriority = PRIORITY_HIGH; // Monophonic note priority
    uint8_t m_nHoldMode = HOLD_MODE_RESET;
    uint8_t m_nTrigMode = 0;
    uint8_t m_anVel[16][128]; // Store last received note-on velocity
    std::deque<uint8_t> m_qNotes[16]; // Ordered queue of held notes

    // Set our plugin class as non-copyable and add a leak detector just in case.
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Monophonic)
};

// Plugin entry point, called by DPF to create a new plugin instance.
Plugin* createPlugin() { return new Monophonic(); }

END_NAMESPACE_DISTRHO
