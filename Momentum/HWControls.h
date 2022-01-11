#include "ButtonTool/ButtonTool.h"
using namespace ButtonTool;
#include "EncoderTool/EncoderTool.h"
using namespace EncoderTool;

// Because of the way the four encoders and ten buttons are arranged on three 74HC595 shift registers in parallel, these counts are not the same
constexpr unsigned encoderCount = 8; // There are four arranged over three Shift Registers on the last four positions, hence all eight positions must be shifted to reach them.
constexpr unsigned buttonCount = 12; // There are ten buttons, but arranged over three Shift Registers on the first four positions. The last four are skipped.

constexpr unsigned srA = 40;   // output pin QH of shift register Enc pin A
constexpr unsigned srB = 41;   // output pin QH of shift register Enc pin B
constexpr unsigned srC = 42;   // output pin QH of shift register Button
constexpr unsigned pinLD = 45; // load pin for all shift registers
constexpr unsigned pinCLK = 6; // clock pin for all shift registers

const int latchPin = 26; // LEDs SR2
const int clockPin = 9;  // LEDs CLK2
const int dataPinG = 44; // Green LEDs data
const int dataPinR = 43; // Red LEDs data

EncPlex74165 encoders(encoderCount, pinLD, pinCLK, srB, srA, srC);
ButtonPlex74165 buttons(buttonCount, pinLD, pinCLK, srB, srA, srC);

// TeensyMM pins
#define BACKLIGHT 24 // A10/I2C_SCL1
typedef enum
{
  RED,
  GREEN,
} ledColour;

typedef enum
{
  control_noiseLevel,
  control_pitchLfoRate,
  control_pitchLfoWaveform,
  control_pitchLfoAmount,
  control_detune,
  control_oscMix,
  control_filterAttack,
  control_filterDecay,
  control_pwmAmountA,
  control_waveformA,
  control_pitchA,
  control_pwmAmountB,
  control_waveformB,
  control_pitchB,
  control_pwmRate,
  control_pitchEnv,
  control_attack,
  control_decay,
  control_sustain,
  control_release,
  control_filterLFOAmount,
  control_FXMix,
  control_FXAmount,
  control_glide,
  control_filterEnv,
  control_filterRelease,
  control_filterSustain,
  control_filterType,
  control_resonance,
  control_cutoff,
  control_filterLFORate,
  control_filterLFOWaveform,
  control_volume,
  control_unison,
  control_oscFX,
  control_filterLFORetrig,
  control_filterLFOMidiClkSync,
} controlParameter;

const int16_t LED_TO_BIN[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128}; // First value is dummy, LEDs are indexed 1-8

FLASHMEM void setupHardware(EncoderTool::allCallback_t ec, EncoderTool::allBtnCallback_t ebc, ButtonTool::allBtnCallback_t bc)
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPinG, OUTPUT);
  pinMode(dataPinR, OUTPUT);

  encoders.begin(CountMode::full);
  encoders.attachCallback(ec);
  encoders.attachBtnCallback(ebc);
  buttons.begin(bc);

  // Display backlight - Can be used to turn off or dim using PWM
  // Not used but you never know
  // pinMode(BACKLIGHT, OUTPUT);
}

// Shift both red and green LEDs - FOR 595s IN PARALLEL
void shiftOutX(uint8_t dataPinR, uint8_t dataPinG, uint8_t clockPin, uint8_t bitOrder, uint8_t valR, uint8_t valG)
{
  uint8_t i;

  for (i = 0; i < 8; i++)
  {
    if (bitOrder == LSBFIRST)
    {
      digitalWrite(dataPinR, valR & 1);
      valR >>= 1;
      digitalWrite(dataPinG, valG & 1);
      valG >>= 1;
    }
    else
    {
      digitalWrite(dataPinR, (valR & 128) != 0);
      valR <<= 1;
      digitalWrite(dataPinG, (valG & 128) != 0);
      valG <<= 1;
    }

    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }
}

void singleLED(ledColour color, int8_t ledNo)
{
  digitalWrite(latchPin, LOW);
  if (color == RED)
  {
    shiftOutX(dataPinR, dataPinG, clockPin, MSBFIRST, LED_TO_BIN[ledNo], 0);
  }
  else
  {
    shiftOutX(dataPinR, dataPinG, clockPin, MSBFIRST, 0, LED_TO_BIN[ledNo]);
  }
  digitalWrite(latchPin, HIGH);
}

void lightRGLEDs(int8_t ledRNos, int8_t ledGNos)
{
  digitalWrite(latchPin, LOW);
  shiftOutX(dataPinR, dataPinG, clockPin, MSBFIRST, ledRNos, ledGNos);
  digitalWrite(latchPin, HIGH);
}

void ledAnimation(long millis)
{
  lightRGLEDs(24, 0);
  delay(millis);
  lightRGLEDs(36, 0);
  delay(millis);
  lightRGLEDs(66, 0);
  delay(millis);
  lightRGLEDs(129, 0);
  delay(millis);
  lightRGLEDs(0, 0);
  delay(millis);
  lightRGLEDs(0, 129);
  delay(millis);
  lightRGLEDs(0, 66);
  delay(millis);
  lightRGLEDs(0, 36);
  delay(millis);
  lightRGLEDs(0, 24);
  delay(millis);
  lightRGLEDs(0, 0);
}