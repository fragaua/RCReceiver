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

#define ANALOG_MAX_VALUE 1023
#define ANALOG_MIN_VALUE 0
#define ANALOG_HALF_VALUE 512

#define PWM_MAX_MICROSECONDS 2000
#define PWM_MIN_MICROSECONDS 1000


typedef struct RFPayload{
  uint16_t u16_Channels[N_CHANNELS];
}RFPayload;


typedef struct ChannelOutput_t
{
  uint8_t u8_Pin;
  Servo   output;
}ChannelOutput_t;

ChannelOutput_t Receiver_Output[N_PHYSICAL_CHANNELS];

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);
RFPayload payload;

void initPayload(RFPayload* pl)
{
  uint8_t idx;
    for(idx = 0; idx < N_CHANNELS; idx++)
    {
      pl->u16_Channels[idx] = ANALOG_HALF_VALUE;
    }
}

void setup() {
  Serial.begin(115200);
  
  initPayload(&payload);

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
    Receiver_Output[i].output.attach(Receiver_Output[i].u8_Pin);
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

  for(i = 0; i < N_PHYSICAL_CHANNELS; i++)
  {
    Receiver_Output[i].output.writeMicroseconds(map(payload.u16_Channels[i], ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, PWM_MIN_MICROSECONDS, PWM_MAX_MICROSECONDS));
  }

}
