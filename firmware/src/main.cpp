#include <Arduino.h>
#include "RCSwitch.h"
#define TX 2

RCSwitch mySwitch = RCSwitch();

uint8_t messageID = 0;

// CLK, PULSE, BTN
uint8_t encoders[3][3] = {
  {3,4,5},
  {6,7,8},
  {9,10,14}
};

// CLK, BTN
bool last[3][2] ={
  {0,0},
  {0,0},
  {0,0}
};

void signMessage(const char* message, char* result) {
  uint8_t index = 0;
  for (int i = 2; i >= 0; i--) {
    if (bitRead(messageID, i)) {
      result[index] = '1';
      index++;
    } else {
      result[index]= '0';
      index++;
    }
  }
  
  for (int i = 0; i < 13; i++) {
    result[index] = message[i];
    index++;
  }
}

void sendState(bool clockwise, bool buttonPressed){
  char message[13] = {'0','0','0','0','0','0','0','0','0','0','0','0','0'};
  if(buttonPressed){
    signMessage("0001011001", message);
  }else if(clockwise){
    signMessage("0001100110", message);
  }else{
    signMessage("0001110011", message);
  }

  mySwitch.send(message);
  messageID = (messageID + 1) % 8;
  delay(11);
}

void setup() {
  for(uint8_t i = 0; i < 3; i++){
    pinMode(encoders[i][0],INPUT);
    pinMode(encoders[i][1],INPUT);
    pinMode(encoders[i][2], INPUT);
  }
  
  mySwitch.enableTransmit(TX);
  mySwitch.setRepeatTransmit (4);
  mySwitch.setProtocol(1);
}

void loop() {
  for(uint8_t i = 0; i < 1; i++){

    bool clkRead = digitalRead(encoders[i][0]);
    bool pulseRead = digitalRead(encoders[i][1]);
    bool buttonRead = digitalRead(encoders[i][2]);
    bool lastClkRead = last[i][0];
    bool lastBtnRead = last[i][1];
    if(!clkRead && lastClkRead && i == 0){
      if(pulseRead){
        sendState(true, false); // Clockwise
      }else{
        sendState(false, false); // Counter-Clockwise
      }
    }
    if(!buttonRead && !lastBtnRead && i == 0){
      sendState(false, true); // Button Press
      delay(300);
    }
    last[i][0] = clkRead;
    last[i][1] = buttonRead;
  }
}
