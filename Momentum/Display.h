/*
  https://forum.pjrc.com/threads/64746-Yet-another-highly-optimzed-ILI9341-library-for-T4-4-1-with-diff-updates-and-vsync-!/page2?highlight=ili9341
*/

#ifndef MOMENTUM_ILI9341_DISPLAY_H
#define MOMENTUM_ILI9341_DISPLAY_H

#define sclk 13  // SCK
#define mosi 11  // MOSI
#define cs 10    // CS
#define dc 5     // D1
#define rst 4    // D0
#define miso 255 // Unused

#define SPI_SPEED 100000000
#define SPICLOCK_READ 80000000

#define DISPLAYTIMEOUT 700

#include "Voice.h"
#include "Yeysk12pt7b.h"
#include "Yeysk16pt7b.h"
#include <ili9341_t3n_font_Arial.h>
#include <ili9341_t3n_font_ArialBold.h>
#include <ili9341_t3n_font_OpenSans.h>

#define PULSE 1
#define VAR_TRI 2
#define FILTER_ENV 3
#define AMP_ENV 4

// https://trolsoft.ru/en/articles/rgb565-color-picker
#define ILI9341_LIGHTBLUE 0x853E
#define ILI9341_DARKRED 0x6000
#define ILI9341_MIDGREY 0xB576
#define ILI9341_DARKYELLOW 0xBDE0

ILI9341_t3n tft = ILI9341_t3n(cs, dc, rst, mosi, sclk, miso);

String currentParameter = "";
String currentValue = "";
float currentFloatValue = 0.0;
String currentPgmNum = "";
String currentPatchName = "";
String newPatchName = "";
const char *currentSettingsOption = "";
const char *currentSettingsValue = "";
uint32_t currentSettingsPart = State::SETTINGS;
uint32_t paramType = 0;

boolean MIDIClkSignal = false;
uint32_t peakCount = 0;
uint16_t prevLen = 0;

const uint32_t colourPriority[5] = {ILI9341_BLACK, ILI9341_BLUE, ILI9341_YELLOW, ILI9341_ORANGE, ILI9341_MAROON};

const uint32_t encTriColour[4] = {ILI9341_LIGHTBLUE, ILI9341_YELLOW, ILI9341_WHITE, ILI9341_ORANGE};

unsigned long timer = 0;

boolean updateDisplay = true;

void startTimer()
{
  timer = millis();
}

FLASHMEM void setMIDIClkSignal(bool val)
{
  MIDIClkSignal = val;
}

FLASHMEM bool getMIDIClkSignal()
{
  return MIDIClkSignal;
}

FLASHMEM void renderBootUpPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.drawRect(10, 40, 134, 26, ILI9341_WHITE);
  tft.fillRect(144, 40, 155, 26, ILI9341_WHITE);
  tft.setCursor(13, 42);
  tft.setFont(&Yeysk12pt7b);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(F("ELECTRO"));
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(145, 48);
  tft.println(F("TECHNIQUE"));
  tft.setCursor(90, 82);
  tft.setFont(&Yeysk16pt7b);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(F("MOMENTUM"));
  tft.setTextColor(ILI9341_ORANGE);
  tft.setFont(Arial_16);
  tft.setCursor(140, 120);
  tft.println(VERSION);
}

FLASHMEM void renderMIDI()
{
  tft.setFont(Arial_12);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.drawString(F("MIDI In: "), 40, 145);
  tft.setTextColor(ILI9341_GREENYELLOW);
  if (midiChannel == MIDI_CHANNEL_OMNI)
  {
    tft.print(F("All"));
  }
  else
  {
    tft.print(midiChannel);
  }

  tft.setTextColor(ILI9341_LIGHTGREY);
  switch (MIDIThru)
  {
  case midi::Thru::SameChannel:
    tft.print(F("  MIDI Thru: "));
    tft.setTextColor(ILI9341_GREENYELLOW);
    tft.print(midiOutCh);
    break;
  case midi::Thru::DifferentChannel:
    tft.print(F("  MIDI Thru: Dif. Ch"));
    break;
  case midi::Thru::Full:
    tft.print(F("  MIDI Thru: All"));
    break;
  default:
    tft.print(F("  MIDI Out: "));
    tft.setTextColor(ILI9341_GREENYELLOW);
    tft.print(midiOutCh);
    break;
  }
}

