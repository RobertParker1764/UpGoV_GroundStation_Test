// UpGoV Ground Station Test
// Version: 1.0.3
// Author: Jonah Parker
// Date: 12/14/2023


/// ==============================  Include Files  ==================================
#include <RH_RF95.h>                  // Radio Head driver class for LoRa Radio
#include <RHReliableDatagram.h>       // Radio Head manager class for reliable comms
#include <SPI.h>                      // Radio communications serial bus

// ===============================  Constants  =====================================
const char* VERSION = "1.0.3";
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
uint8_t buf[MAX_MESSAGE_LENGTH]; // Message buffer
char radioPacket[20];            // Buffer containing radio message
int radioError = 0;              // The number of radio messages not receiving an ack

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
    if (radioManager.available()) {
      // Wait for a message addressed to us
      uint8_t len = sizeof(buf);
      uint8_t from;
      if (radioManager.recvfromAck(buf, &len, &from)) {
        // There is a message addressed to us. Check if it is from the ground station
        if (from == GROUND_STATION_ADDR) {
          // The message is from the Ground Station. Is it a "connect" message?
          if (!strncmp((char*)buf, "connect", 7)) {
            // It is a "connect" message. Send a "connect" reply.
            uint8_t data[] = "connect";
            if (radioManager.sendtoWait(data, sizeof(data), GROUND_STATION_ADDR)) {
              connected = true;
            }
          }
        }
      }
    }
  } // End radio connect loop

  delay(2000);

  Serial.println("Sending first message");
  sendRadioMessage("MS:MSG 1", GROUND_STATION_ADDR);
  delay(5000);
}


void loop() {
  // put your main code here, to run repeatedly:

}

//=================== sendRadioMessage ======================
// Sends a radio message to the specified address
// Parameters:
//  message:  The radio message that is to be sent
//  address: The address the message is to be sent to
// Return: True if an acknowledgment was received
//=================================================================
bool sendRadioMessage(const char * message, uint8_t address) {
    strncpy(radioPacket, message, sizeof(radioPacket));
  if (radioManager.sendtoWait((uint8_t *)radioPacket, sizeof(radioPacket), address)) {
#ifdef DEBUG
  Serial.print("Radio message sent: ");
  Serial.println(message);
#endif
  return true;
  } else {
    radioError++;
#ifdef DEBUG
    Serial.println("No message ack");
    Serial.print("Radio Error #: ");
    Serial.println(radioError);
#endif
    return false;
  }
}
