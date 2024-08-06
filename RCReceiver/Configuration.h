/**
 * @file Configuration.h
 * @author Marcelo Fraga
 * @brief Configuration file for a generic RC receiver.
 * @version 0.1
 * @date 2022-11-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <Arduino.h>


/*
* Feature availability
*/

#define OFF 0u
#define ON  1u

#define DEBUG OFF


/* 
 *  PIN Definitions  
 */

#define RF24_CSN_PIN              8  // When CSN is low, module listens on SPI port
#define RF24_CE_PIN               9  // CE Selects whether transmit or receive
#define RF24_MOSI_PIN             11 // SPI input. Used to receive data from uC
#define RF24_MISO_PIN             12 // SPI output. Used to send data to uC
#define RF24_SCK_PIN              13 // Serial clock


// Pin definitions for arduino pro micro, testing purposes
// #define RF24_CSN_PIN              8  // When CSN is low, module listens on SPI port
// #define RF24_CE_PIN               9  // CE Selects whether transmit or receive
// #define RF24_MOSI_PIN             16 // SPI input. Used to receive data from uC
// #define RF24_MISO_PIN             14 // SPI output. Used to send data to uC
// #define RF24_SCK_PIN              15 // Serial clock


/*
* NRF24L01 RFCom related
*/

#define RF_ADDRESS_SIZE 3u
const byte RF_Address[RF_ADDRESS_SIZE] = "FG";






/*
* Channel configuration
*/

#define N_CHANNELS             8u // Channels are exactly the same as the remote here. The physical outputs however, might be less
#define N_PHYSICAL_CHANNELS    6u // Channels are exactly the same as the remote here. The physical outputs however, might be less
#define N_ANALOG_CHANNELS      3u
#define FIRST_PHYSICAL_CHANNEL 2u // Currently defined like this becuse the other channels are just after this one. Not the best approach though
#define FIRST_ANALOG_CHANNEL   A0 // Currently defined like this becuse the other channels are just after this one. Not the best approach though



/* Payload definition */

typedef struct RFPayload{
  uint16_t u16_Channels[N_CHANNELS];
}RFPayload;

/*
* 
**/

#define ANALOG_MAX_VALUE 1023
#define ANALOG_MIN_VALUE 0
#define ANALOG_HALF_VALUE 512

#define PWM_MAX_MICROSECONDS 2000
#define PWM_MIN_MICROSECONDS 1000




#endif