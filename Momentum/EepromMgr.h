#include <EEPROM.h>

#define EEPROM_MIDI_CH 0
#define EEPROM_MIDI_OUT_CH 1
#define EEPROM_MIDI_THRU 2
#define EEPROM_SCOPE_ENABLE 3
#define EEPROM_VU_ENABLE 4
#define EEPROM_VOLUME 5
#define EEPROM_LAST_BANK 6
#define EEPROM_LAST_PATCH 7

FLASHMEM int8_t getLastBank()
{
  byte b = EEPROM.read(EEPROM_LAST_BANK);
  if (b < 0 || b > 7)
    return 0;
  return b;
}

FLASHMEM void storeLastBankToEEPROM(byte type)
{
  if (type != getLastBank())
    EEPROM.update(EEPROM_LAST_BANK, type);
}

FLASHMEM int8_t getLastPatch()
{
  byte p = EEPROM.read(EEPROM_LAST_PATCH);
  if (p < 0 || p > 127)
    return 0;
  return p;
}

FLASHMEM void storeLastPatchToEEPROM(byte type)
{
  if (type != getLastPatch())
    EEPROM.update(EEPROM_LAST_PATCH, type);
}

FLASHMEM void loadLastPatchUsed()
{
  currentBankIndex = getLastBank();
  currentPatchIndex = getLastPatch();
}

FLASHMEM int8_t getVolume()
{
  byte v = EEPROM.read(EEPROM_VOLUME);
  if (v < 0 || v > 127)
    return currentVolume;
  return v;
}

FLASHMEM void storeVolumeToEEPROM(byte type)
{
  if (type != getVolume())
    EEPROM.update(EEPROM_VOLUME, type);
}

FLASHMEM int getMIDIChannel()
{
  byte midiChannel = EEPROM.read(EEPROM_MIDI_CH);
  if (midiChannel < 0 || midiChannel > 16)
    midiChannel = MIDI_CHANNEL_OMNI; // If EEPROM has no MIDI channel stored
  return midiChannel;
}

FLASHMEM void storeMidiChannel(byte channel)
{
  if (channel != getMIDIChannel())
    EEPROM.update(EEPROM_MIDI_CH, channel);
}

FLASHMEM int getMIDIOutCh()
{
  byte mc = EEPROM.read(EEPROM_MIDI_OUT_CH);
  if (mc < 0 || midiOutCh > 16)
    mc = 0; // If EEPROM has no MIDI channel stored
  return mc;
}

FLASHMEM void storeMidiOutCh(byte channel)
{
  if (channel != getMIDIOutCh())
    EEPROM.update(EEPROM_MIDI_OUT_CH, channel);
}

FLASHMEM midi::Thru::Mode getMidiThru()
{
  byte midiThruMode = EEPROM.read(EEPROM_MIDI_THRU);
  if (midiThruMode < midi::Thru::Off || midiThruMode > midi::Thru::DifferentChannel)
    return midi::Thru::Full; // If EEPROM has no MIDI Thru stored
  return (midi::Thru::Mode)midiThruMode;
}

FLASHMEM void storeMidiThru(byte thru)
{
  if (thru != getMidiThru())
    EEPROM.update(EEPROM_MIDI_THRU, thru);
}

FLASHMEM boolean getScopeEnable()
{
  byte sc = EEPROM.read(EEPROM_SCOPE_ENABLE);
  if (sc < 0 || sc > 1)
    return false; // If EEPROM has no scope enable stored
  return sc == 1 ? true : false;
}

FLASHMEM void storeScopeEnable(byte ScopeEnable)
{
  if (ScopeEnable != getScopeEnable())
    EEPROM.update(EEPROM_SCOPE_ENABLE, ScopeEnable);
}

FLASHMEM boolean getVUEnable()
{
  byte vu = EEPROM.read(EEPROM_VU_ENABLE);
  if (vu < 0 || vu > 1)
    return false; // If EEPROM has no VU enable stored
  return vu == 1 ? true : false;
}

FLASHMEM void storeVUEnable(byte VUEnable)
{
  if (VUEnable != getVUEnable())
    EEPROM.update(EEPROM_VU_ENABLE, VUEnable);
}
