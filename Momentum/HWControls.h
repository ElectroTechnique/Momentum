#include <EncoderTool.h>
using namespace EncoderTool;
#include "TButton.h"

constexpr unsigned encoderCount = 8; // number of attached

constexpr unsigned encA = 40;   //output pin QH of shift register Enc pin A
constexpr unsigned encB = 41;   //output pin QH of shift register Enc pin B
constexpr unsigned encButton = 42;   //output pin QH of shift register Button
constexpr unsigned pinLD = 45;  //load pin for all shift registers
constexpr unsigned pinCLK = 6; //clock pin for all shift registers

const int latchPin = 26;//LEDs SR2
const int clockPin = 9;//LEDs CLK2
const int dataPinG = 44;//Green LEDs data
const int dataPinR = 43;//Ref LEDs data

EncPlex74165 encoders(encoderCount, pinLD, pinCLK, encB, encA, encButton);

//TeensyMM pins
#define BACKLIGHT 24 //A10/I2C_SCL1

typedef enum {
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
} controlParameter  ;

void encoderCallback(unsigned enc_idx, int value, int delta) {
  Serial.printf("enc[%u]: v=%d, d=%d\n", enc_idx, value, delta);
}


FLASHMEM void setupHardware() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPinG, OUTPUT);
  pinMode(dataPinR, OUTPUT);

  encoders.begin(CountMode::full);
  encoders.attachCallback(encoderCallback);

  //Display backlight - Can be used to turn off or dim using PWM
  //Not used but you never know
  //pinMode(BACKLIGHT, OUTPUT);
}