FLASHMEM void renderCorners()
{
  // ENC_TL
  if (encMap[ENC_TL].active)
  {
    tft.fillTriangle(0, 0, 0, 14, 14, 0, encTriColour[ENC_TL]);
    if (encMap[ENC_TL].Push)
    {
      tft.fillTriangle(2, 2, 2, 10, 10, 2, ILI9341_BLACK);
    }
    tft.setTextColor(encTriColour[ENC_TL]);
    tft.setTextDatum(TL_DATUM);
    tft.setFont(Arial_13);
    tft.drawString(encMap[ENC_TL].ParameterStr, 14, 7);
    if (encMap[ENC_TL].ShowValue)
    {
      tft.setFont(Arial_16);
      tft.drawString(encMap[ENC_TL].ValueStr, 4, 27);
    }
  }
  // ENC_TR
  if (encMap[ENC_TR].active)
  {
    tft.fillTriangle(305, 0, 319, 0, 319, 14, encTriColour[ENC_TR]);
    if (encMap[ENC_TR].Push)
    {
      tft.fillTriangle(309, 2, 317, 2, 317, 10, ILI9341_BLACK);
    }
    tft.setTextColor(encTriColour[ENC_TR]);
    tft.setTextDatum(TR_DATUM);
    tft.setFont(Arial_13);
    tft.drawString(encMap[ENC_TR].ParameterStr, 303, 7);
    if (encMap[ENC_TR].ShowValue)
    {
      tft.setFont(Arial_16);
      tft.drawString(encMap[ENC_TR].ValueStr, 310, 27);
    }
  }
  // ENC_BR
  if (encMap[ENC_BR].active)
  {
    tft.fillTriangle(305, 239, 319, 239, 319, 225, encTriColour[ENC_BR]);
    if (encMap[ENC_BR].Push)
    {
      tft.fillTriangle(309, 237, 317, 237, 317, 229, ILI9341_BLACK);
    }
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(encTriColour[ENC_BR]);
    tft.setFont(Arial_13);
    tft.drawString(encMap[ENC_BR].ParameterStr, 303, 220);
    if (encMap[ENC_BR].ShowValue)
    {
      tft.setFont(Arial_16);
      tft.drawString(encMap[ENC_BR].ValueStr, 310, 196);
    }
  }

  // ENC_BL
  if (encMap[ENC_BL].active)
  {
    tft.fillTriangle(0, 225, 0, 239, 14, 239, encTriColour[ENC_BL]);
    if (encMap[ENC_BL].Push)
    {
      tft.fillTriangle(2, 229, 2, 237, 10, 237, ILI9341_BLACK);
    }
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(encTriColour[ENC_BL]);
    tft.setFont(Arial_13);
    tft.drawString(encMap[ENC_BL].ParameterStr, 14, 220);
    if (encMap[ENC_BL].ShowValue)
    {
      tft.setFont(Arial_16);
      tft.drawString(encMap[ENC_BL].ValueStr, 4, 196);
    }
  }
}

FLASHMEM void renderPeak()
{
  // vuMeter = true;
  if (vuMeter && global.peak.available())
  {
    uint16_t len = 0;
    if (peakCount > 1)
    {
      len = (int)(global.peak.read() * 150.0f);
      prevLen = len;
      peakCount = 0;
    }
    else
    {
      len = prevLen;
      peakCount++;
    }
    tft.drawFastVLine(318, 140 - len, len, len > 130 ? ILI9341_RED : ILI9341_GREEN);
    tft.drawFastVLine(319, 140 - len, len, len > 130 ? ILI9341_RED : ILI9341_GREEN);
  }
}

