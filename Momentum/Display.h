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
#include "Fonts/FreeSansBold18pt7b.h"
#include "Fonts/FreeSansBold12pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSansOblique24pt7b.h"
#include "Fonts/FreeSansBoldOblique24pt7b.h"
#include "Fonts/FreeSansBold9pt7b.h"

#define PULSE 1
#define VAR_TRI 2
#define FILTER_ENV 3
#define AMP_ENV 4

// https://trolsoft.ru/en/articles/rgb565-color-picker
#define ILI9341_LIGHTBLUE 0x853E
#define ILI9341_DARKRED 0x6000

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
  // if (state == State::MAIN)
  // {
  timer = millis();
  // }
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
  tft.println("ELECTRO");
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(145, 48);
  tft.println("TECHNIQUE");
  tft.setCursor(90, 82);
  tft.setFont(&Yeysk16pt7b);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println("MOMENTUM");
  tft.setTextColor(ILI9341_ORANGE);
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(140, 120);
  tft.println(VERSION);
}

FLASHMEM void renderMIDI()
{
  tft.setFont(&FreeSans9pt7b);
  tft.setCursor(40, 145);
  tft.setTextColor(ILI9341_YELLOW);
  tft.print("MIDI In: All    ");
  tft.setTextColor(ILI9341_GREEN);
  switch (MIDIThru)
  {
  case midi::Thru::SameChannel:
    tft.print("MIDI Thru: ");
    tft.println(midiChannel);
    break;
  case midi::Thru::DifferentChannel:
    tft.print("MIDI Thru: Dif Ch");
    break;
  case midi::Thru::Full:
    tft.println("MIDI Thru: All");
    break;
  default:
    tft.print("MIDI Out: ");
    tft.println(midiOutCh);
    break;
  }
}

FLASHMEM void renderCorners()
{
  tft.setFont(&FreeSans9pt7b);
  // ENC_TL
  if (encMap[ENC_TL].active)
  {
    tft.fillTriangle(0, 0, 0, 14, 14, 0, encTriColour[ENC_TL]);
    if (encMap[ENC_TL].Push)
    {
      tft.fillTriangle(2, 2, 2, 10, 10, 2, ILI9341_BLACK);
    }
    tft.setCursor(14, 7);
    tft.setTextColor(encTriColour[ENC_TL]);
    tft.println(encMap[ENC_TL].ParameterStr);
    if (encMap[ENC_TL].ShowValue)
    {
      tft.setCursor(14, 27);
      tft.println(encMap[ENC_TL].ValueStr);
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
    tft.setCursor(180, 7);
    tft.setTextColor(encTriColour[ENC_TR]);
    tft.println(encMap[ENC_TR].ParameterStr);
    if (encMap[ENC_TR].ShowValue)
    {
      tft.setCursor(180, 27);
      tft.println(encMap[ENC_TR].ValueStr);
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
    tft.setCursor(200, 207);
    tft.setTextColor(encTriColour[ENC_BR]);
    tft.println(encMap[ENC_BR].ParameterStr);
    if (encMap[ENC_BR].ShowValue)
    {
      tft.setCursor(200, 187);
      tft.println(encMap[ENC_BR].ValueStr);
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
    tft.setCursor(14, 207);
    tft.setTextColor(encTriColour[ENC_BL]);
    tft.println(encMap[ENC_BL].ParameterStr);
    if (encMap[ENC_BL].ShowValue)
    {
      tft.setCursor(14, 187);
      tft.println(encMap[ENC_BL].ValueStr);
    }
  }
}

FLASHMEM void renderPeak()
{
  vuMeter = true;
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

FLASHMEM void renderCurrentPatchPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(45, 94);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.println(currentPgmNum);

  tft.setCursor(100, 94);
  tft.setTextColor(ILI9341_ORANGE);
  tft.setTextSize(1);
  tft.println(bankNames[currentBankIndex]);

  tft.setTextColor(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);

  if (MIDIClkSignal)
  {
    tft.fillRect(100, 27, 14, 7, ILI9341_ORANGE);
    tft.setCursor(101, 32);
    tft.println("CK");
  }
  renderPeak();
  renderCorners();
  renderMIDI();

  uint8_t fillColour[global.maxVoices()] = {};
  uint8_t borderColour[global.maxVoices()] = {};
  // Select colours based on voice state.
  uint8_t i = 0;
  for (uint8_t group = 0; group < groupvec.size(); group++)
  {
    for (uint8_t voice = 0; voice < groupvec[group]->size(); voice++)
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
  uint8_t max_rows = 3;
  uint8_t x_step = 10;
  uint8_t y_step = 10;
  uint8_t x_end = 175;
  uint8_t x_start = x_end - (x_step * ceil(global.maxVoices() / float(max_rows))) + x_step;
  uint8_t y_start = 57;
  uint8_t y_end = 77;
  uint8_t idx = 0;
  for (uint8_t y = y_start; y <= y_end; y += y_step)
  {
    for (uint8_t x = x_start; x <= x_end; x += x_step)
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
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(45, 120);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(currentPatchName);
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
  tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(60, 70);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.println(currentParameter);
  tft.setFont(&FreeSansBold12pt7b);
  tft.setCursor(60, 120);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(currentValue);
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
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(5, 53);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.println("Delete?");
  tft.drawFastHLine(10, 60, tft.width() - 20, ILI9341_RED);
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(0, 78);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(patches.back().patchUID);
  tft.setCursor(35, 78);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(patches.back().patchName);
  tft.fillRect(0, 85, tft.width(), 23, ILI9341_MAROON);
  tft.setCursor(0, 98);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(patches.front().patchUID);
  tft.setCursor(35, 98);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(patches.front().patchName);
}

FLASHMEM void renderDeleteMessagePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(2, 53);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.println("Patch Deleted from");
  tft.setCursor(10, 90);
  tft.println("SD Card");
}

FLASHMEM void renderSavePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(5, 53);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.println("Save?");
  tft.drawFastHLine(10, 60, tft.width() - 20, ILI9341_RED);
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(0, 78);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(patches[patches.size() - 2].patchUID);
  tft.setCursor(35, 78);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(patches[patches.size() - 2].patchName);
  tft.fillRect(0, 85, tft.width(), 23, ILI9341_MAROON);
  tft.setCursor(0, 98);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(patches.back().patchUID);
  tft.setCursor(35, 98);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(patches.back().patchName);
}

FLASHMEM void renderReinitialisePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.setCursor(ILI9341_t3n::CENTER, ILI9341_t3n::CENTER);
  tft.println("Initialise Patch Settings");
}

FLASHMEM void renderPatchNamingPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.setCursor(0, 53);
  tft.println("Rename Patch");
  tft.drawFastHLine(10, 63, tft.width() - 20, ILI9341_RED);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(5, 90);
  tft.println(newPatchName);
}

FLASHMEM void renderRecallPage()
{
  // tft.setTextDatum(MC_DATUM);
  // Bank list - 8 banks
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(ILI9341_RED);
  for (uint8_t i = 0; i < 8; i++)
  {
    tft.setCursor(0, 30 + (20 * i));
    tempBankIndex == i ? tft.setTextColor(ILI9341_ORANGE)
                       : tft.setTextColor(ILI9341_RED);
    tft.println(bankNames[i]);
  }

  // Patches -  up to 128 in each bank

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(ILI9341_LIGHTGREY);
  for (uint8_t i = 0; i < patches.size(); i++)
  {
    tft.setCursor(110, 30 + (20 * i));
    currentPatchIndex == i ? tft.setTextColor(ILI9341_WHITE)
                           : tft.setTextColor(ILI9341_LIGHTGREY);
    tft.print(i + 1);
    tft.print(" ");
    tft.println(patches[i].patchName);
  }
}

FLASHMEM void renderOscPage(uint8_t no)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(110, 150);
  if (no == 3)
  {
    tft.print("Noise & Unison");
  }
  else
  {
    tft.print("Oscillator ");
    tft.println(no);
  }
  renderCorners();
}

