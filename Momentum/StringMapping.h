#include <Arduino.h>
#include "MidiCC.h"

String StrMap[256] = {};

FLASHMEM void assignStrings()
{
    StrMap[CCosclfoamt] = "Pitch LFO Amount";
    StrMap[CCglide] = "Glide Time";
    StrMap[CCvolume] = "Volume";
    StrMap[CCoscwaveformA] = "Osc1 Waveform";
    StrMap[CCoscwaveformB] = "Osc2 Waveform";
    StrMap[CCfilterenv] = "Filter Envelope";
    StrMap[CCfiltermixer] = "Filter Type";
    StrMap[CCoscLevelA] = "Osc1 Level";
    StrMap[CCoscLevelB] = "Osc2 Level";
    StrMap[CCnoiseLevel] = "Noise Level";
    StrMap[CCoscfx] = "Osc Effects";
    StrMap[CCpitchA] = "Osc1 Pitch";
    StrMap[CCpitchB] = "Osc2 Pitch";
    StrMap[CCpitchenv] = "Pitch Envelope";
    StrMap[CCosclforetrig] = "Pitch LFO Retrigger";
    StrMap[CCfilterlforetrig] = "Filter LFO Retrigger";
    StrMap[CCfilterres] = "Resonance";
    StrMap[CCamprelease] = "Release";
    StrMap[CCampattack] = "Attack";
    StrMap[CCfilterfreq] = "Cutoff Frequency";
    StrMap[CCampdecay] = "Decay";
    StrMap[CCfilterlforate] = "Filter LFO Rate";
    StrMap[CCfilterlfoamt] = "Filter LFO Level";
    StrMap[CCampsustain] = "Sustain";
    StrMap[CCfilterattack] = "Filter Attack";
    StrMap[CCfilterdecay] = "Filter Decay";
    StrMap[CCfiltersustain] = "Filter Sustain";
    StrMap[CCfilterrelease] = "Filter Release";
    StrMap[CCpwA] = "Osc1 Pulse Width";
    StrMap[CCpwB] = "Osc2 Pulse Width";
    StrMap[CCpwmRate] = "PWM LFO Rate";
    StrMap[CCpwmAmt] = "PWM Level";
    StrMap[CCkeytracking] = "Key Tracking";
    StrMap[CCfilterlfowaveform] = "Filter LFO Waveform";
    StrMap[CCfxamt] = "Effects Amount";
    StrMap[CCfxmix] = "Effects Mix";
    StrMap[CCdetune] = "Detune";
    StrMap[CCpwmAmtA] = "Osc1 PWM Level";
    StrMap[CCpwmAmtB] = "Osc2 PWM Level";
    StrMap[CCoscLfoRate] = "Pitch LFO Rate";
    StrMap[CCoscLfoWaveform] = "Pitch LFO Waveform";
    StrMap[CCfilterLFOMidiClkSync] = "Filter LFO MIDI Clk Sync";
    StrMap[CCoscLFOMidiClkSync] = "Pitch LFO MIDI Clk Sync";
    StrMap[CCpwmSource] = "PWM Source";
    StrMap[CCunison] = "Unison";
    StrMap[CCmonomode] = "Monophonic";
    StrMap[patchselect] = "Patch";
    StrMap[CCbankselectLSB] = "Bank";
}