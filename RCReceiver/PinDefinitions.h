/**
 * @file PinDefinitions.h
 * @author Marcelo Fraga
 * @brief Arduino pinout definition for the receiver module of the Ferrari RC Project.
 *        This is project specific. There is a different pinout definition for the transmitter part.
 * @version 0.1
 * @date 2022-11-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#ifndef PINDEFINITIONS_H
#define PINDEFITIONS_H

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


#endif