FLASHMEM void renderOscModPage(uint8_t no)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(60, 150);
  switch (no)
  {
  case 1:
    tft.print("Osc 1 PWM");
    break;
  case 2:
    tft.print("Osc 2 PWM");
    break;
  case 3:
    tft.print("Osc Pitch");
    break;
  case 4:
    tft.print("Osc Effects");
    break;
  }
  renderCorners();
}

FLASHMEM void renderFilterPage(uint8_t no)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(110, 150);
  tft.print("Filter");
  renderCorners();
}

FLASHMEM void renderFilterModPage(uint8_t no)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(110, 150);
  tft.print("Filter Modulation ");
  tft.println(no);
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

FLASHMEM void renderAmpPage(uint8_t no)
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(110, 150);
  tft.print("Amplifier ");
  tft.println(no);
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
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(110, 150);
  tft.print("Effects");
  renderCorners();
}

FLASHMEM void renderSequencerPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(110, 150);
  tft.print("Sequencer");
  renderCorners();
}

FLASHMEM void renderMIDIPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(110, 150);
  tft.print("MIDI");
  renderCorners();
}

FLASHMEM void renderArpPage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(110, 150);
  tft.print("Arpeggiator");
  renderCorners();
}

FLASHMEM void renderPerformancePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(110, 150);
  tft.print("Performance");
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
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.setCursor(0, 53);
  tft.println(currentSettingsOption);
  if (currentSettingsPart == State::SETTINGS)
    renderUpDown(140, 42, ILI9341_YELLOW);
  tft.drawFastHLine(10, 63, tft.width() - 20, ILI9341_RED);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(5, 90);
  tft.println(currentSettingsValue);
  if (currentSettingsPart == State::SETTINGSVALUE)
    renderUpDown(140, 80, ILI9341_WHITE);
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

FLASHMEM void showCurrentParameterOverlay(const char *param, String val, int pType)
{
  // Prevents all the changing parameters showing when a patch is loaded
  if (!updateDisplay)
    return;
  currentParameter = param;
  currentValue = val;
  paramType = pType;
  startTimer();
}

FLASHMEM void showCurrentParameterOverlay(const char *param, String val)
{
  showCurrentParameterOverlay(param, val, 0);
}

FLASHMEM void showPatchPage(String number, String patchName)
{
  currentPgmNum = number;
  currentPatchName = patchName;
}

FLASHMEM void showSettingsPage(const char *option, const char *value, int settingsPart)
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
  threads.delay(2000); // Give bootup page chance to display
  while (1)
  {

    if ((millis() - timer) > DISPLAYTIMEOUT)
    {
      switch (state)
      {
      case State::MAIN:
        renderCurrentPatchPage();
        break;
      case State::PATCHLIST:
        renderRecallPage();
        break;
      case State::SAVE:
        renderSavePage();
        break;
      case State::REINITIALISE:
        renderReinitialisePage();
        tft.updateScreen(); // update before delay
        threads.delay(1000);
        // state = State::MAIN; //Now sets previous state
        break;
      case State::PATCHNAMING:
        renderPatchNamingPage();
        break;
      // case State::PATCH:
      //   renderCurrentPatchPage();
      //   break;
      case State::DELETE:
        renderDeletePatchPage();
        break;
      case State::DELETEMSG:
        renderDeleteMessagePage();
        break;
      case State::SETTINGS:
      case State::SETTINGSVALUE:
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
