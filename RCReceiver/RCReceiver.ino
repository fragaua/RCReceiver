// #include <printf.h>
#include "PinDefinitions.h"
#include <RF24.h>
#include <Arduino.h>
#include <Servo.h>


/*
* NRF24L01 RFCom related
*/

#define DEBUG_MODE   0 // Debug mode prints all variables from the received payload.
#define NORMAL_MODE  1 

// Define what is the running mode.
#define RUN_MODE NORMAL_MODE

#define RF_ADDRESS_SIZE 6
const byte RF_Address[RF_ADDRESS_SIZE] = "1Node";


#define N_CHANNELS 7u // Channels are exactly the same as the remote here. The physical outputs however, might be less

typedef struct RFPayload{
  uint16_t u16_Channels[N_CHANNELS];
}RFPayload;



RF24 radio(9, 8); // use a different set of signals so we liberate one of the PWM outputs
Servo servo;

RFPayload payload;


void setup() {
  Serial.begin(115200);
  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(RFPayload));
  radio.openReadingPipe(0, RF_Address);
  radio.startListening();

  servo.attach(3);
}

int i = 0;
int cnt = 0;
uint16_t motor_run, servo_turn;
void loop() {
   // TODO: Use internal LED to give some information on the communication state
    uint8_t pipe;
    if (radio.available(&pipe)) {              // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
      radio.read(&payload, bytes);             // fetch payload from FIFO
      // Serial.print(F("Received "));
      // Serial.print(bytes);  // print the size of the payload
      // Serial.println();
      // Serial.print(F(" bytes on pipe "));
      //Serial.print(pipe);  // print the pipe number
      //Serial.print(F(": "));
      if(cnt % 20 == 0)
      {
        uint8_t i;
        for(i = 0; i < N_CHANNELS; i++)
        {
          Serial.print("Channel ");
          Serial.print(i);
          Serial.print(" :");
          Serial.println(payload.u16_Channels[i]);
        }
          Serial.println("\n\n\n");
      }
      cnt++;
    }
    else{
      // Serial.println("Com Broken");
      // delay(1000);
      // payload.d_Joystick_Left.u16_Value_X = 509;
    }
//
    servo_turn = map(payload.u16_Channels[4], 0, 1023, 0, 180);
    servo.write(servo_turn);

  delay(5);
}