FLASHMEM void renderMainPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_YELLOW);
  tft.drawString(currentPgmNum, 45, 94);

  tft.setTextColor(ILI9341_ORANGE);
  tft.drawString(bankNames[currentBankIndex], 100, 94);

  tft.setTextColor(ILI9341_BLACK);
  tft.setFont(Arial_12_Bold);
  if (MIDIClkSignal)
  {
    tft.fillRect(100, 27, 14, 7, ILI9341_LIGHTGREY);
    tft.drawString(F("CLK"), 101, 32);
  }
  renderPeak();
  renderCorners();
  renderMIDI();

  size_t fillColour[global.maxVoices()] = {};
  size_t borderColour[global.maxVoices()] = {};
  // Select colours based on voice state.
  size_t i = 0;
  for (size_t group = 0; group < groupvec.size(); group++)
  {
    for (size_t voice = 0; voice < groupvec[group]->size(); voice++)
    {
      borderColour[i] = group + 1;
      if ((*groupvec[group])[voice]->on())
        fillColour[i] = (*groupvec[group])[voice]->noteId() + 1;
      else
        fillColour[i] = 0;
      i++;
    }
  }

  // Draw rectangles to represent each voice.
  size_t max_rows = 3;
  size_t x_step = 10;
  size_t y_step = 10;
  size_t x_end = 175;
  size_t x_start = x_end - (x_step * ceil(global.maxVoices() / float(max_rows))) + x_step;
  size_t y_start = 57;
  size_t y_end = 77;
  size_t idx = 0;
  for (size_t y = y_start; y <= y_end; y += y_step)
  {
    for (size_t x = x_start; x <= x_end; x += x_step)
    {
      // Always draw border to indicate timbre.
      tft.fillRect(x, y, 8, 8, colourPriority[fillColour[idx]]);
      tft.drawRect(x, y, 8, 8, colourPriority[borderColour[idx]]);
      idx++;
      if (idx >= global.maxVoices())
      {
        break;
      }
    }
  }
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_YELLOW);
  tft.drawString(currentPatchName, 45, 120);
}

FLASHMEM void renderPulseWidth(float value)
{
  tft.setOrigin(-40, -30);
  const uint16_t a = 216;
  const uint16_t b = 148;
  const uint16_t c = 246;
  const uint16_t d = 26;
  const uint16_t e = 40;
  const uint16_t f = 188;
  const uint16_t g = 32;
  const uint16_t h = 42;
  const uint16_t i = 276;
  const uint16_t j = 30;

  tft.drawFastHLine(a, b, j + (value * d), ILI9341_CYAN);
  tft.drawFastVLine(c + (value * d), b, e, ILI9341_CYAN);
  tft.drawFastHLine(c + (value * d), f, g - (value * d), ILI9341_CYAN);
  if (value < 0)
  {
    tft.drawFastVLine(a, b, h, ILI9341_CYAN);
  }
  else
  {
    tft.drawFastVLine(i, b, h, ILI9341_CYAN);
  }
  tft.setOrigin(0, 0);
}

FLASHMEM void renderVarTriangle(float value)
{
  tft.setOrigin(-40, -30);
  const uint16_t a = 220;
  const uint16_t b = 192;
  const uint16_t c = 246;
  const uint16_t d = 26;
  const uint16_t e = 172;
  const uint16_t f = 188;

  tft.drawLine(a, b, c + (value * d), f, ILI9341_CYAN);
  tft.drawLine(c + (value * d), f, e, b, ILI9341_CYAN);
  tft.setOrigin(0, 0);
}

