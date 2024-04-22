#include "PinDefinitions.h"
#include <RF24.h>
#include <Arduino.h>
#include <Servo.h>
#include <SoftPWM.h>



/*
* NRF24L01 RFCom related
*/

#define RF_ADDRESS_SIZE 6
const byte RF_Address[RF_ADDRESS_SIZE] = "1Node";

#define N_CHANNELS             7u // Channels are exactly the same as the remote here. The physical outputs however, might be less
#define N_PHYSICAL_CHANNELS    6u // Channels are exactly the same as the remote here. The physical outputs however, might be less
#define N_ANALOG_CHANNELS      3u
#define FIRST_PHYSICAL_CHANNEL 2u // Currently defined like this becuse the other channels are just after this one. Not the best approach though
#define FIRST_ANALOG_CHANNEL   A0 // Currently defined like this becuse the other channels are just after this one. Not the best approach though


#define ANALOG_MAX_VALUE 1023
#define ANALOG_MIN_VALUE 0
#define ANALOG_HALF_VALUE 512

#define PWM_MAX_MICROSECONDS 2000
#define PWM_MIN_MICROSECONDS 1000


typedef struct RFPayload{
  uint16_t u16_Channels[N_CHANNELS];
}RFPayload;


/* The actual physical channels on the receiver side, used to control any module using 50Hz PWM Control*/
typedef struct ChannelOutput_t
{
  uint8_t u8_Pin;
  Servo   output;
}ChannelOutput_t;

/* Version 1 of the Receiver PCB contains three Analog channels for general purpose use. Currently, we use SoftPWM on them.
   For this particular define, we assume the AnalogOutput_t as simply being the pin corresponding to the actual output */
typedef uint8_t AnalogOutput_t;


ChannelOutput_t Receiver_Output[N_PHYSICAL_CHANNELS];
AnalogOutput_t  Receiver_Analog_Output[N_ANALOG_CHANNELS];

RF24 radio(RF24_CE_PIN, RF24_CSN_PIN);
RFPayload payload;

void initPayload(RFPayload* const pl)
{
  uint8_t idx;
  for(idx = 0; idx < N_CHANNELS; idx++)
  {
    pl->u16_Channels[idx] = ANALOG_HALF_VALUE;
  }
}

void initPhysicalChannels(ChannelOutput_t* pChannelOutput)
{
  /* Initialize output channels with their type and pin. Attach the servo object */
  uint8_t i;
  for(i = 0; i < N_PHYSICAL_CHANNELS; i++)
  {
    pChannelOutput[i].u8_Pin = FIRST_PHYSICAL_CHANNEL + i;
    pChannelOutput[i].output.attach(pChannelOutput[i].u8_Pin);
  }
}

void initAnalogChannels(AnalogOutput_t* pAnalogOutput)
{
  SoftPWMBegin();
  uint8_t i;
  for(i = 0; i < N_ANALOG_CHANNELS; i++)
  {
    pAnalogOutput[i] = FIRST_ANALOG_CHANNEL + i;  
    SoftPWMSet(pAnalogOutput[i], 0);
  }

}

bool receivePayload(RFPayload* pl, RF24* pRadio)
{
  uint8_t pipe;
  if (pRadio->available(&pipe)) {             
    uint8_t bytes = pRadio->getPayloadSize();  
    pRadio->read(pl, bytes);                 
    return true;
  }

  return false;

}


void writePhysicalChannels(const RFPayload* const pl, ChannelOutput_t* pChannelOutput)
{
  uint8_t i;
  // Normal physical channels get controlled just like any comercial receiver, with 50Hz pwm control
  for(i = 0; i < N_PHYSICAL_CHANNELS; i++)
  {
    pChannelOutput[i].output.writeMicroseconds(map(pl->u16_Channels[i], ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, PWM_MIN_MICROSECONDS, PWM_MAX_MICROSECONDS));
  }

} 


void writeAnalogChannels(const RFPayload* const pl, AnalogOutput_t* pAnalogOutput)
{
  uint8_t i;
  // For now, on the first version of the receiver pcb, the A0, A1 and A2 pins are connected to 3 outputs pins.
  // Let's use these pins for barebones motor control using the SoftPWM library, to use on simpler projects.

  for(i = 0; i < N_ANALOG_CHANNELS; i++)
  {
    int8_t i8_pwm = map(payload.u16_Channels[i], ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, -100, 100);
    uint8_t u8_polarity = (i8_pwm >= 0) ? SOFTPWM_NORMAL : SOFTPWM_INVERTED;

    SoftPWMSetPolarity(pAnalogOutput[i], u8_polarity);
    SoftPWMSetPercent (pAnalogOutput[i], abs(i8_pwm));
  }

} 



void writePayload(RFPayload* pl, ChannelOutput_t* pChannelOutput, AnalogOutput_t* pAnalogOutput)
{
  writePhysicalChannels(pl, pChannelOutput);
  writeAnalogChannels(pl, pAnalogOutput);
}





void setup() {
  Serial.begin(115200);
  
  initPayload(&payload);
  initPhysicalChannels(Receiver_Output);
  initAnalogChannels(Receiver_Analog_Output);

  pinMode(LED_BUILTIN, OUTPUT);
  
  /* Initialize radio*/
  if (!radio.begin()) 
  {
    Serial.println("Failed");
    while (1) 
    {
      digitalWrite(LED_BUILTIN, HIGH);   
      delay(1000);                       
      digitalWrite(LED_BUILTIN, LOW);
      delay(1000);     
    } 
  }
  

  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(RFPayload));
  radio.openReadingPipe(0, RF_Address);
  radio.startListening();

}

void loop() 
{
  receivePayload(&payload, &radio);
  writePayload(&payload, Receiver_Output, Receiver_Analog_Output);

  delay(50);
}
