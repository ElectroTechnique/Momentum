typedef struct EncoderMappingStruct
{
    boolean active = true;
    uint8_t Parameter = 255;        // MIDI cc (up to 127) or other code up to 255
    uint8_t ShowValue = false;      // Some parameters don't show value like patch no
    uint8_t Value = 0;              // Up to 255
    int8_t Counter = 0;             // For reduced sensitivity for small ranges
    String ValueStr = "";           // For transforming values in strings for display
    uint8_t Range = 127;            // Up to 255
    String ParameterStr = "";       // For display
    boolean Push = false;           // Pushing encoder button for further settings
    State PushAction = State::MAIN; // Next state when button pushed
} EncoderMappingStruct;

EncoderMappingStruct encMap[4] = {};

// Sets the value and value string of the encoder when this parameter is on the display
boolean setEncValue(boolean act, uint8_t parameter, uint8_t value, String str, uint8_t newParameter)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        if (encMap[i].Parameter == parameter)
        {
            if (act != encMap[i].active && state != State::MAIN)
                encMap[i].active = act;
            if (newParameter != encMap[i].Parameter)
                encMap[i].Parameter = newParameter;
            encMap[i].ParameterStr = ParameterStrMap[newParameter];
            if (value != encMap[i].Value)
                encMap[i].Value = value;
            encMap[i].ValueStr = str;
            return true; // True if this was set
            break;
        }
    }
    return false;
}

boolean setEncValue(uint8_t parameter, uint8_t value, String str)
{
    return setEncValue(true, parameter, value, str, parameter);
}

// For turning off
boolean setEncInactive(uint8_t parameter)
{
    return setEncValue(false, parameter, 0, "", parameter);
}