FLASHMEM void renderEnv(float att, float dec, float sus, float rel)
{
  // tft.setOrigin(-40, -30); //Set origin before calling

  const uint16_t a = 200;
  const uint16_t b = 188;
  const uint16_t c = 30;
  const uint16_t d = 278;
  const uint16_t e = 40;
  const uint16_t f = 148;
  const uint16_t g = 80;
  const uint16_t h = 26;

  tft.drawLine(a, b, a + (att * c), f, ILI9341_CYAN);                                           // attack
  tft.drawLine(a + (att * c), f, a + ((att + dec) * c), b - (sus * e), ILI9341_CYAN);           // decay
  tft.drawFastHLine(a + ((att + dec) * c), b - (sus * e), g - ((att + dec) * c), ILI9341_CYAN); // sustain
  tft.drawLine(d, b - (sus * e), d + (rel * h), b, ILI9341_CYAN);                               // release
  tft.setOrigin(0, 0);
}

FLASHMEM void renderCurrentParameterOverlay()
{
  tft.fillRect(40, 30, 230, 150, ILI9341_DARKRED);
  tft.drawRect(40, 30, 230, 150, ILI9341_RED);

  tft.setFont(Arial_12_Bold);
  tft.setTextColor(ILI9341_YELLOW);
  tft.drawString(currentParameter, 60, 70);

  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_WHITE);
  tft.drawString(currentValue, 60, 120);
  switch (paramType)
  {
  case PULSE:
    renderPulseWidth(currentFloatValue);
    break;
  case VAR_TRI:
    renderVarTriangle(currentFloatValue);
    break;
  case FILTER_ENV:
    tft.setOrigin(-40, -30);
    renderEnv(groupvec[activeGroupIndex]->getFilterAttack() * 0.0001f, groupvec[activeGroupIndex]->getFilterDecay() * 0.0001f, groupvec[activeGroupIndex]->getFilterSustain(), groupvec[activeGroupIndex]->getFilterRelease() * 0.0001f);
    break;
  case AMP_ENV:
    tft.setOrigin(-40, -30);
    renderEnv(groupvec[activeGroupIndex]->getAmpAttack() * 0.0001f, groupvec[activeGroupIndex]->getAmpDecay() * 0.0001f, groupvec[activeGroupIndex]->getAmpSustain(), groupvec[activeGroupIndex]->getAmpRelease() * 0.0001f);
    break;
  }
}

FLASHMEM void renderDeletePatchPage()
{

  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16_Bold);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(F("Delete Patch"), 160, 19);
  tft.drawFastHLine(10, 42, tft.width() - 20, ILI9341_RED);

  // Bank list - 8 banks
  tft.setFont(Arial_12_Bold);
  tft.setTextColor(ILI9341_RED);
  for (size_t i = 0; i < 8; i++)
  {
    tempBankIndex == i ? tft.setTextColor(ILI9341_ORANGE)
                       : tft.setTextColor(ILI9341_RED);
    tft.drawString(bankNames[i], 0, 51 + (20 * i));
  }

  // Patches -  up to 128 in each bank
  tft.setTextDatum(TL_DATUM);
  tft.setFont(Arial_12_Bold);
  tft.setTextColor(ILI9341_LIGHTGREY);
  size_t offset = ceil(currentPatchIndex / 8);
  for (size_t i = 0; i < min(patches.size(), 8); i++)
  {
    currentPatchIndex == i + (8 * offset) ? tft.setTextColor(ILI9341_YELLOW)
                                          : tft.setTextColor(ILI9341_DARKYELLOW);
    if (i + 1 + (8 * offset) <= patches.size())
    {
      tft.drawString(String(i + 1 + (8 * offset)) + " " + patches[i + (8 * offset)].patchName, 110, 51 + (20 * i));
    }
  }
  renderCorners();
}

FLASHMEM void renderDeleteMessagePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(40, 30, 230, 150, ILI9341_DARKRED);
  tft.drawRect(40, 30, 230, 150, ILI9341_RED);
  tft.setFont(Arial_16);
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(ILI9341_YELLOW);
  tft.drawString(F("Deleted Patch"), 160, 100);
}

FLASHMEM void renderDeleteBankMessagePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(40, 30, 230, 150, ILI9341_DARKRED);
  tft.drawRect(40, 30, 230, 150, ILI9341_RED);
  tft.setFont(Arial_16);
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(ILI9341_YELLOW);
  tft.drawString(F("Deleted Bank"), 160, 100);
}

