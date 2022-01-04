/*
  https://forum.pjrc.com/threads/64746-Yet-another-highly-optimzed-ILI9341-library-for-T4-4-1-with-diff-updates-and-vsync-!/page2?highlight=ili9341
*/

#ifndef MOMENTUM_ILI9341_DISPLAY_H
#define MOMENTUM_ILI9341_DISPLAY_H

#define sclk 13  //SCK
#define mosi 11  //MOSI
#define cs 10    //CS
#define dc 5     //D1
#define rst 4    //D0
#define miso 255 //Unused
#define SPI_SPEED 60000000

#define DISPLAYTIMEOUT 700

#include "Voice.h"
#include "Yeysk12pt7b.h"
#include "Yeysk16pt7b.h"
#include "Fonts/FreeSansBold18pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSansOblique24pt7b.h"
#include "Fonts/FreeSansBoldOblique24pt7b.h"

#define PULSE 1
#define VAR_TRI 2
#define FILTER_ENV 3
#define AMP_ENV 4

ILI9341_t3n tft = ILI9341_t3n(cs, dc, rst, mosi, sclk, miso);

String currentParameter = "";
String currentValue = "";
float currentFloatValue = 0.0;
String currentPgmNum = "";
String currentPatchName = "";
String newPatchName = "";
const char * currentSettingsOption = "";
const char * currentSettingsValue = "";
uint32_t currentSettingsPart = SETTINGS;
uint32_t paramType = PARAMETER;

boolean MIDIClkSignal = false;
uint32_t peakCount = 0;
uint16_t prevLen = 0;

uint32_t colourPriority[5] = {ILI9341_BLACK, ILI9341_BLUE, ILI9341_YELLOW, ILI9341_ORANGE, ILI9341_MAROON};

unsigned long timer = 0;

void startTimer() {
  if (state == PARAMETER)
  {
    timer = millis();
  }
}

FLASHMEM void setMIDIClkSignal(bool val) {
  MIDIClkSignal = val;
}

FLASHMEM bool getMIDIClkSignal() {
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
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(90, 82);
  tft.setFont(&Yeysk16pt7b);
  tft.println("MOMENTUM");
  tft.setTextColor(ILI9341_RED);
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(140, 120);
  tft.println(VERSION);
}

