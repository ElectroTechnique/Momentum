#include "VoiceGroup.h"
#include "SettingsService.h"

void settingsMIDICh(int index, const char *value);
void settingsMIDIOutCh(int index, const char *value);
void settingsMIDIThru(int index, const char *value);
void settingsScopeEnable(int index, const char *value);
void settingsVUEnable(int index, const char *value);

int currentIndexMIDICh();
int currentIndexMIDIOutCh();
int currentIndexMIDIThru();
int currentIndexScopeEnable();
int currentIndexVUEnable();


FLASHMEM void settingsMIDICh(int index, const char *value)
{
  if (strcmp(value, "ALL") == 0)
  {
    midiChannel = MIDI_CHANNEL_OMNI;
  }
  else
  {
    midiChannel = atoi(value);
  }
  storeMidiChannel(midiChannel);
}


FLASHMEM void settingsMIDIOutCh(int index, const char *value)
{
  if (strcmp(value, "Off") == 0)
  {
    midiOutCh = 0;
  }
  else
  {
    midiOutCh = atoi(value);
  }
  storeMidiOutCh(midiOutCh);
}

FLASHMEM void settingsMIDIThru(int index, const char *value)
{
  if (strcmp(value, "Off") == 0)
    MIDIThru = midi::Thru::Off;
  if (strcmp(value, "Full") == 0)
    MIDIThru = midi::Thru::Full;
  if (strcmp(value, "Same Ch.") == 0)
    MIDIThru = midi::Thru::SameChannel;
  if (strcmp(value, "Diff. Ch.") == 0)
    MIDIThru = midi::Thru::DifferentChannel;
  changeMIDIThruMode();
  storeMidiThru(MIDIThru);
}

FLASHMEM void settingsScopeEnable(int index, const char *value)
{
  if (strcmp(value, "Off") == 0)
  {
    enableScope(false);
    storeScopeEnable(0);
  }
  else
  {
    enableScope(true);
    storeScopeEnable(1);
  }
}

FLASHMEM void settingsVUEnable(int index, const char *value)
{
  if (strcmp(value, "Off") == 0)
  {
    vuMeter = false;
    storeVUEnable(0);
  }
  else
  {
    vuMeter = true;
    storeVUEnable(1);
  }
}

FLASHMEM int currentIndexMIDICh()
{
  return getMIDIChannel();
}

FLASHMEM int currentIndexMIDIOutCh()
{
  return getMIDIOutCh();
}

FLASHMEM int currentIndexMIDIThru()
{
  if (MIDIThru == midi::Thru::Off)
    return 0;
  if (MIDIThru == midi::Thru::Full)
    return 1;
  if (MIDIThru == midi::Thru::SameChannel)
    return 2;
  if (MIDIThru == midi::Thru::DifferentChannel)
    return 3;
  return 0;
}

FLASHMEM int currentIndexScopeEnable()
{
  return getScopeEnable() ? 1 : 0;
}

FLASHMEM int currentIndexVUEnable()
{
  return getVUEnable() ? 1 : 0;
}

// add settings to the circular buffer
FLASHMEM void setUpSettings()
{
  settings::append(settings::SettingsOption{"Oscilloscope", {"Off", "On", "\0"}, settingsScopeEnable, currentIndexScopeEnable});
  settings::append(settings::SettingsOption{"VU Meter", {"Off", "On", "\0"}, settingsVUEnable, currentIndexVUEnable});
}
