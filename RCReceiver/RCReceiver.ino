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
#define N_PHYSICAL_CHANNELS 6u // Channels are exactly the same as the remote here. The physical outputs however, might be less
#define FIRST_CHANNEL 2u // Currently defined like this becuse the other channels are just after this one. Not the best approach though

typedef struct RFPayload{
  uint16_t u16_Channels[N_CHANNELS];
}RFPayload;


typedef struct ChannelOutput_t
{
  uint8_t u8_Pin;
  Servo   e_Output;
  bool    b_ServoType; /* True for servos and outputs controlled via 0-180 angle. False for ESCs and others controlled by 50Hz pwm (1to2us period)*/
}ChannelOutput_t;

bool Output_Channel_Types[N_PHYSICAL_CHANNELS] = {false, false, true, true, true, true}; // Hardcoded, first two channels are ESC like channels.
ChannelOutput_t Receiver_Output[N_PHYSICAL_CHANNELS];

RF24 radio(9, 8); // use a different set of signals so we liberate one of the PWM outputs
RFPayload payload;

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT);
  /* Initialize radio*/
  if (!radio.begin()) 
  {
    Serial.println("Failed");
    while (1) 
    {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);     
    } 
  }
  

  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(RFPayload));
  radio.openReadingPipe(0, RF_Address);
  radio.startListening();

   
  /* Initialize output channels with their type and pin. Attach the servo object */
  uint8_t i;
  for(i = 0; i < N_PHYSICAL_CHANNELS; i++)
  {
    Receiver_Output[i].u8_Pin = FIRST_CHANNEL + i;
    Receiver_Output[i].e_Output.attach(Receiver_Output[i].u8_Pin);
    Receiver_Output[i].b_ServoType = Output_Channel_Types[i];
  }
}

uint8_t i;
bool    led_debug;
void loop() {
   // TODO: Use internal LED to give some information on the communication state
    uint8_t pipe;
    if (radio.available(&pipe)) {              // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
      radio.read(&payload, bytes);             // fetch payload from FIFO
    }
    else
    { 
      // TODO: Count "not available" events and eventually flash built in led
    }

  // TODO: Create function to translate from receiving channel to the receiver output
  for(i = 0; i < N_PHYSICAL_CHANNELS; i++)
  {
    Receiver_Output[i].e_Output.writeMicroseconds(map(payload.u16_Channels[i], 0, 1023, 1000, 2000));
  }

}