FLASHMEM void renderSavePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRect(40, 30, 230, 150, ILI9341_DARKRED);
  tft.drawRect(40, 30, 230, 150, ILI9341_RED);
  tft.setFont(Arial_16);
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(ILI9341_YELLOW);
  tft.drawString(F("Saved Patch"), 160, 100);
  tft.setTextColor(ILI9341_WHITE);
  tft.drawString(currentPatch.PatchName, 160, 120);
}

FLASHMEM void renderReinitialisePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.fillRect(40, 30, 230, 150, ILI9341_DARKRED);
  tft.drawRect(40, 30, 230, 150, ILI9341_RED);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(F("Initialise"), 160, 80);
  tft.drawString(F("Patch Settings"), 160, 105);
}

FLASHMEM void renderPatchSavingPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16_Bold);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(F("Save Patch"), 160, 19);
  tft.drawFastHLine(10, 42, tft.width() - 20, ILI9341_RED);

  // Bank list - 8 banks
  tft.setFont(Arial_12_Bold);
  tft.setTextColor(ILI9341_RED);
  for (size_t i = 0; i < 8; i++)
  {
    tempBankIndex == i ? tft.setTextColor(ILI9341_ORANGE)
                       : tft.setTextColor(ILI9341_RED);
    tft.drawString(bankNames[i], 0, 51 + (20 * i));
  }

  // Patches -  up to 128 in each bank
  tft.setTextDatum(TL_DATUM);
  tft.setFont(Arial_12_Bold);
  tft.setTextColor(ILI9341_LIGHTGREY);
  size_t offset = ceil(currentPatchIndex / 8);
  for (size_t i = 0; i < min(patches.size(), 8); i++)
  {
    currentPatchIndex == i + (8 * offset) ? tft.setTextColor(ILI9341_YELLOW)
                                          : tft.setTextColor(ILI9341_DARKYELLOW);
    if (i + 1 + (8 * offset) <= patches.size())
    {
      tft.drawString(String(i + 1 + (8 * offset)) + " " + patches[i + (8 * offset)].patchName, 110, 51 + (20 * i));
    }
  }
  renderCorners();
}

FLASHMEM void renderPatchNamingPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(F("Patch Name"), 160, 19);
  tft.drawFastHLine(10, 42, tft.width() - 20, ILI9341_RED);
  tft.setTextColor(ILI9341_RED);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(currentPatchName, 90, 70);
  tft.drawFontChar(95); // Underscore caret
  tft.setFont(Arial_14);
  tft.setTextColor(ILI9341_LIGHTGREY);
  int row = 0;
  int start = 0;
  for (int i = 0; i < TOTALCHARS; i++)
  {
    i % 20 != 0 ? row : row++;
    i % 20 != 0 ? start++ : start = 0;
    chosenChar == i ? tft.setTextColor(ILI9341_WHITE) : tft.setTextColor(ILI9341_MIDGREY);
    tft.drawString(CHARACTERS[i], 19 + start * 15, 90 + row * 22);
  }
  renderCorners();
}

FLASHMEM void renderBankNamingPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(F("Bank Name"), 160, 19);
  tft.drawFastHLine(10, 42, tft.width() - 20, ILI9341_RED);
  tft.setTextColor(ILI9341_RED);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(bankNames[tempBankIndex], 90, 70);
  tft.drawFontChar(95); // Underscore caret
  tft.setFont(Arial_14);
  tft.setTextColor(ILI9341_LIGHTGREY);
  int row = 0;
  int start = 0;
  for (int i = 0; i < TOTALCHARS; i++)
  {
    i % 20 != 0 ? row : row++;
    i % 20 != 0 ? start++ : start = 0;
    chosenChar == i ? tft.setTextColor(ILI9341_WHITE) : tft.setTextColor(ILI9341_MIDGREY);
    if (i == 26 || i == 53)
    {
      tft.drawString('_', 19 + start * 15, 90 + row * 22);
    }
    else
    {
      tft.drawString(CHARACTERS[i], 19 + start * 15, 90 + row * 22);
    }
  }
  renderCorners();
}

