// UpGoV Ground Station Test
// Version: 1.01
// Author: Jonah Parker
// Date: 12/11/2023


/// ==============================  Include Files  ==================================
#include <RH_RF95.h>                  // Radio Head driver class for LoRa Radio
#include <RHReliableDatagram.h>       // Radio Head manager class for reliable comms
#include <SPI.h>                      // Radio communications serial bus

// ===============================  Constants  =====================================
const char* VERSION = "1.01";
const uint8_t RADIO_SPI_CS = 8;
const uint8_t RADIO_SPI_IRQ = 3;
const double RF95_FREQ = 915.0;
const uint8_t MAX_MESSAGE_LENGTH = 20;    // Maximum radio message length
const uint8_t UPGOV_ADDR = 2;             // Radio address for UpGoV avionics package
const uint8_t GROUND_STATION_ADDR = 1;    // Radio address for the Ground Station

// =======================  Global Variables/Objects  ===============================
// Driver Class Objects
RH_RF95 radioDriver = RH_RF95(RADIO_SPI_CS, RADIO_SPI_IRQ); // LoRa radio driver object
RHReliableDatagram radioManager(radioDriver, UPGOV_ADDR);   // LoRa radio manager object

// Other global variables

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
