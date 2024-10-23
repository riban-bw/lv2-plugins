/* riban CCSend plugin built on DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2024 Brian Walton <brian@riban.co.uk>
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

// -----------------------------------------------------------------------------------------------------------

// Plugin that sends MIDI CC when a control is adjusted
class CCSend : public Plugin {
  public:
    CCSend()
        : Plugin(8, // Quantity of parameters
                 0, // Quantity of internal presets (enable DISTRHO_PLUGIN_WANT_PROGRAMS)
                 0, // Quantity of internal states
          ) {}

  protected:
    // Get the plugin label. Short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    const char* getLabel() const override { return "CCSend"; }

    // Get an full description about the plugin.
    const char* getDescription() const override { return "Plugin that sends MIDI CC commands when a control is adjusted"; }

    // Get the plugin author.
    const char* getMaker() const override { return "riban"; }

    // Get the plugin homepage.
    const char* getHomePage() const override { return "https://github.com/riban-bw/lv2"; }

    // Get the plugin license name (a single line of text).
    const char* getLicense() const override { return "ISC"; }

    // Get the plugin version, in hexadecimal.
    uint32_t getVersion() const override { return d_version(1, 0, 0); }

    // Get the plugin unique Id. Used by LADSPA, DSSI and VST plugin formats.
    int64_t getUniqueId() const override {
        int64_t nValue = ('r' << 24) | ('i' << 16) | ('b' << 8) | ('a' << 0);
        return (nValue << 32) | ('n' << 24) | 2;
    }

    // Inititialise controls and parameters.
    void initParameter(uint32_t index, Parameter& parameter) override {
        if (index < 4) {
            String sName         = String("Send ") + String(index + 1);
            parameter.hints      = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.min = 0;
            parameter.ranges.max = 127;
            parameter.ranges.def = 0;
            parameter.groupId    = 0;
            m_val[index]         = 0;
            m_lastVal[index]     = 0;
            parameter.name       = sName;
            parameter.symbol     = String("send_") + String(index + 1);
        } else if (index < 8) {
            String sName         = String("CC ") + String(index - 3);
            parameter.hints      = kParameterIsInteger;
            parameter.ranges.min = 0;
            parameter.ranges.max = 127;
            parameter.ranges.def = index + 97;
            m_cc[index - 4]      = parameter.ranges.def;
            parameter.name       = sName;
            parameter.symbol     = String("cc_") + String(index - 3);
            parameter.groupId    = 1;
        }
    }

    // Initialise a port group.
    void initPortGroup(uint32_t groupId, PortGroup& portGroup) override {
        switch (groupId) {
        case 0:
            portGroup.name   = String("Send");
            portGroup.symbol = String("send");
            break;
        case 1:
            portGroup.name   = String("CC");
            portGroup.symbol = String("cc");
            break;
        }
    }

    // Get a value from a control or parameter
    float getParameterValue(uint32_t index) const override {
        if (index < 4)
            return m_val[index];
        else if (index < 8)
            return m_cc[index - 4];
        return 0;
    }

    // Set a control or parameter value
    void setParameterValue(uint32_t index, float value) override {
        if (value >= 0.0f && value <= 127.0f) {
            if (index < 4) {
                if (value != m_lastVal[index]) {
                    MidiEvent ccEvent;
                    ccEvent.frame   = 0;
                    ccEvent.size    = 3;
                    ccEvent.data[0] = 0xb0;
                    ccEvent.data[1] = m_cc[index];
                    ccEvent.data[2] = m_val[index];
                    writeMidiEvent(ccEvent);
                    m_val[index]     = value;
                    m_lastVal[index] = m_val[index];
                }
            } else if (index < 8) {
                m_cc[index - 4] = value;
            }
        }
    }

    // Process audio and MIDI input.
    void run(const float**, float**, uint32_t, const MidiEvent* midiEvents, uint32_t midiEventCount) override {
        // Pass all MIDI thru
        for (uint32_t j = 0; j < midiEventCount; ++j)
            writeMidiEvent(midiEvents[j]);
    }

    // -------------------------------------------------------------------------------------------------------

  private:
    uint8_t m_val[4];
    uint8_t m_lastVal[4];
    uint8_t m_cc[4];

    // Set our plugin class as non-copyable and add a leak detector just in case.
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CCSend)
};

// Plugin entry point, called by DPF to create a new plugin instance.
Plugin* createPlugin() { return new CCSend(); }

END_NAMESPACE_DISTRHO
