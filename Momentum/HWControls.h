#include <EncoderTool.h>
using namespace EncoderTool;
#include "TButton.h"

constexpr unsigned encoderCount = 4; // number of attached

constexpr unsigned QH_A = 0;   //output pin QH of shift register B
constexpr unsigned QH_B = 1;   //output pin QH of shift register A
constexpr unsigned pinLD = 3;  //load pin for all shift registers)
constexpr unsigned pinCLK = 4; //clock pin for all shift registers


EncPlex74165 encoders(encoderCount, pinLD, pinCLK, QH_A, QH_B);

//TeensyMM pins
#define BACKLIGHT 24 //A10/I2C_SCL1

//These are pushbuttons and require debouncing
//TButton button1{OSC_FX_SW, LOW, HOLD_DURATION, DEBOUNCE, CLICK_DURATION};


void encoderCallback(unsigned enc_idx, int value, int delta) {
  Serial.printf("enc[%u]: v=%d, d=%d\n", enc_idx, value, delta);
}

//   if (enc.buttonChanged()) { Serial.printf("button: %s\n", enc.getButton() == LOW ? "pressed" : "released"); }

FLASHMEM void setupHardware() {
  encoders.begin(CountMode::quarterInv);
  encoders.attachCallback(encoderCallback);

  //Display backlight - Can be used to turn off or dim using PWM
  //Not used but you never know
  //pinMode(BACKLIGHT, OUTPUT);
}