// currentPatch.XXX stores relative values (0-127) like MIDI CC
// groupvec[activeGroupIndex]->XXX stores absolute, real values
FLASHMEM void setEncodersState(State s)
{
    switch (s)
    {
    case State::MAIN:
        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = false;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCbankselectLSB;
        encMap[ENC_BL].ShowValue = false;
        encMap[ENC_BL].Value = currentBankIndex;
        encMap[ENC_BL].Range = BANKS_LIMIT;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCbankselectLSB];
        encMap[ENC_BL].ValueStr = "";
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::PATCHLIST;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = patchselect;
        encMap[ENC_BR].ShowValue = false;
        encMap[ENC_BR].Value = currentPatchIndex;
        encMap[ENC_BR].Range = patches.size();
        encMap[ENC_BR].ValueStr = "";
        encMap[ENC_BR].ParameterStr = ParameterStrMap[patchselect];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::PATCHLIST;
        break;

    case State::PATCHLIST:
        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = cancel;
        encMap[ENC_TR].ShowValue = false;
        encMap[ENC_TR].Value = 0;
        encMap[ENC_TR].Range = 0;
        encMap[ENC_TR].ValueStr = "";
        encMap[ENC_TR].ParameterStr = ParameterStrMap[cancel];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCbankselectLSB;
        encMap[ENC_BL].ShowValue = false;
        encMap[ENC_BL].Value = currentBankIndex;
        encMap[ENC_BL].Range = BANKS_LIMIT;
        encMap[ENC_BL].ValueStr = "";
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCbankselectLSB];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = patchselect;
        encMap[ENC_BR].ShowValue = false;
        encMap[ENC_BR].Value = currentPatchIndex;
        encMap[ENC_BR].Range = patches.size();
        encMap[ENC_BR].ValueStr = "";
        encMap[ENC_BR].ParameterStr = ParameterStrMap[patchselect];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;
    case State::OSCPAGE1:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCoscwaveformA;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.WaveformA;
        encMap[ENC_BL].ValueStr = getWaveformStr(groupvec[activeGroupIndex]->getWaveformA());
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCoscwaveformA];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::OSCPAGE2;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCoscLevelA;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.OscLevelA;
        encMap[ENC_BR].ValueStr = String(groupvec[activeGroupIndex]->getOscLevelA());
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCoscLevelA];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::OSCPAGE2;

        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCdetune;
        encMap[ENC_TL].ShowValue = true;
        if (groupvec[activeGroupIndex]->params().unisonMode == 2)
        {
            encMap[ENC_TL].Value = currentPatch.ChordDetune;
            encMap[ENC_TL].ValueStr = String(CDT_STR[groupvec[activeGroupIndex]->params().chordDetune]);
        }
        else
        {
            encMap[ENC_TL].Value = currentPatch.Detune;
            encMap[ENC_TL].ValueStr = String((1 - groupvec[activeGroupIndex]->params().detune) * 100) + " %";
        }
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCdetune];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::OSCPAGE2;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCpitchA;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.PitchA;
        encMap[ENC_TR].ValueStr = (groupvec[activeGroupIndex]->params().oscPitchA > 0 ? "+" : "") + String(groupvec[activeGroupIndex]->params().oscPitchA);
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCpitchA];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::OSCPAGE2;
        break;
    case State::OSCPAGE2:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCoscwaveformB;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.WaveformB;
        encMap[ENC_BL].ValueStr = getWaveformStr(groupvec[activeGroupIndex]->getWaveformB());
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCoscwaveformB];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::OSCPAGE1;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCoscLevelB;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.OscLevelB;
        encMap[ENC_BR].ValueStr = String(groupvec[activeGroupIndex]->getOscLevelB());
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCoscLevelB];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::OSCPAGE1;

        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCdetune;
        encMap[ENC_TL].ShowValue = true;
        if (groupvec[activeGroupIndex]->params().unisonMode == 2)
        {
            encMap[ENC_TL].Value = currentPatch.ChordDetune;
            encMap[ENC_TL].ValueStr = String(CDT_STR[groupvec[activeGroupIndex]->params().chordDetune]);
        }
        else
        {
            encMap[ENC_TL].Value = currentPatch.Detune;
            encMap[ENC_TL].ValueStr = String((1 - groupvec[activeGroupIndex]->params().detune) * 100) + " %";
        }
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCdetune];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::OSCPAGE1;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCpitchB;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.PitchB;
        encMap[ENC_TR].ValueStr = (groupvec[activeGroupIndex]->params().oscPitchB > 0 ? "+" : "") + String(groupvec[activeGroupIndex]->params().oscPitchB);
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCpitchB];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::OSCPAGE1;
        break;
    case State::OSCPAGE3:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCnoiseLevel;
        encMap[ENC_BL].ShowValue = true;

        if (groupvec[activeGroupIndex]->getPinkNoiseLevel() > 0)
        {
            encMap[ENC_BL].Value = currentPatch.NoiseLevel;
            encMap[ENC_BL].ValueStr = String(groupvec[activeGroupIndex]->getPinkNoiseLevel());
        }
        else if (groupvec[activeGroupIndex]->getWhiteNoiseLevel() > 0)
        {
            encMap[ENC_BL].Value = currentPatch.NoiseLevel;
            encMap[ENC_BL].ValueStr = String(groupvec[activeGroupIndex]->getWhiteNoiseLevel());
        }
        else
        {
            encMap[ENC_BL].Value = 64;
            encMap[ENC_BL].ValueStr = "Off";
        }
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCnoiseLevel];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::OSCPAGE1;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCunison;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.Unison;

        if (groupvec[activeGroupIndex]->params().unisonMode == 0)
        {
            encMap[ENC_BR].ValueStr = "Off";
        }
        else if (groupvec[activeGroupIndex]->params().unisonMode == 1)
        {
            encMap[ENC_BR].ValueStr = "Dynamic";
        }
        else
        {
            encMap[ENC_BR].ValueStr = "Chord";
        }
        encMap[ENC_BR].Range = 2;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCunison];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::OSCPAGE1;

        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCdetune;
        encMap[ENC_TL].ShowValue = true;
        if (groupvec[activeGroupIndex]->params().unisonMode == 2)
        {
            encMap[ENC_TL].Value = currentPatch.ChordDetune;
            encMap[ENC_TL].ValueStr = String(CDT_STR[groupvec[activeGroupIndex]->params().chordDetune]);
        }
        else
        {
            encMap[ENC_TL].Value = currentPatch.Detune;
            encMap[ENC_TL].ValueStr = String((1 - groupvec[activeGroupIndex]->params().detune) * 100) + " %";
        }
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCdetune];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::OSCPAGE1;

        encMap[ENC_TR].active = false;

        break;
    case State::OSCMODPAGE1:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCpwmSourceA;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.PWMSourceA;
        switch (currentPatch.PWMSourceA)
        {
        case PWMSOURCEMANUAL:
            encMap[ENC_BL].ValueStr = "Manual";
            break;
        case PWMSOURCELFO:
            encMap[ENC_BL].ValueStr = "LFO";
            break;
        case PWMSOURCEFENV:
            encMap[ENC_BL].ValueStr = "Filter Env";
            break;
        }
        encMap[ENC_BL].Range = 2;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCpwmSourceA];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::OSCMODPAGE2;

        currentPatch.PWMSourceA != PWMSOURCEMANUAL ? encMap[ENC_BR].active = true : encMap[ENC_BR].active = false;
        encMap[ENC_BR].Parameter = CCpwmAmtA;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.PWMA_Amount;
        encMap[ENC_BR].ValueStr = String(LINEAR[currentPatch.PWMA_Amount]);
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCpwmAmtA];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::OSCMODPAGE2;

        currentPatch.PWMSourceA == PWMSOURCELFO ? encMap[ENC_TL].active = true : encMap[ENC_TL].active = false;
        encMap[ENC_TL].Parameter = CCpwmRateA;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.PWMRateA;
        encMap[ENC_TL].ValueStr = String(groupvec[activeGroupIndex]->getPwmRateA()) + "Hz";
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCpwmRateA];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::OSCMODPAGE2;

        currentPatch.PWMSourceA == PWMSOURCEMANUAL ? encMap[ENC_TR].active = true : encMap[ENC_TR].active = false;
        encMap[ENC_TR].Parameter = CCpwA;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCpwA];
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.PWA_Amount;
        if (groupvec[activeGroupIndex]->getWaveformA() == WAVEFORM_TRIANGLE_VARIABLE)
        {
            encMap[ENC_TR].ValueStr = "Tri " + String(groupvec[activeGroupIndex]->getPwA());
        }
        else
        {
            encMap[ENC_TR].ValueStr = "Pulse " + String(groupvec[activeGroupIndex]->getPwA());
        }
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::OSCMODPAGE2;
        break;

    case State::OSCMODPAGE2:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCpwmSourceB;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.PWMSourceB;
        switch (currentPatch.PWMSourceB)
        {
        case PWMSOURCEMANUAL:
            encMap[ENC_BL].ValueStr = "Manual";
            break;
        case PWMSOURCELFO:
            encMap[ENC_BL].ValueStr = "LFO";
            break;
        case PWMSOURCEFENV:
            encMap[ENC_BL].ValueStr = "Filter Env";
            break;
        }
        encMap[ENC_BL].Range = 2;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCpwmSourceB];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::OSCMODPAGE3;

        currentPatch.PWMSourceB != PWMSOURCEMANUAL ? encMap[ENC_BR].active = true : encMap[ENC_BR].active = false;
        encMap[ENC_BR].Parameter = CCpwmAmtB;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.PWMB_Amount;
        encMap[ENC_BR].ValueStr = String(LINEAR[currentPatch.PWMB_Amount]);
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCpwmAmtB];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::OSCMODPAGE3;

        currentPatch.PWMSourceB == PWMSOURCELFO ? encMap[ENC_TL].active = true : encMap[ENC_TL].active = false;
        encMap[ENC_TL].Parameter = CCpwmRateB;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.PWMRateB;
        encMap[ENC_TL].ValueStr = String(groupvec[activeGroupIndex]->getPwmRateB()) + "Hz";
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCpwmRateB];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::OSCMODPAGE3;

        currentPatch.PWMSourceB == PWMSOURCEMANUAL ? encMap[ENC_TR].active = true : encMap[ENC_TR].active = false;
        encMap[ENC_TR].Parameter = CCpwB;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCpwB];
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.PWB_Amount;
        if (groupvec[activeGroupIndex]->getWaveformB() == WAVEFORM_TRIANGLE_VARIABLE)
        {
            encMap[ENC_TR].ValueStr = "Tri " + String(groupvec[activeGroupIndex]->getPwB());
        }
        else
        {
            encMap[ENC_TR].ValueStr = "Pulse " + String(groupvec[activeGroupIndex]->getPwB());
        }
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::OSCMODPAGE3;
        break;
    case State::OSCMODPAGE3:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCoscLfoWaveform;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.PitchLFOWaveform;
        encMap[ENC_BL].ValueStr = getWaveformStr(groupvec[activeGroupIndex]->getPitchLfoWaveform());
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCoscLfoWaveform];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::OSCMODPAGE4;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCosclfoamt;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.PitchLFOAmt;
        encMap[ENC_BR].ValueStr = String(groupvec[activeGroupIndex]->getPitchLfoAmount());
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCosclfoamt];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::OSCMODPAGE4;

        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCoscLfoRate;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.PitchLFORate;
        encMap[ENC_TL].ValueStr = String(groupvec[activeGroupIndex]->getPitchLfoRate()) + " Hz";
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCoscLfoRate];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::OSCMODPAGE4;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCosclforetrig;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.PitchLFORetrig;
        encMap[ENC_TR].ValueStr = groupvec[activeGroupIndex]->getPitchLfoRetrig() > 0 ? "On" : "Off";
        encMap[ENC_TR].Range = 1;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCosclforetrig];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::OSCMODPAGE4;
        break;

    case State::OSCMODPAGE4:
        currentPatch.OscFX == OSCFXXMOD ? encMap[ENC_BR].active = true : encMap[ENC_BR].active = false;
        encMap[ENC_BR].Parameter = CCoscLevelB;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.OscLevelB;
        encMap[ENC_BR].ValueStr = String(groupvec[activeGroupIndex]->getOscLevelB());
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCoscLevelB];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::OSCPAGE1;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCoscfx;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.OscFX;

        switch (groupvec[activeGroupIndex]->getOscFX())
        {
        case 2:
            encMap[ENC_BL].ValueStr = "X Mod";
            break;
        case 1:
            encMap[ENC_BL].ValueStr = "XOR Mod";
            break;
        default:
            encMap[ENC_BL].ValueStr = "Off";
            break;
        }
        encMap[ENC_BL].Range = 2;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCoscfx];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::OSCMODPAGE1;

        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCpitchenv;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.PitchEnv;
        encMap[ENC_TL].ValueStr = String(groupvec[activeGroupIndex]->getPitchEnvelope());
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCpitchenv];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::OSCMODPAGE1;

        currentPatch.OscFX == OSCFXXMOD ? encMap[ENC_TR].active = true : encMap[ENC_TR].active = false;
        encMap[ENC_TR].Parameter = CCoscLevelA;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.OscLevelA;
        encMap[ENC_TR].ValueStr = String(groupvec[activeGroupIndex]->getOscLevelA());
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCoscLevelA];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::OSCPAGE1;
        break;

    case State::FILTERPAGE1:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = filterfreq256; // 256 values
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.FilterFreq;
        encMap[ENC_BL].ValueStr = String(int(groupvec[activeGroupIndex]->getCutoff())) + " Hz";
        encMap[ENC_BL].Range = 255;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCfilterfreq];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCfilterres;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.FilterRes;
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ValueStr = String(groupvec[activeGroupIndex]->getResonance());
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCfilterres];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;

        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCfiltermixer;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.FilterMixer;
        if (groupvec[activeGroupIndex]->getFilterMixer() == BANDPASS)
        {
            encMap[ENC_TL].ValueStr = "Band Pass";
        }
        else
        {
            // LP-HP mix mode - a notch filter
            if (groupvec[activeGroupIndex]->getFilterMixer() == LOWPASS)
            {
                encMap[ENC_TL].ValueStr = "Low Pass";
            }
            else if (groupvec[activeGroupIndex]->getFilterMixer() == HIGHPASS)
            {
                encMap[ENC_TL].ValueStr = "High Pass";
            }
            else
            {
                encMap[ENC_TL].ValueStr = "Low " + String(100 - int(100 * groupvec[activeGroupIndex]->getFilterMixer())) + " - " + String(int(100 * groupvec[activeGroupIndex]->getFilterMixer())) + " High";
            }
        }
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCfiltermixer];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCfilterenv;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.FilterEnv;
        encMap[ENC_TR].ValueStr = String(groupvec[activeGroupIndex]->getFilterEnvelope());
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCfilterenv];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;
        break;

    case State::FILTERMODPAGE1:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCfilterattack;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.FilterAttack;
        encMap[ENC_TL].ValueStr = milliToString(groupvec[activeGroupIndex]->getFilterAttack());
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCfilterattack];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::FILTERMODPAGE2;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCfilterdecay;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.FilterDecay;
        encMap[ENC_TR].ValueStr = milliToString(groupvec[activeGroupIndex]->getFilterDecay());
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCfilterdecay];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::FILTERMODPAGE2;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCfiltersustain;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.FilterSustain;
        encMap[ENC_BL].ValueStr = String(groupvec[activeGroupIndex]->getFilterSustain());
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCfiltersustain];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::FILTERMODPAGE2;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCfilterrelease;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.FilterRelease;
        encMap[ENC_BR].ValueStr = milliToString(groupvec[activeGroupIndex]->getFilterRelease());
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCfilterrelease];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::FILTERMODPAGE2;
        break;

    case State::FILTERMODPAGE2:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCfilterlfowaveform;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.FilterLFOWaveform;
        encMap[ENC_BL].ValueStr = getWaveformStr(groupvec[activeGroupIndex]->getFilterLfoWaveform());
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCfilterlfowaveform];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::FILTERMODPAGE3;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCfilterlfoamt;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.FilterLfoAmt;
        encMap[ENC_BR].ValueStr = String(groupvec[activeGroupIndex]->getFilterLfoAmt());
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCfilterlfoamt];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::FILTERMODPAGE3;

        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCfilterlforate;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.FilterLFORate;
        encMap[ENC_TL].ValueStr = String(groupvec[activeGroupIndex]->getFilterLfoRate()) + " Hz";
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCfilterlforate];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::FILTERMODPAGE3;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCfilterlforetrig;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.FilterLFORetrig;
        encMap[ENC_TR].ValueStr = groupvec[activeGroupIndex]->getFilterLfoRetrig() > 0 ? "On" : "off";
        encMap[ENC_TR].Range = 1;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCfilterlforetrig];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::FILTERMODPAGE3;
        break;
    case State::FILTERMODPAGE3:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCkeytracking;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.KeyTracking;
        encMap[ENC_BL].ValueStr = String(LINEAR[currentPatch.KeyTracking]);
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCkeytracking];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::FILTERMODPAGE1;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCfilterenv;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.FilterEnv;
        encMap[ENC_BR].ValueStr = String(groupvec[activeGroupIndex]->getFilterEnvelope());
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCfilterenv];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::FILTERMODPAGE1;

        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = filterenvshape;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.FilterEnvShape;
        encMap[ENC_TR].ValueStr = EnvShapeStr[currentPatch.FilterEnvShape];
        encMap[ENC_TR].Range = 17;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[filterenvshape];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        break;
    case State::AMPPAGE1:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCampattack;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.Attack;
        encMap[ENC_TL].ValueStr = milliToString(groupvec[activeGroupIndex]->getAmpAttack());
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCampattack];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCampdecay;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.Decay;
        encMap[ENC_TR].ValueStr = milliToString(groupvec[activeGroupIndex]->getAmpDecay());
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCampdecay];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCampsustain;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.Sustain;
        encMap[ENC_BL].ValueStr = String(groupvec[activeGroupIndex]->getAmpSustain());
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCampsustain];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCamprelease;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.Release;
        encMap[ENC_BR].ValueStr = milliToString(groupvec[activeGroupIndex]->getAmpRelease());
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCamprelease];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;

    case State::AMPPAGE2:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = pitchbendrange;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.PitchBend;
        encMap[ENC_TL].ValueStr = String(currentPatch.PitchBend);
        encMap[ENC_TL].Range = 11;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[pitchbendrange];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = modwheeldepth;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.ModWheelDepth;
        encMap[ENC_TR].ValueStr = String(currentPatch.ModWheelDepth);
        encMap[ENC_TR].Range = 9;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[modwheeldepth];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCvelocitySens;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.VelocitySensitivity;
        encMap[ENC_BL].ValueStr = velocityStr[currentPatch.VelocitySensitivity];
        encMap[ENC_BL].Range = 4;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCvelocitySens];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCmonomode;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.MonophonicMode;
        encMap[ENC_BR].ValueStr = MonophonicStr[currentPatch.MonophonicMode];
        encMap[ENC_BR].Range = 4;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCmonomode];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;

        break;

    case State::AMPPAGE3:
        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = ampenvshape;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.AmpEnvShape;
        encMap[ENC_TR].ValueStr = EnvShapeStr[currentPatch.AmpEnvShape];
        encMap[ENC_TR].Range = 17;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[ampenvshape];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCglide;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.Glide;
        encMap[ENC_BL].ValueStr = milliToString(POWER[currentPatch.Glide] * GLIDEFACTOR);
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCglide];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = glideshape;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.GlideShape;
        encMap[ENC_BR].ValueStr = GlideShapeStr[currentPatch.GlideShape];
        encMap[ENC_BR].Range = 1;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[glideshape];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;

        break;

    case State::FXPAGE:
        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = false;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCfxamt;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.EffectAmt;
        encMap[ENC_BL].ValueStr = String(ENSEMBLE_LFO[currentPatch.EffectAmt]) + " Hz";
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCfxamt];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCfxmix;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.EffectMix;
        encMap[ENC_BR].ValueStr = String(groupvec[activeGroupIndex]->getEffectMix());
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCfxmix];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;

    case State::SAVE:
        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = false;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = savepatch;
        encMap[ENC_BL].ShowValue = false;
        encMap[ENC_BL].Value = 0;
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[savepatch];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = renamepatch;
        encMap[ENC_BR].ShowValue = false;
        encMap[ENC_BL].Value = 0;
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[renamepatch];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;

    case State::PATCHSAVING:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = editbank;
        encMap[ENC_TL].ShowValue = false;
        encMap[ENC_TL].Value = 0;
        encMap[ENC_TL].Range = 0;
        encMap[ENC_TL].ValueStr = "";
        encMap[ENC_TL].ParameterStr = ParameterStrMap[editbank];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::EDITBANK;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = cancel;
        encMap[ENC_TR].ShowValue = false;
        encMap[ENC_TR].Value = 0;
        encMap[ENC_TR].Range = 0;
        encMap[ENC_TR].ValueStr = "";
        encMap[ENC_TR].ParameterStr = ParameterStrMap[cancel];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = savebankselect;
        encMap[ENC_BL].ShowValue = false;
        encMap[ENC_BL].Value = currentBankIndex;
        encMap[ENC_BL].Range = BANKS_LIMIT;
        encMap[ENC_BL].ValueStr = "";
        encMap[ENC_BL].ParameterStr = ParameterStrMap[savebankselect];
        encMap[ENC_BL].Push = false;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = savepatchselect;
        encMap[ENC_BR].ShowValue = false;
        encMap[ENC_BR].Value = currentPatchIndex;
        encMap[ENC_BR].Range = patches.size();
        encMap[ENC_BR].ValueStr = "";
        encMap[ENC_BR].ParameterStr = ParameterStrMap[savepatchselect];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::RENAMEPATCH;
        break;

    case State::RENAMEPATCH:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = deleteCharacterPatch;
        encMap[ENC_TL].ShowValue = false;
        encMap[ENC_TL].Value = 0;
        encMap[ENC_TL].Range = 0;
        encMap[ENC_TL].ValueStr = "";
        encMap[ENC_TL].ParameterStr = ParameterStrMap[deleteCharacterPatch];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::DELETECHARPATCH;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = cancel;
        encMap[ENC_TR].ShowValue = false;
        encMap[ENC_TR].Value = 0;
        encMap[ENC_TR].Range = 0;
        encMap[ENC_TR].ValueStr = "";
        encMap[ENC_TR].ParameterStr = ParameterStrMap[cancel];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = choosecharacterPatch;
        encMap[ENC_BL].ShowValue = false;
        encMap[ENC_BL].Value = 0;
        encMap[ENC_BL].Range = TOTALCHARS;
        encMap[ENC_BL].ValueStr = "";
        encMap[ENC_BL].ParameterStr = ParameterStrMap[choosecharacterPatch];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::CHOOSECHARPATCH;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = savepatch;
        encMap[ENC_BR].ShowValue = false;
        encMap[ENC_BR].ValueStr = "";
        encMap[ENC_BR].ParameterStr = ParameterStrMap[savepatch];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;

    case State::DELETEPATCH:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = deletepatch;
        encMap[ENC_TL].ShowValue = false;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[deletepatch];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = cancel;
        encMap[ENC_TR].ShowValue = false;
        encMap[ENC_TR].Value = 0;
        encMap[ENC_TR].Range = 0;
        encMap[ENC_TR].ValueStr = "";
        encMap[ENC_TR].ParameterStr = ParameterStrMap[cancel];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCbankselectLSB;
        encMap[ENC_BL].ShowValue = false;
        encMap[ENC_BL].Value = currentBankIndex;
        encMap[ENC_BL].Range = BANKS_LIMIT;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCbankselectLSB];
        encMap[ENC_BL].ValueStr = "";
        encMap[ENC_BL].Push = false;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = patchselect;
        encMap[ENC_BR].ShowValue = false;
        encMap[ENC_BR].Value = currentPatchIndex;
        encMap[ENC_BR].Range = patches.size();
        encMap[ENC_BR].ValueStr = "";
        encMap[ENC_BR].ParameterStr = ParameterStrMap[patchselect];
        encMap[ENC_BR].Push = false;
        break;

    case State::MIDIPAGE:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = MIDIThruMode;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = MIDIThru;
        encMap[ENC_TL].Range = 3;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[MIDIThruMode];
        encMap[ENC_TL].ValueStr = MIDIThruStr[MIDIThru];
        encMap[ENC_TL].Push = false;

        encMap[ENC_TR].active = false;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = MIDIChIn;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = midiChannel;
        encMap[ENC_BL].Range = 16;
        if (midiChannel == 0)
        {
            encMap[ENC_BL].ValueStr = "All";
        }
        else
        {
            encMap[ENC_BL].ValueStr = String(midiChannel);
        }
        encMap[ENC_BL].ParameterStr = ParameterStrMap[MIDIChIn];
        encMap[ENC_BL].Push = false;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = MIDIChOut;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = midiOutCh;
        encMap[ENC_BR].Range = 16;
        if (midiOutCh == 0)
        {
            encMap[ENC_BR].ValueStr = "Off";
        }
        else
        {
            encMap[ENC_BR].ValueStr = String(midiOutCh);
        }
        encMap[ENC_BR].ParameterStr = ParameterStrMap[MIDIChOut];
        encMap[ENC_BR].Push = false;
        break;

    case State::PERFORMANCEPAGE:
        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = false;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = PerfSelect;
        encMap[ENC_BL].ShowValue = false;
        encMap[ENC_BL].Value = 0;
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[PerfSelect];
        encMap[ENC_BL].ValueStr = String(midiChannel);
        encMap[ENC_BL].Push = false;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = PerfEdit;
        encMap[ENC_BR].ShowValue = false;
        encMap[ENC_BR].ValueStr = "";
        encMap[ENC_BR].ParameterStr = ParameterStrMap[PerfEdit];
        encMap[ENC_BR].Push = true;
        break;

    case State::SETTINGS:
        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = false;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = settingoption;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = 0;
        encMap[ENC_BL].Range = 3;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[settingoption];
        encMap[ENC_BL].ValueStr = settings::current_setting();
        encMap[ENC_BL].Push = false;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = settingvalue;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = settings::index();
        encMap[ENC_BR].Range = SETTINGSVALUESNO - 1;
        encMap[ENC_BR].ValueStr = settings::current_setting_value();
        encMap[ENC_BR].ParameterStr = ParameterStrMap[settingvalue];
        encMap[ENC_BR].Push = false;
        break;

    case State::ARPPAGE:
        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = false;

        encMap[ENC_BL].active = false;

        encMap[ENC_BR].active = false;
        break;

    case State::SEQPAGE:
        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = false;

        encMap[ENC_BL].active = false;

        encMap[ENC_BR].active = false;
        break;

    case State::EDITBANK:
        encMap[ENC_TL].active = false;
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = deletebank;
        encMap[ENC_TL].ShowValue = false;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[deletebank];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = cancel;
        encMap[ENC_TR].ShowValue = false;
        encMap[ENC_TR].Value = 0;
        encMap[ENC_TR].Range = 0;
        encMap[ENC_TR].ValueStr = "";
        encMap[ENC_TR].ParameterStr = ParameterStrMap[cancel];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = bankeditselect;
        encMap[ENC_BL].ShowValue = false;
        encMap[ENC_BL].Range = BANKS_LIMIT;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[bankeditselect];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = renamebank;
        encMap[ENC_BR].ShowValue = false;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[renamebank];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::RENAMEBANK;
        break;

    case State::RENAMEBANK:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = deleteCharacterBank;
        encMap[ENC_TL].ShowValue = false;
        encMap[ENC_TL].Value = 0;
        encMap[ENC_TL].Range = 0;
        encMap[ENC_TL].ValueStr = "";
        encMap[ENC_TL].ParameterStr = ParameterStrMap[deleteCharacterBank];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::DELETECHARBANK;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = cancel;
        encMap[ENC_TR].ShowValue = false;
        encMap[ENC_TR].Value = 0;
        encMap[ENC_TR].Range = 0;
        encMap[ENC_TR].ValueStr = "";
        encMap[ENC_TR].ParameterStr = ParameterStrMap[cancel];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = choosecharacterBank;
        encMap[ENC_BL].ShowValue = false;
        encMap[ENC_BL].Value = 0;
        encMap[ENC_BL].Range = TOTALCHARS;
        encMap[ENC_BL].ValueStr = "";
        encMap[ENC_BL].ParameterStr = ParameterStrMap[choosecharacterBank];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::CHOOSECHARBANK;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = savebank;
        encMap[ENC_BR].ShowValue = false;
        encMap[ENC_BR].ValueStr = "";
        encMap[ENC_BR].ParameterStr = ParameterStrMap[savebank];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;
    default:
        break;
    }
}