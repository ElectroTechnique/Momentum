#ifndef MOMENTUM_VOICE_H
#define MOMENTUM_VOICE_H

#include <stdint.h>
#include <stddef.h>
#include "AudioPatching.h"
#include "Constants.h"
#include "Detune.h"
#include "Velocity.h"

// TODO: Store / update these in the VoiceGroup instead of re-initializing for each noteOn.
struct VoiceParams
{
    float keytrackingAmount;
    float mixerLevel;
    int prevNote;
    float glideSpeed;
    int unisonMode;
    uint32_t chordDetune;
    float detune;
    int oscPitchA;
    int oscPitchB;
};

class Voice
{
private:
    Patch &_oscillator;
    long _timeOn;
    uint8_t _note;
    float _velocity;
    bool _voiceOn;
    uint8_t _idx;
    uint8_t _noteId;
    Mixer *mixer = nullptr;

public:
    Voice(Patch &p, uint8_t i) : _oscillator(p), _timeOn(-1), _note(0), _velocity(0), _voiceOn(false), _idx(i), _noteId(0)
    {
        p.waveformMod_a.frequencyModulation(PITCHLFOOCTAVERANGE);
        p.waveformMod_a.begin(WAVEFORMLEVEL, 440.0f, WAVEFORM_SQUARE);
        p.waveformMod_b.frequencyModulation(PITCHLFOOCTAVERANGE);
        p.waveformMod_b.begin(WAVEFORMLEVEL, 440.0f, WAVEFORM_SQUARE);

        // Arbitary waveform needs initializing to something
        p.waveformMod_a.arbitraryWaveform(PARABOLIC_WAVE, AWFREQ);
        p.waveformMod_b.arbitraryWaveform(PARABOLIC_WAVE, AWFREQ);

        // Gain controls to signal input of the filter from a AudioMixer4 object are limited to +/-1
        //Key tracking needs +/-5, so divide it by 5 and multiply using the octavecontrol
        p.filter_.octaveControl(5.0f);
    }

    inline void setMixer(Mixer *mixer_)
    {
        delete mixer;
        mixer = mixer_;
    }

    inline uint8_t index()
    {
        return this->_idx;
    }

    inline bool on()
    {
        return this->_voiceOn;
    }

    inline uint8_t note()
    {
        return this->_note;
    }

    inline uint8_t noteId()
    {
        return this->_noteId;
    }

    inline uint8_t velocity()
    {
        return this->_velocity;
    }

    inline long timeOn()
    {
        return this->_timeOn;
    }

    inline Patch &patch()
    {
        return this->_oscillator;
    }

    void updateVoice(VoiceParams &params, uint8_t notesOn)
    {
        Patch &osc = this->patch();

        if (params.unisonMode == 1)
        {
            int offset = 2 * this->index();
            osc.waveformMod_a.frequency(NOTEFREQS[this->_note + params.oscPitchA] * tuningCentsFrac * (params.detune + ((1 - params.detune) * DETUNE[notesOn - 1][offset])));
            osc.waveformMod_b.frequency(NOTEFREQS[this->_note + params.oscPitchB] * tuningCentsFrac * (params.detune + ((1 - params.detune) * DETUNE[notesOn - 1][offset + 1])));
        }
        else if (params.unisonMode == 2)
        {
            // TODO: This approach doesn't make sense with voices spread across multiple timbres.
            osc.waveformMod_a.frequency(NOTEFREQS[this->_note + params.oscPitchA + CHORD_DETUNE[this->index()][params.chordDetune]] * tuningCentsFrac);
            osc.waveformMod_b.frequency(NOTEFREQS[this->_note + params.oscPitchB + CHORD_DETUNE[this->index()][params.chordDetune]] * CDT_DETUNE * tuningCentsFrac);
        }
        else
        {
            osc.waveformMod_a.frequency(NOTEFREQS[this->_note + params.oscPitchA] * tuningCentsFrac);
            osc.waveformMod_b.frequency(NOTEFREQS[this->_note + params.oscPitchB] * params.detune * tuningCentsFrac);
        }
    }

    void noteOn(uint8_t note, int velocity, VoiceParams &params, uint8_t notesOn, uint8_t id)
    {
        Patch &osc = this->patch();

        mixer->gain(VELOCITY[currentPatch.VelocitySensitivity][velocity] * params.mixerLevel);
        if (currentPatch.FilterVelocitySensitivity != 0)
        {
            osc.filterVelocity_.amplitude(2.0f * (VELOCITY[currentPatch.FilterVelocitySensitivity][velocity]) - 1.0f); //-1 to 1 (63=0)
        }
        else
        {
            osc.filterVelocity_.amplitude(0.0f);
        }

        // C3 261Hz is the pivot key which is unaffected by keytracking. Divide by 5 as the AudioMixer4 is limited to +/-1 output. OctaveControl on filter multiplies this by 5
        osc.filterModMixer_.gain(2, KEYTRACKINGFACTOR[note] * params.keytrackingAmount / 5.0f); 
        osc.filterEnvelope_.noteOn();
        osc.ampEnvelope_.noteOn();
        if (params.glideSpeed > 0 && note != params.prevNote)
        {
            osc.glide_.amplitude((params.prevNote - note) * DIV24);   // Set glide to previous note frequency (limited to 1 octave max)
            osc.glide_.amplitude(0, params.glideSpeed * GLIDEFACTOR); // Glide to current note
        }

        this->_noteId = id;
        this->_voiceOn = true;
        this->_note = note;
        this->_velocity = velocity;
        this->_timeOn = millis();

        this->updateVoice(params, notesOn);
    }

    void noteOff()
    {
        if (!this->_voiceOn)
            return;
        this->_voiceOn = false;
        this->_noteId = 0;
        this->patch().filterEnvelope_.noteOff();
        this->patch().ampEnvelope_.noteOff();
    }
};

#endif