FLASHMEM void renderRecallPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Recall Patch", 160, 19);
  tft.drawFastHLine(10, 42, tft.width() - 20, ILI9341_RED);

  // tft.setTextDatum(MC_DATUM);
  // Bank list - 8 banks
  tft.setFont(Arial_12_Bold);
  tft.setTextColor(ILI9341_RED);
  for (size_t i = 0; i < BANKS_LIMIT; i++)
  {
    tempBankIndex == i ? tft.setTextColor(ILI9341_ORANGE)
                       : tft.setTextColor(ILI9341_RED);
    tft.drawString(bankNames[i], 0, 51 + (20 * i));
  }

  // Patches -  up to 128 in each bank
  tft.setFont(Arial_12_Bold);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(ILI9341_YELLOW);
  size_t offset = ceil(currentPatchIndex / 8);
  for (size_t i = 0; i < min(patches.size(), 8); i++)
  {
    currentPatchIndex == i + (8 * offset) ? tft.setTextColor(ILI9341_YELLOW)
                                          : tft.setTextColor(ILI9341_DARKYELLOW);
    if (i + 1 + (8 * offset) <= patches.size())
    {
      tft.drawString(String(i + 1 + (8 * offset)) + " " + patches[i + (8 * offset)].patchName, 110, 51 + (20 * i));
    }
  }
  renderCorners();
}

FLASHMEM void renderOscPage(size_t no)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextDatum(TC_DATUM);
  if (no == 3)
  {
    tft.drawString("Noise & Unison", 160, 150);
  }
  else
  {
    tft.drawString("Oscillator " + String(no), 160, 150);
  }
  renderCorners();
}

FLASHMEM void renderOscModPage(size_t no)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextDatum(TC_DATUM);
  switch (no)
  {
  case 1:
    tft.drawString("Osc 1 PWM", 160, 150);
    break;
  case 2:
    tft.drawString("Osc 2 PWM", 160, 150);
    break;
  case 3:
    tft.drawString("Osc Pitch", 160, 150);
    break;
  case 4:
    tft.drawString("Osc Effects", 160, 150);
    break;
  }
  renderCorners();
}

FLASHMEM void renderFilterPage(size_t no)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Filter", 160, 150);
  renderCorners();
}

FLASHMEM void renderFilterModPage(size_t no)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Filter Modulation" + String(no), 160, 150);
  renderCorners();
  if (no == 1)
  {
    tft.setOrigin(-80, -55);
    renderEnv(groupvec[activeGroupIndex]->getFilterAttack() * 0.0001f,
              groupvec[activeGroupIndex]->getFilterDecay() * 0.0001f,
              groupvec[activeGroupIndex]->getFilterSustain(),
              groupvec[activeGroupIndex]->getFilterRelease() * 0.0001f);
  }
}

FLASHMEM void renderAmpPage(size_t no)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Amplifier " + String(no), 160, 150);
  renderCorners();
  if (no == 1)
  {
    tft.setOrigin(-80, -55);
    renderEnv(groupvec[activeGroupIndex]->getAmpAttack() * 0.0001f,
              groupvec[activeGroupIndex]->getAmpDecay() * 0.0001f,
              groupvec[activeGroupIndex]->getAmpSustain(),
              groupvec[activeGroupIndex]->getAmpRelease() * 0.0001f);
  }
}

FLASHMEM void renderFXPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Effects", 160, 150);
  renderCorners();
}

FLASHMEM void renderSequencerPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Sequencer", 160, 19);
  tft.drawFastHLine(10, 42, tft.width() - 20, ILI9341_RED);
  renderCorners();
}

FLASHMEM void renderMIDIPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("MIDI", 160, 150);
  renderCorners();
}

