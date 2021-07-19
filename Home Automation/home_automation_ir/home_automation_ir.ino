#include <IRremote.h>
/*
  CH- 0xBA45FF00
  CH 0xB946FF00
  CH+ 0xB847FF00
  PREV 0xBB44FF00
  NEXT 0xBF40FF00
  PLAY/PAUSE 0xBC43FF00
  VOL- 0xF807FF00
  VOL+ EA15FF00
  EQ 0xF609FF00

  0 0xE916FF00
  1 0xF30CFF00
  2 0xE718FF00
  3 0xA15EFF00
  4 0xF708FF00
  5 0xE31CFF00
  6 0xA55AFF00
  7 0xBD42FF00
  8 0xAD52FF00
  9 0xB54AFF00
*/
//codes
#define main_switch 0xE916FF00
#define channel_1 0xF30CFF00
#define channel_2 0xE718FF00
#define channel_3 0xA15EFF00
#define channel_4 0xF708FF00
#define turn_off_all 0xE31CFF00

#define IR_RECEIVE_PIN A0

//signal pin
const int main_switch_pin = 2;
const int channel1_pin = 12;
const int channel2_pin = 8;
const int channel3_pin = 7;
const int channel4_pin = 4;

int on_channels = 0;

int pin_data[] = {0, 0, 0, 0, 0, 0};
int pins[] = {main_switch_pin, channel1_pin, channel2_pin, channel3_pin, channel4_pin};

void setup() {
  //Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  for (int i = 0; i < 5; i++) {
    pinMode(pins[i], OUTPUT);
  }
  digitalWrite(pins[0], updatePinData(0)); //start the main switch HIGH
}

void modify_all_channels(int mode) {
  //reset all channels but stay the power
  if (pin_data[0] != 0) {
    //is ms is on
    if (mode == 0) {
      pin_data[5] = 1;
    } else {
      pin_data[5] = 0;
    }
    for (int i = 1; i < 5; i++) {
      digitalWrite(pins[i], mode);
      pin_data[i] = mode;
    }
  }
}

//will update toggle status of each output pin
int updatePinData(int index) {
  if (index != 0 && pin_data[0] == 0) {
    //if other pins is requesting but the main switch is off do not turn on
    return 0;
  }
  if (pin_data[index] == 0) {
    pin_data[index] = 1;
    return 1;
  }
  pin_data[index] = 0;
  return 0;
}

//cut the power to led indicators if main switch is off
//but do not reset pin status
boolean cutPower() {
  int main_status = pin_data[0];
  if (!main_status) {
    //LOW turned off
    for (int i = 1; i < 5; i++) {
      digitalWrite(pins[i], main_status);
    }
    return true;
  }
  //  //bring back all pin status
  for (int i = 1; i < 5; i++) {
    digitalWrite(pins[i], pin_data[i]);
  }
  return false;
}

void loop() {
  if (IrReceiver.decode()) {
    switch (IrReceiver.decodedIRData.decodedRawData) {

      case main_switch: //main switch
        //Serial.println(IrReceiver.decodedIRData.decodedRawData);
        digitalWrite(pins[0], updatePinData(0));
        cutPower();
        break;
      case channel_1: //channel 1
        //Serial.println(IrReceiver.decodedIRData.decodedRawData);
        digitalWrite(channel1_pin, updatePinData(1));
        break;
      case channel_2: //channel 2
        //Serial.println(IrReceiver.decodedIRData.decodedRawData);
        digitalWrite(pins[2], updatePinData(2));
        break;
      case channel_3: //channel 3
        //Serial.println(IrReceiver.decodedIRData.decodedRawData);
        digitalWrite(pins[3], updatePinData(3));
        break;
      case channel_4: //channel 4
        //Serial.println(IrReceiver.decodedIRData.decodedRawData);
        digitalWrite(pins[4], updatePinData(4));
        break;
      case turn_off_all:
        //Serial.println(IrReceiver.decodedIRData.decodedRawData);
        on_channels = 0;
        for (int i = 1; i < 5; i++) {
          if (pin_data[i] == 1) {
            on_channels++;
          }
        }
        if (on_channels > 0) {
          //if one is detected turn off all
          modify_all_channels(0); //off all
        } else {
          modify_all_channels(1); //on all
        }
        break;
      default:
        break;
    }
    IrReceiver.resume();
  }
}