FLASHMEM void renderPeak() {
  if (vuMeter && global.peak.available()) {
    uint16_t len = 0;
    if (peakCount > 1) {
      len = (int)(global.peak.read() * 75.0f);
      prevLen = len;
      peakCount = 0;
    } else {
      len = prevLen;
      peakCount++;
    }
    tft.drawFastVLine(158, 103 - len, len ,  len > 72 ? ILI9341_RED : ILI9341_GREEN);
    tft.drawFastVLine(159, 103 - len, len ,  len > 72 ? ILI9341_RED : ILI9341_GREEN);
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
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(1);
  tft.println("Category");

  tft.setTextColor(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);

  if (MIDIClkSignal)
  {
    tft.fillRect(100, 27, 14, 7, ILI9341_ORANGE);
    tft.setCursor(101, 32);
    tft.println("CK");
  }
  renderPeak();


  uint8_t fillColour[global.maxVoices()] = {};
  uint8_t borderColour[global.maxVoices()] = {};
  // Select colours based on voice state.
  uint8_t i = 0;
  for (uint8_t group = 0; group < groupvec.size(); group++) {
    for (uint8_t voice = 0; voice  < groupvec[group]->size(); voice++) {
      borderColour[i] = group + 1;
      if ((*groupvec[group])[voice]->on()) fillColour[i] = (*groupvec[group])[voice]->noteId() + 1;
      else fillColour[i] = 0;
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
  tft.drawFastHLine(108, 74, 15 + (value * 13), ILI9341_CYAN);
  tft.drawFastVLine(123 + (value * 13), 74, 20, ILI9341_CYAN);
  tft.drawFastHLine(123 + (value * 13), 94, 16 - (value * 13), ILI9341_CYAN);
  if (value < 0)
  {
    tft.drawFastVLine(108, 74, 21, ILI9341_CYAN);
  }
  else
  {
    tft.drawFastVLine(138, 74, 21, ILI9341_CYAN);
  }
}

FLASHMEM void renderVarTriangle(float value)
{
  tft.drawLine(110, 94, 123 + (value * 13), 74, ILI9341_CYAN);
  tft.drawLine(123 + (value * 13), 74, 136, 94, ILI9341_CYAN);
}

FLASHMEM void renderEnv(float att, float dec, float sus, float rel)
{
  tft.drawLine(100, 94, 100 + (att * 15), 74, ILI9341_CYAN);
  tft.drawLine(100 + (att * 15), 74.0, 100 + ((att + dec) * 15), 94 - (sus * 20), ILI9341_CYAN);
  tft.drawFastHLine(100 + ((att + dec) * 15), 94 - (sus * 20), 40 - ((att + dec) * 15), ILI9341_CYAN);
  tft.drawLine(139, 94 - (sus * 20), 139 + (rel * 13), 94, ILI9341_CYAN);
}

FLASHMEM void renderCurrentParameterPage() {
  switch (state) {
    case PARAMETER:
      tft.fillScreen(ILI9341_BLACK);
      tft.setFont(&FreeSans12pt7b);
      tft.setCursor(0, 53);
      tft.setTextColor(ILI9341_YELLOW);
      tft.setTextSize(1);
      tft.println(currentParameter);
      renderPeak();
      tft.setCursor(1, 90);
      tft.setTextColor(ILI9341_WHITE);
      tft.println(currentValue);
      switch (paramType) {
        case PULSE:
          renderPulseWidth(currentFloatValue);
          break;
        case VAR_TRI:
          renderVarTriangle(currentFloatValue);
          break;
        case FILTER_ENV:
          renderEnv(groupvec[activeGroupIndex]->getFilterAttack() * 0.0001f, groupvec[activeGroupIndex]->getFilterDecay() * 0.0001f, groupvec[activeGroupIndex]->getFilterSustain(), groupvec[activeGroupIndex]->getFilterRelease() * 0.0001f);
          break;
        case AMP_ENV:
          renderEnv(groupvec[activeGroupIndex]->getAmpAttack() * 0.0001f, groupvec[activeGroupIndex]->getAmpDecay() * 0.0001f, groupvec[activeGroupIndex]->getAmpSustain(), groupvec[activeGroupIndex]->getAmpRelease() * 0.0001f);
          break;
      }
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
  tft.println(patches.last().patchNo);
  tft.setCursor(35, 78);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(patches.last().patchName);
  tft.fillRect(0, 85, tft.width(), 23, ILI9341_MAROON);
  tft.setCursor(0, 98);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(patches.first().patchNo);
  tft.setCursor(35, 98);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(patches.first().patchName);
}

FLASHMEM void renderDeleteMessagePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(2, 53);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.println("Renumbering");
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
  tft.println(patches[patches.size() - 2].patchNo);
  tft.setCursor(35, 78);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(patches[patches.size() - 2].patchName);
  tft.fillRect(0, 85, tft.width(), 23, ILI9341_MAROON);
  tft.setCursor(0, 98);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(patches.last().patchNo);
  tft.setCursor(35, 98);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(patches.last().patchName);
}

FLASHMEM void renderReinitialisePage()
{
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(1);
  tft.setCursor(5, 53);
  tft.println("Initialise to");
  tft.setCursor(5, 90);
  tft.println("panel setting");
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
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(&FreeSans12pt7b);
  tft.setCursor(0, 45);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(patches.last().patchNo);
  tft.setCursor(35, 45);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(patches.last().patchName);

  tft.fillRect(0, 56, tft.width(), 23, 0xA000);
  tft.setCursor(0, 72);
  tft.setTextColor(ILI9341_YELLOW);
  tft.println(patches.first().patchNo);
  tft.setCursor(35, 72);
  tft.setTextColor(ILI9341_WHITE);
  tft.println(patches.first().patchName);

  tft.setCursor(0, 98);
  tft.setTextColor(ILI9341_YELLOW);
  patches.size() > 1 ? tft.println(patches[1].patchNo) : tft.println(patches.last().patchNo);
  tft.setCursor(35, 98);
  tft.setTextColor(ILI9341_WHITE);
  patches.size() > 1 ? tft.println(patches[1].patchName) : tft.println(patches.last().patchName);
}

FLASHMEM void showRenamingPage(String newName)
{
  newPatchName = newName;
}

FLASHMEM void renderUpDown(uint16_t x, uint16_t y, uint16_t colour)
{
  //Produces up/down indicator glyph at x,y
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
  if (currentSettingsPart == SETTINGS)
    renderUpDown(140, 42, ILI9341_YELLOW);
  tft.drawFastHLine(10, 63, tft.width() - 20, ILI9341_RED);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(5, 90);
  tft.println(currentSettingsValue);
  if (currentSettingsPart == SETTINGSVALUE)
    renderUpDown(140, 80, ILI9341_WHITE);
}

FLASHMEM void showCurrentParameterPage( const char *param, float val, int pType) {
  currentParameter = param;
  currentValue = String(val);
  currentFloatValue = val;
  paramType = pType;
  startTimer();
}

FLASHMEM void showCurrentParameterPage(const char *param, String val, int pType) {
  if (state == SETTINGS || state == SETTINGSVALUE)state = PARAMETER;//Exit settings page if showing
  currentParameter = param;
  currentValue = val;
  paramType = pType;
  startTimer();
}

FLASHMEM void showCurrentParameterPage(const char *param, String val) {
  showCurrentParameterPage(param, val, PARAMETER);
}

FLASHMEM void showPatchPage(String number, String patchName) {
  currentPgmNum = number;
  currentPatchName = patchName;
}

FLASHMEM void showSettingsPage(const char *  option, const char * value, int settingsPart) {
  currentSettingsOption = option;
  currentSettingsValue = value;
  currentSettingsPart = settingsPart;
}

FLASHMEM void enableScope(boolean enable) {
  enable ? global.scope.ScreenSetup(&tft) : global.scope.ScreenSetup(NULL);
}

void displayThread() {
  threads.delay(2000); //Give bootup page chance to display
  while (1) {
    switch (state) {
      case PARAMETER:
        if ((millis() - timer) > DISPLAYTIMEOUT) {
          renderCurrentPatchPage();
        } else {
          renderCurrentParameterPage();
        }
        break;
      case RECALL:
        renderRecallPage();
        break;
      case SAVE:
        renderSavePage();
        break;
      case REINITIALISE:
        renderReinitialisePage();
        tft.updateScreen(); //update before delay
        threads.delay(1000);
        state = PARAMETER;
        break;
      case PATCHNAMING:
        renderPatchNamingPage();
        break;
      case PATCH:
        renderCurrentPatchPage();
        break;
      case DELETE:
        renderDeletePatchPage();
        break;
      case DELETEMSG:
        renderDeleteMessagePage();
        break;
      case SETTINGS:
      case SETTINGSVALUE:
        renderSettingsPage();
        break;
    }
    tft.updateScreen();
  }
}

void setupDisplay()
{
  tft.begin(SPI_SPEED);
  tft.useFrameBuffer(true);
  //tft.updateChangedAreasOnly(true);
  tft.setRotation(1);
  tft.invertDisplay(true);
  renderBootUpPage();
  tft.updateScreen();
  threads.addThread(displayThread);
}

#endif