FLASHMEM void renderArpPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Arpeggiator", 160, 19);
  tft.drawFastHLine(10, 42, tft.width() - 20, ILI9341_RED);
  renderCorners();
}

FLASHMEM void renderPerformancePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Recall Performance", 160, 19);
  tft.drawFastHLine(10, 42, tft.width() - 20, ILI9341_RED);

  tft.setFont(Arial_12);
  tft.setTextColor(ILI9341_RED);
  for (size_t i = 0; i < performances.size(); i++)
  {
    tempBankIndex == i ? tft.setTextColor(ILI9341_ORANGE)
                       : tft.setTextColor(ILI9341_RED);
    tft.drawString(performances[i].performanceName, 0, 44 + (20 * i));
  }
  renderCorners();
}

FLASHMEM void renderEditBankPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Edit Banks", 160, 19);
  tft.drawFastHLine(10, 42, tft.width() - 20, ILI9341_RED);

  // Bank list - 8 banks
  tft.setFont(Arial_12_Bold);
  tft.setTextColor(ILI9341_RED);
  for (size_t i = 0; i < 8; i++)
  {
    tempBankIndex == i ? tft.setTextColor(ILI9341_ORANGE)
                       : tft.setTextColor(ILI9341_RED);
    tft.drawString(bankNames[i], 0, 51 + (20 * i));
  }
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(ILI9341_YELLOW);
  if (patches[0].patchUID == 0)
  {
    tft.drawString("0 Patches", 110, 51);
  }
  else
  {
    tft.drawString(String(patches.size()) + " Patches", 110, 51);
  }
  tft.drawString(String(patches.size()) + " Patches", 110, 51);
  for (size_t i = 0; i < min(patches.size(), 6); i++)
  {
    tft.setTextColor(ILI9341_DARKYELLOW);
    if (i + 1 <= patches.size())
    {
      tft.drawString(String(i + 1) + " " + patches[i].patchName, 110, 76 + (20 * i));
    }
  }
  renderCorners();
}

FLASHMEM void renderDeleteBankPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Delete Bank", 160, 19);
  tft.drawFastHLine(10, 42, tft.width() - 20, ILI9341_RED);

  tft.setFont(Arial_12_Bold);
  tft.setTextColor(ILI9341_RED);
  tft.drawString(bankNames[currentBankIndex], 160, 120);
  tft.drawString("This will delete all", 160, 145);
  tft.drawString("patches in this bank", 160, 165);

  renderCorners();
}

FLASHMEM void showRenamingPage(String newName)
{
  newPatchName = newName;
}

FLASHMEM void renderUpDown(uint16_t x, uint16_t y, uint16_t colour)
{
  // Produces up/down indicator glyph at x,y
  tft.setCursor(x, y);
  tft.fillTriangle(x, y, x + 8, y - 8, x + 16, y, colour);
  tft.fillTriangle(x, y + 4, x + 8, y + 12, x + 16, y + 4, colour);
}

FLASHMEM void renderSettingsPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(currentSettingsOption, 10, 50);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextDatum(TR_DATUM);
  tft.drawString(currentSettingsValue, 310, 50);
  tft.setFont(Arial_16);
  tft.setTextColor(ILI9341_BLUE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Global Settings", 160, 150);
  renderCorners();
}

FLASHMEM void showCurrentParameterOverlay(const char *param, float val, int pType)
{
  if (!updateDisplay)
    return;
  currentParameter = param;
  currentValue = String(val);
  currentFloatValue = val;
  paramType = pType;
  startTimer();
}

FLASHMEM void showCurrentParameterOverlay(String param, String val, int pType)
{
  // Prevents all the changing parameters showing when a patch is loaded
  if (!updateDisplay)
    return;
  currentParameter = param;
  currentValue = val;
  paramType = pType;
  startTimer();
}

FLASHMEM void showCurrentParameterOverlay(String param, String val)
{
  showCurrentParameterOverlay(param, val, 0);
}

