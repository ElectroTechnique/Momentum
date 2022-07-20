#include <Arduino.h>
#include "Constants.h"
#include "MidiCC.h"

const char *ParameterStrMap[256] = {};

FLASHMEM void assignStrings()
{
    ParameterStrMap[CCosclfoamt] = "Pitch LFO Amount";
    ParameterStrMap[CCglide] = "Glide Time";
    ParameterStrMap[CCvolume] = "Volume";
    ParameterStrMap[CCoscwaveformA] = "1 Waveform";
    ParameterStrMap[CCoscwaveformB] = "2 Waveform";
    ParameterStrMap[CCfilterenv] = "Filter Envelope";
    ParameterStrMap[CCfiltermixer] = "Filter Type";
    ParameterStrMap[CCoscLevelA] = "Osc1 Level";
    ParameterStrMap[CCoscLevelB] = "Osc2 Level";
    ParameterStrMap[CCnoiseLevel] = "Noise Level";
    ParameterStrMap[CCoscfx] = "Osc Effect";
    ParameterStrMap[CCpitchA] = "1 Pitch";
    ParameterStrMap[CCpitchB] = "2 Pitch";
    ParameterStrMap[CCpitchenv] = "Pitch Envelope";
    ParameterStrMap[CCosclforetrig] = "Pitch LFO Retrigger";
    ParameterStrMap[CCfilterlforetrig] = "Filter LFO Retrigger";
    ParameterStrMap[CCfilterres] = "Resonance";
    ParameterStrMap[CCamprelease] = "Release";
    ParameterStrMap[CCampattack] = "Attack";
    ParameterStrMap[CCfilterfreq] = "Cutoff Frequency";
    ParameterStrMap[filterfreq256] = "Cutoff Frequency";
    ParameterStrMap[CCampdecay] = "Decay";
    ParameterStrMap[CCfilterlforate] = "Filter LFO Rate";
    ParameterStrMap[CCfilterlfoamt] = "Filter LFO Level";
    ParameterStrMap[CCampsustain] = "Sustain";
    ParameterStrMap[CCfilterattack] = "Filter Attack";
    ParameterStrMap[CCfilterdecay] = "Filter Decay";
    ParameterStrMap[CCfiltersustain] = "Filter Sustain";
    ParameterStrMap[CCfilterrelease] = "Filter Release";
    ParameterStrMap[CCpwA] = "1 Pulse Width";
    ParameterStrMap[CCpwB] = "2 Pulse Width";
    ParameterStrMap[CCpwmRateA] = "1 PWM LFO Rate";
    ParameterStrMap[CCpwmRateB] = "2 PWM LFO Rate";
    ParameterStrMap[CCpwmAmtA] = "1 PWM Level";
    ParameterStrMap[CCpwmAmtB] = "2 PWM Level";
    ParameterStrMap[CCkeytracking] = "Key Tracking";
    ParameterStrMap[CCfilterlfowaveform] = "Filter LFO Waveform";
    ParameterStrMap[CCfxamt] = "Effects Amount";
    ParameterStrMap[CCfxmix] = "Effects Mix";
    ParameterStrMap[CCdetune] = "Detune";
    ParameterStrMap[CCoscLfoRate] = "Pitch LFO Rate";
    ParameterStrMap[CCoscLfoWaveform] = "Pitch LFO Waveform";
    ParameterStrMap[CCfilterLFOMidiClkSync] = "Filter LFO MIDI Clk Sync";
    ParameterStrMap[CCoscLFOMidiClkSync] = "Pitch LFO MIDI Clk Sync";
    ParameterStrMap[CCpwmSourceA] = "1 PWM Source";
    ParameterStrMap[CCpwmSourceB] = "2 PWM Source";
    ParameterStrMap[CCunison] = "Unison";
    ParameterStrMap[CCmonomode] = "Monophonic";
    ParameterStrMap[patchselect] = "Patch";
    ParameterStrMap[CCbankselectLSB] = "Bank";
    ParameterStrMap[savepatch] = "Save";
    ParameterStrMap[deletepatch] = "Delete";
    ParameterStrMap[renamepatch] = "Rename";
    ParameterStrMap[temposync] = "Tempo Sync";
    ParameterStrMap[CCvelocitySens] = "Velocity Sensitivity";
}

FLASHMEM String getWaveformStr(int value)
{
    switch (value)
    {
    case WAVEFORM_SILENT:
        return F("Off");
    case WAVEFORM_SAMPLE_HOLD:
        return F("Sample & Hold");
    case WAVEFORM_SINE:
        return F("Sine");
    case WAVEFORM_BANDLIMIT_SQUARE:
    case WAVEFORM_SQUARE:
        return F("Square");
    case WAVEFORM_TRIANGLE:
        return F("Triangle");
    case WAVEFORM_BANDLIMIT_SAWTOOTH:
    case WAVEFORM_SAWTOOTH:
        return F("Sawtooth");
    case WAVEFORM_SAWTOOTH_REVERSE:
        return F("Ramp");
    case WAVEFORM_BANDLIMIT_PULSE:
        return F("Variable Pulse");
    case WAVEFORM_TRIANGLE_VARIABLE:
        return F("Variable Triangle");
    case WAVEFORM_PARABOLIC:
        return F("Parabolic");
    case WAVEFORM_HARMONIC:
        return F("Harmonic");
    default:
        return F("ERR_WAVE");
    }
}

const char *CDT_STR[128] = {"Major", "Major", "Major", "Major", "Major", "Major", "Major", "Minor", "Minor", "Minor", "Minor", "Minor", "Minor", "Minor", "Diminished", "Diminished", "Diminished", "Diminished", "Diminished", "Diminished", "Diminished", "Augmented", "Augmented", "Augmented", "Augmented", "Augmented", "Augmented", "Augmented", "Sus 2nd", "Sus 2nd", "Sus 2nd", "Sus 2nd", "Sus 2nd", "Sus 2nd", "Sus 2nd", "Sus 4th", "Sus 4th", "Sus 4th", "Sus 4th", "Sus 4th", "Sus 4th", "Sus 4th", "7th Sus 2nd", "7th Sus 2nd", "7th Sus 2nd", "7th Sus 2nd", "7th Sus 2nd", "7th Sus 2nd", "7th Sus 2nd", "7th Sus 4th", "7th Sus 4th", "7th Sus 4th", "7th Sus 4th", "7th Sus 4th", "7th Sus 4th", "7th Sus 4th", "6th", "6th", "6th", "6th", "6th", "6th", "6th", "7th", "7th", "7th", "7th", "7th", "7th", "7th", "9th", "9th", "9th", "9th", "9th", "9th", "9th", "Major 7th", "Major 7th", "Major 7th", "Major 7th", "Major 7th", "Major 7th", "Major 7th", "Major 9th", "Major 9th", "Major 9th", "Major 9th", "Major 9th", "Major 9th", "Major 9th", "Major 11th", "Major 11th", "Major 11th", "Major 11th", "Major 11th", "Major 11th", "Major 11th", "Minor 6th", "Minor 6th", "Minor 6th", "Minor 6th", "Minor 6th", "Minor 6th", "Minor 6th", "Minor 7th", "Minor 7th", "Minor 7th", "Minor 7th", "Minor 7th", "Minor 7th", "Minor 7th", "Minor 9th", "Minor 9th", "Minor 9th", "Minor 9th", "Minor 9th", "Minor 9th", "Minor 9th", "Minor 11th", "Minor 11th", "Minor 11th", "Minor 11th", "Minor 11th", "Minor 11th", "Minor 11th", "All 12", "All 12"};
