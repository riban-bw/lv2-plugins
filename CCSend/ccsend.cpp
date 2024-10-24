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

#define NUM_CC 8

// Plugin that sends MIDI CC when a control is adjusted
class CCSend : public Plugin {
  public:
    CCSend()
        : Plugin(NUM_CC * 3 + 5, // Quantity of parameters
                 0,              // Quantity of internal presets (enable DISTRHO_PLUGIN_WANT_PROGRAMS)
                 0               // Quantity of internal states
          ) {}

  protected:
    // Get the plugin label. Short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    const char* getLabel() const override { return "CCSend"; }

    // Get an full description about the plugin.
    const char* getDescription() const override { return "Plugin that sends MIDI CC, PC & BS commands when a control is adjusted"; }

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
        if (index == 0) {
            parameter.hints      = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.min = 1;
            parameter.ranges.max = 16;
            parameter.ranges.def = 1;
            parameter.groupId    = 0;
            parameter.name       = String("Channel");
            parameter.symbol     = String("channel");
        } else if (index == 1) {
            parameter.hints      = kParameterIsAutomatable | kParameterIsInteger;
            parameter.hints      = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.min = 0;
            parameter.ranges.max = 127;
            parameter.ranges.def = 0;
            parameter.groupId    = 0;
            parameter.name       = String("Program");
            parameter.symbol     = String("program");
        } else if (index == 2) {
            parameter.hints      = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.min = 0;
            parameter.ranges.max = 127;
            parameter.ranges.def = 0;
            parameter.groupId    = 0;
            parameter.name       = String("Bank MSB");
            parameter.symbol     = String("bank_msb");
        } else if (index == 3) {
            parameter.hints      = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.min = 0;
            parameter.ranges.max = 127;
            parameter.ranges.def = 0;
            parameter.groupId    = 0;
            parameter.name       = String("Bank LSB");
            parameter.symbol     = String("bank_lsb");
        } else if (index < 4 + NUM_CC) {
            parameter.hints      = kParameterIsAutomatable | kParameterIsInteger;
            parameter.ranges.min = 0;
            parameter.ranges.max = 127;
            parameter.ranges.def = 0;
            parameter.groupId    = 1;
            m_val[index - 4]     = 0;
            parameter.name       = String("Ctrl ") + String(char('A' + index - 4));
            parameter.symbol     = String("send_") + String(char('a' + index - 4));
        } else if (index < 4 + NUM_CC * 2) {
            parameter.hints          = kParameterIsInteger;
            parameter.ranges.min     = 0;
            parameter.ranges.max     = 127;
            parameter.ranges.def     = index - NUM_CC - 4;
            m_cc[index - NUM_CC - 4] = parameter.ranges.def;
            parameter.name           = String(char('A' + index - NUM_CC - 4)) + " CC#";
            parameter.symbol         = String("cc_") + String(char('a' + index - NUM_CC - 4));
            parameter.groupId        = 2;
        } else if (index < 4 + NUM_CC * 3) {
            parameter.hints                         = kParameterIsInteger;
            parameter.ranges.min                    = 0;
            parameter.ranges.max                    = 16;
            parameter.ranges.def                    = 0;
            parameter.enumValues.count              = 17;
            parameter.enumValues.restrictedMode     = true;
            ParameterEnumerationValue* const values = new ParameterEnumerationValue[17];
            values[0].value                         = 0;
            values[0].label                         = "Global";
            for (int i = 1; i < 17; ++i) {
                values[i].value = i;
                values[i].label = String(i);
            }
            parameter.enumValues.values      = values;
            m_ccChan[index - 2 * NUM_CC - 4] = 0;
            parameter.name                   = String(char('A' + index - 2 * NUM_CC - 4)) + " Chan";
            parameter.symbol                 = String("cc_") + String(char('a' + index - 2 * NUM_CC - 4)) + "_chan";
            parameter.groupId                = 2;
        } else if (index == 4 + NUM_CC * 3) {
            parameter.hints      = kParameterIsBoolean;
            parameter.ranges.min = 0;
            parameter.ranges.max = 1;
            parameter.ranges.def = 1;
            m_pc_bs              = 1;
            parameter.name       = "BS + PC";
            parameter.symbol     = "bs_pc";
            parameter.groupId    = 2;
        }
    }

    // Initialise a port group.
    void initPortGroup(uint32_t groupId, PortGroup& portGroup) override {
        switch (groupId) {
        case 0:
            portGroup.name   = String("Program Change");
            portGroup.symbol = String("program");
            break;
        case 1:
            portGroup.name   = String("CC");
            portGroup.symbol = String("send");
            break;
        case 2:
            portGroup.name   = String("Config");
            portGroup.symbol = String("config");
            break;
        }
    }

    // Get a value from a control or parameter
    float getParameterValue(uint32_t index) const override {
        if (index == 0)
            return m_channel;
        else if (index == 1)
            return m_prog;
        else if (index == 2)
            return m_bank_msb;
        else if (index == 3)
            return m_bank_lsb;
        else if (index < 4 + NUM_CC)
            return m_val[index - 4];
        else if (index < 4 + NUM_CC * 2)
            return m_cc[index - NUM_CC - 4];
        else if (index < 4 + 2 * NUM_CC * 2)
            return m_ccChan[index - 2 * NUM_CC - 4];
        else if (index == 4 + 3 * NUM_CC)
            return m_pc_bs;
        return 0;
    }

    // Set a control or parameter value
    void setParameterValue(uint32_t index, float value) override {
        if (index == 0) {
            // Global MIDI channel
            if (value != m_channel && value >= 1 && value <= 16) {
                m_channel = value;
            }
        } else if (index == 1) {
            // Program change
            if (value != m_prog && value >= 0 && value <= 127) {
                MidiEvent event;
                event.frame   = 0;
                event.size    = 2;
                event.data[0] = 0xc0 | (m_channel - 1);
                event.data[1] = value;
                writeMidiEvent(event);
                m_prog = value;
            }
        } else if (index == 2) {
            // Bank MSB select
            if (value != m_bank_msb && value >= 0 && value <= 127) {
                m_bank_msb = value;
                MidiEvent event;
                event.frame   = 0;
                event.size    = 3;
                event.data[0] = 0xb0 | (m_channel - 1);
                event.data[1] = m_bank_lsb;
                event.data[2] = m_bank_msb;
                writeMidiEvent(event);
                if (m_pc_bs) {
                    event.size    = 2;
                    event.data[0] = 0xc0 | (m_channel - 1);
                    event.data[1] = value;
                    writeMidiEvent(event);
                }
            }
        } else if (index == 3) {
            // Bank LSB select
            if (value != m_bank_lsb && value >= 0 && value <= 127) {
                m_bank_lsb = value;
                MidiEvent event;
                event.frame   = 0;
                event.size    = 3;
                event.data[0] = 0xb0 | (m_channel - 1);
                event.data[1] = m_bank_lsb;
                event.data[2] = m_bank_msb;
                writeMidiEvent(event);
                if (m_pc_bs) {
                    event.size    = 2;
                    event.data[0] = 0xc0 | (m_channel - 1);
                    event.data[1] = value;
                    writeMidiEvent(event);
                }
            }
        } else if (index < 4 + NUM_CC) {
            // CC value
            if (value != m_val[index - 4] && value >= 0 && value <= 127) {
                MidiEvent event;
                event.frame = 0;
                event.size  = 3;
                if (m_ccChan[index - 3] == 0)
                    event.data[0] = 0xb0 | (m_channel - 1);
                else
                    event.data[0] = 0xb0 | (m_ccChan[index - 4] - 1);
                event.data[1] = m_cc[index - 4];
                event.data[2] = value;
                writeMidiEvent(event);
                m_val[index - 4] = value;
            }
        } else if (index < 4 + 2 * NUM_CC) {
            // CC number
            if (value != m_cc[index - NUM_CC - 4] && value >= 0 && value <= 127)
                m_cc[index - NUM_CC - 4] = value;
        } else if (index < 4 + 3 * NUM_CC) {
            // CC MIDI channel
            if (value != m_ccChan[index - 2 * NUM_CC - 4] && value >= 0 && value <= 16)
                m_ccChan[index - 2 * NUM_CC - 4] = value;
        } else if (index == 4 + 3 * NUM_CC) {
            m_pc_bs = value ? 1 : 0;
        }
    }

    // Process audio and MIDI input.
    void run(const float**, float**, uint32_t, const MidiEvent* midiEvents, uint32_t midiEventCount) override {
        // Pass all MIDI thru
        for (uint32_t j = 0; j < midiEventCount; ++j)
            writeMidiEvent(midiEvents[j]);
    }

  private:
    uint8_t m_val[NUM_CC];
    uint8_t m_cc[NUM_CC];
    uint8_t m_ccChan[NUM_CC];
    uint8_t m_prog     = 0;
    uint8_t m_bank_lsb = 0;
    uint8_t m_bank_msb = 0;
    uint8_t m_channel  = 1;
    uint8_t m_pc_bs    = 1;

    // Set our plugin class as non-copyable and add a leak detector just in case.
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CCSend)
};

// Plugin entry point, called by DPF to create a new plugin instance.
Plugin* createPlugin() { return new CCSend(); }

END_NAMESPACE_DISTRHO
