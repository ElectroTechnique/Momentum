

typedef struct EncoderMappingStruct
{
    boolean active = true;
    uint8_t Parameter = 0;                    // MIDI cc (up to 127) or other code up to 255
    uint8_t Value = 0;                        // Up to 255
    uint8_t Range = 127;                      // Up to 255
    String ParameterStr = "";                 // For display
    boolean Push = false;                     // Pushing encoder button for further settings
    State PushActionState = State::PARAMETER; // Next state when button pushed
} EncoderMappingStruct;

EncoderMappingStruct encMap[4] = {};

FLASHMEM void setEncodersState(State s)
{
    switch (s)
    {
    case State::PARAMETER:
        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = patchselect;
        encMap[ENC_BR].Value = currentPatchIndex;
        encMap[ENC_BR].Range = patches.size();
        encMap[ENC_BR].ParameterStr = StrMap[patchselect];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushActionState = State::RECALL;

        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = false;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCbankselectLSB;
        encMap[ENC_BL].Value = currentBankIndex;
        encMap[ENC_BL].Range = BANKS_LIMIT;
        encMap[ENC_BL].ParameterStr = StrMap[CCbankselectLSB];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushActionState = State::RECALL;
        break;

    case State::RECALL:
        encMap[ENC_BR].active = true;
        encMap[ENC_BR].Parameter = patchselect;
        encMap[ENC_BR].Value = currentPatchIndex;
        encMap[ENC_BR].Range = patches.size();
        encMap[ENC_BR].ParameterStr = StrMap[patchselect];
        encMap[ENC_BR].Push = true;
        encMap[ENC_BR].PushActionState = State::PARAMETER;

        encMap[ENC_TL].active = false;

        encMap[ENC_TR].active = false;

        encMap[ENC_BL].active = true;
        encMap[ENC_BL].Parameter = CCbankselectLSB;
        encMap[ENC_BL].Value = currentBankIndex;
        encMap[ENC_BL].Range = BANKS_LIMIT;
        encMap[ENC_BL].ParameterStr = StrMap[CCbankselectLSB];
        encMap[ENC_BL].Push = true;
        encMap[ENC_BL].PushActionState = State::PARAMETER;
        break;

    default:
        break;
    }
}