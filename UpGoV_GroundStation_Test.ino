// UpGoV Ground Station Test
// Version: 1.02
// Author: Jonah Parker
// Date: 12/12/2023


/// ==============================  Include Files  ==================================
#include <RH_RF95.h>                  // Radio Head driver class for LoRa Radio
#include <RHReliableDatagram.h>       // Radio Head manager class for reliable comms
#include <SPI.h>                      // Radio communications serial bus

// ===============================  Constants  =====================================
const char* VERSION = "1.02";
const uint8_t RADIO_SPI_CS = 8;
const uint8_t RADIO_SPI_IRQ = 3;
const double RF95_FREQ = 915.0;
const uint8_t MAX_MESSAGE_LENGTH = 20;    // Maximum radio message length
const uint8_t UPGOV_ADDR = 2;             // Radio address for UpGoV avionics package
const uint8_t GROUND_STATION_ADDR = 1;    // Radio address for the Ground Station
const uint8_t RFM95_RST = 4;

// =======================  Global Variables/Objects  ===============================
// Driver Class Objects
RH_RF95 radioDriver = RH_RF95(RADIO_SPI_CS, RADIO_SPI_IRQ); // LoRa radio driver object
RHReliableDatagram radioManager(radioDriver, UPGOV_ADDR);   // LoRa radio manager object

// Other global variables
uint8_t buffer[MAX_MESSAGE_LENGTH]; // Message buffer

void setup() {
    // Initialize the Serial Console interface (used for debugging only)

  Serial.begin(115200); // Make sure the Serial Monitor is set to 115200 baud
  while (!Serial) {
      ; 
  }
  Serial.println("UpGoer5 Intrumentation");
  Serial.print("Version: ");
  Serial.println(VERSION);

  // Configure GPIO Pins

  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(20);

    // ========================== LoRa Radio Initialization ============================
  // Reset the LoRa radio
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // Initialize the LoRa radio
  if (!radioManager.init()) {

    Serial.println("Radio init() error");

  }

  // Set the LoRa radio operating frequency
  if (!radioDriver.setFrequency(RF95_FREQ)) {
 
    Serial.println("Failed to set LoRa radio frequency");
 
  }
  // Set the LoRa radio transmit power
  radioDriver.setTxPower(23, false);
 
  Serial.println("Radio initialized");
  Serial.println("Waiting for ground station connect");
 

  radioManager.setThisAddress(UPGOV_ADDR);

  // Block here until we connect to the UpGoV Ground Station
  bool connected = false;
  uint8_t messageLength;
  uint8_t from;
  uint8_t to;
  while (!connected) {
    messageLength = sizeof(buffer);
    if (radioManager.recvfromAck(buffer, &messageLength, &from, &to)) {
      // Message received. Is it to us?
      Serial.println("Radio message received");
      if (to == UPGOV_ADDR) {
        // The message is to us. Is it from the ground station?
        if (from == GROUND_STATION_ADDR) {
          // Message is from the ground station. Is it a connect message?
          buffer[messageLength] = 0;  // Terminate with a null character
          if (!strncmp((char*)buffer, "connect", 7)) {
            // Connect message received. Send connect message reply
            uint8_t data[] = "connect";
            if (radioManager.sendtoWait(data, sizeof(data), GROUND_STATION_ADDR)) {
              connected = true;
            }
          }
        }
      }
    }
  } // End radio connect loop

  delay(500);

}


void loop() {
  // put your main code here, to run repeatedly:

}