FLASHMEM void setPatchNoAndNameForDisp(String number, String patchName)
{
  currentPgmNum = number;
  currentPatchName = patchName;
}

FLASHMEM void setSettingsForDisp(const char *option, const char *value, int settingsPart)
{
  currentSettingsOption = option;
  currentSettingsValue = value;
  currentSettingsPart = settingsPart;
}

FLASHMEM void enableScope(boolean enable)
{
  enable ? global.scope.ScreenSetup(&tft) : global.scope.ScreenSetup(NULL);
}

void displayThread()
{
  threads.delay(1500); // Give bootup page chance to display
  while (1)
  {

    if ((millis() - timer) > DISPLAYTIMEOUT)
    {
      switch (state)
      {
      case State::MAIN:
        renderMainPage();
        break;
      case State::PATCHLIST:
        renderRecallPage();
        break;
      case State::SAVE:
        renderSavePage();
        tft.updateScreen(); // update before delay
        threads.delay(1000);
        break;
      case State::REINITIALISE:
        renderReinitialisePage();
        tft.updateScreen(); // update before delay
        threads.delay(1000);
        // state = State::MAIN; //Now sets previous state
        break;
      case State::PATCHSAVING:
        renderPatchSavingPage();
        break;
      case State::RENAMEPATCH:
      case State::CHOOSECHARPATCH:
      case State::DELETECHARPATCH:
        renderPatchNamingPage();
        break;
      case State::DELETEPATCH:
        renderDeletePatchPage();
        break;
      case State::DELETEMSG:
        renderDeleteMessagePage();
        tft.updateScreen(); // update before delay
        threads.delay(1000);
        break;
      case State::DELETEBANKMSG:
        renderDeleteBankMessagePage();
        tft.updateScreen(); // update before delay
        threads.delay(1000);
        break;
      case State::SETTINGS:
        renderSettingsPage();
        break;
      case State::OSCPAGE1:
        renderOscPage(1);
        break;
      case State::OSCPAGE2:
        renderOscPage(2);
        break;
      case State::OSCPAGE3:
        renderOscPage(3);
        break;
      case State::OSCMODPAGE1:
        renderOscModPage(1);
        break;
      case State::OSCMODPAGE2:
        renderOscModPage(2);
        break;
      case State::OSCMODPAGE3:
        renderOscModPage(3);
        break;
      case State::OSCMODPAGE4:
        renderOscModPage(4);
        break;
      case State::FILTERPAGE1:
        renderFilterPage(1);
        break;
      case State::FILTERMODPAGE1:
        renderFilterModPage(1);
        break;
      case State::FILTERMODPAGE2:
        renderFilterModPage(2);
        break;
      case State::FILTERMODPAGE3:
        renderFilterModPage(3);
        break;
      case State::AMPPAGE1:
        renderAmpPage(1);
        break;
      case State::AMPPAGE2:
        renderAmpPage(2);
        break;
      case State::AMPPAGE3:
        renderAmpPage(3);
        break;
      case State::FXPAGE:
        renderFXPage();
        break;
      case State::SEQPAGE:
        renderSequencerPage();
        break;
      case State::ARPPAGE:
        renderArpPage();
        break;
      case State::MIDIPAGE:
        renderMIDIPage();
        break;
      case State::PERFORMANCEPAGE:
        renderPerformancePage();
        break;
      case State::EDITBANK:
        renderEditBankPage();
        break;
      case State::RENAMEBANK:
      case State::CHOOSECHARBANK:
      case State::DELETECHARBANK:
        renderBankNamingPage();
        break;
      default:
        break;
      }
    }
    else
    {
      renderCurrentParameterOverlay();
    }
    tft.updateScreen();
  }
}

void setupDisplay()
{
  tft.begin(SPI_SPEED, SPICLOCK_READ);
  tft.useFrameBuffer(true);
  tft.setRotation(1);
  tft.invertDisplay(true);
  renderBootUpPage();
  tft.updateScreen();
  threads.addThread(displayThread);
}

#endif
