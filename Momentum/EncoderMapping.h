typedef struct EncoderMappingStruct
{
    boolean active = true;
    uint8_t Parameter = 0;          // MIDI cc (up to 127) or other code up to 255
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

void setEncValueStr(uint8_t parameter, String str)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        if (encMap[i].Parameter == parameter)
        {
            encMap[i].ValueStr = str;
            break;
        }
    }
}

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

        encMap[ENC_TR].active = false;

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
        encMap[ENC_TL].Value = currentPatch.Detune;
        // if (groupvec[activeGroupIndex]->params().unisonMode == 2)
        // {
        //     encMap[ENC_TL].ValueStr = "Chord " + String(CDT_STR[groupvec[activeGroupIndex]->params().chordDetune]);
        // }
        // else
        // {
        //     encMap[ENC_TL].ValueStr = String(ParameterStrMap[CCdetune]) + String((1 - groupvec[activeGroupIndex]->params().detune) * 100) + " %"));
        // }
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCdetune];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::OSCPAGE2;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCpitchA;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.PitchA;
        encMap[ENC_TR].ValueStr = groupvec[activeGroupIndex]->params().oscPitchA > 0 ? "+" : "" + String(groupvec[activeGroupIndex]->params().oscPitchA);
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
        encMap[ENC_TL].Value = currentPatch.Detune;
        // if (groupvec[activeGroupIndex]->params().unisonMode == 2)
        // {
        //     encMap[ENC_TL].ValueStr = "Chord " + String(CDT_STR[groupvec[activeGroupIndex]->params().chordDetune]);
        // }
        // else
        // {
        //     encMap[ENC_TL].ValueStr = String(ParameterStrMap[CCdetune]) + String((1 - groupvec[activeGroupIndex]->params().detune) * 100) + " %"));
        // }
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCdetune];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::OSCPAGE1;
  
        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCpitchB;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.PitchB;
        encMap[ENC_TR].ValueStr = groupvec[activeGroupIndex]->params().oscPitchB > 0 ? "+" : "" + String(groupvec[activeGroupIndex]->params().oscPitchB);
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCpitchB];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::OSCPAGE1;
        break;
    case State::OSCMODPAGE1:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCbankselectLSB;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentBankIndex;
        encMap[ENC_BL].Range = BANKS_LIMIT;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCbankselectLSB];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::OSCMODPAGE2;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCoscLevelA;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.OscLevelA;
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCoscLevelA];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::OSCMODPAGE2;

        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCdetune;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.Detune;
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCdetune];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::OSCMODPAGE2;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = patchselect;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatchIndex;
        encMap[ENC_TR].Range = patches.size();
        encMap[ENC_TR].ParameterStr = ParameterStrMap[patchselect];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::OSCMODPAGE2;
        break;
    case State::OSCMODPAGE2:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCbankselectLSB;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentBankIndex;
        encMap[ENC_BL].Range = BANKS_LIMIT;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCbankselectLSB];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCoscLevelA;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.OscLevelA;
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCoscLevelA];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;

        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCdetune;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.Detune;
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCdetune];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = patchselect;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatchIndex;
        encMap[ENC_TR].Range = patches.size();
        encMap[ENC_TR].ParameterStr = ParameterStrMap[patchselect];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;
        break;
    case State::FILTERPAGE1:
        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = filterfreq256; // 256 values
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.FilterFreq;
        encMap[ENC_BL].ValueStr = String(groupvec[activeGroupIndex]->getCutoff()) + " Hz";
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
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCfiltermixer];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCfilterenv;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.FilterEnv;
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCfilterenv];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;
        break;
    case State::FILTERPAGE2:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCfilterattack;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.FilterAttack;
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCfilterattack];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCfilterdecay;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.FilterDecay;
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCfilterdecay];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCfiltersustain;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.FilterSustain;
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCfiltersustain];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCfilterrelease;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.FilterRelease;
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCfilterrelease];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;

    case State::FILTERMODPAGE1:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCfilterattack;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.FilterAttack;
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCfilterattack];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCfilterdecay;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.FilterDecay;
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCfilterdecay];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCfiltersustain;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.FilterSustain;
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCfiltersustain];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCfilterrelease;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.FilterRelease;
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCfilterrelease];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;

    case State::FILTERMODPAGE2:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCfilterattack;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.FilterAttack;
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCfilterattack];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCfilterdecay;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.FilterDecay;
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCfilterdecay];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCfiltersustain;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.FilterSustain;
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCfiltersustain];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCfilterrelease;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.FilterRelease;
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCfilterrelease];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;

    case State::AMPPAGE:
        encMap[ENC_TL].active = true;
        encMap[ENC_TL].Parameter = CCampattack;
        encMap[ENC_TL].ShowValue = true;
        encMap[ENC_TL].Value = currentPatch.Attack;
        encMap[ENC_TL].Range = 127;
        encMap[ENC_TL].ParameterStr = ParameterStrMap[CCampattack];
        encMap[ENC_TL].Push = true;
        encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = CCampdecay;
        encMap[ENC_TR].ShowValue = true;
        encMap[ENC_TR].Value = currentPatch.Decay;
        encMap[ENC_TR].Range = 127;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[CCampdecay];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCampsustain;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.Sustain;
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCampsustain];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCamprelease;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.Release;
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[CCamprelease];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;

    case State::FXPAGE:
        encMap[ENC_TL].active = false;
        // encMap[ENC_TL].Parameter = CCampattack;
        // encMap[ENC_TL].ShowValue = true;
        // encMap[ENC_TL].Value = currentPatch.Attack;
        // encMap[ENC_TL].Range = 127;
        // encMap[ENC_TL].ParameterStr = ParameterStrMap[CCampattack];
        // encMap[ENC_TL].Push = true;
        // encMap[ENC_TL].PushAction = State::MAIN;

        encMap[ENC_TR].active = false;
        // encMap[ENC_TR].Parameter = CCampdecay;
        // encMap[ENC_TR].ShowValue = true;
        // encMap[ENC_TR].Value = currentPatch.Decay;
        // encMap[ENC_TR].Range = 127;
        // encMap[ENC_TR].ParameterStr = ParameterStrMap[CCampdecay];
        // encMap[ENC_TR].Push = true;
        // encMap[ENC_TR].PushAction = State::MAIN;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCfxamt;
        encMap[ENC_BL].ShowValue = true;
        encMap[ENC_BL].Value = currentPatch.EffectAmt;
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[CCfxamt];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = CCfxmix;
        encMap[ENC_BR].ShowValue = true;
        encMap[ENC_BR].Value = currentPatch.EffectMix;
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
        encMap[ENC_BL].Value = "";
        encMap[ENC_BL].Range = 127;
        encMap[ENC_BL].ParameterStr = ParameterStrMap[savepatch];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushAction = State::MAIN;

        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = renamepatch;
        encMap[ENC_BR].ShowValue = false;
        encMap[ENC_BL].Value = "";
        encMap[ENC_BR].Range = 127;
        encMap[ENC_BR].ParameterStr = ParameterStrMap[renamepatch];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushAction = State::MAIN;
        break;

    case State::DELETE:
        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = true;
        encMap[ENC_TR].Parameter = deletepatch;
        encMap[ENC_TR].ShowValue = false;
        encMap[ENC_TR].ParameterStr = ParameterStrMap[deletepatch];
        encMap[ENC_TR].Push = true;
        encMap[ENC_TR].PushAction = State::DELETEMSG;

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
    default:
        break;
    }
}