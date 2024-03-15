/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi network name and password:
const char* networkName = "Letmeknow";
const char* networkPswd = "letmeknow75002";

//IP address to send UDP data to:
// either use the ip address of the server or
// a network broadcast address
const char* udpAddress = "192.168.0.255";
const int udpPort = 3333;

const char* udpAddress2 = "18.184.137.24";
const int udpPort2 = 60004;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

void setup() {
  // Initilize hardware serial:
  Serial.begin(115200);

  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
}

char* RESPONSE;

void loop() {
  //only send data when connected
  RESPONSE = new char[64];
  RESPONSE[0] = '\0';
  if (connected) {
    //Send a packet
    logger.println("Sending Data: " + String(udpAddress2) + " " + String(udpPort2));
    udp.beginPacket(udpAddress2, udpPort2);
    udp.printf("{0,0,12}");
    udp.endPacket();
    logger.println("Data sent");
    //delay(1000);



    unsigned long startTime = millis();
    bool packetReceived = false;
    while (millis() - startTime < 1000) {  // Check for up to 1 second if this timing is necessary
      if (udp.parsePacket()) {
        packetReceived = true;
        break;  // Exit the loop if a packet is received
      }
    }

    if (packetReceived) {
      int c = 0;
      while (udp.available()) {
        RESPONSE[c] = static_cast<char>(udp.read());
        logger.print(F("UDP available: "));
        logger.println(udp.available());
        logger.print(F("TEMP RESPONSE: "));

        // Directly print characters without creating temporary String objects
        for (int i = 0; i <= c; ++i) {
          logger.print(RESPONSE[i]);
        }
        logger.print(' ');
        logger.println(RESPONSE[c]);
        c++;
      }

      // Null-terminate the string if RESPONSE is a char array
      RESPONSE[c] = '\0';

      logger.print(F("RESPONSE: "));
      logger.println(RESPONSE);
    } else {
      logger.println(F("No UDP packet received within the timeout period."));
    }


  }

    delete[] RESPONSE;
  }

  void connectToWiFi(const char* ssid, const char* pwd) {
    logger.println("Connecting to WiFi network: " + String(ssid));

    // delete old config
    WiFi.disconnect(true);
    //register event handler
    WiFi.onEvent(WiFiEvent);  // Will call WiFiEvent() from another thread.

    //Initiate connection
    WiFi.begin(ssid, pwd);

    logger.println("Waiting for WIFI connection...");
  }

  // WARNING: WiFiEvent is called from a separate FreeRTOS task (thread)!
  void WiFiEvent(WiFiEvent_t event) {
    switch (event) {
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        //When connected set
        logger.print("WiFi connected! IP address: ");
        logger.println(WiFi.localIP());
        //initializes the UDP state
        //This initializes the transfer buffer
        udp.begin(WiFi.localIP(), udpPort);
        connected = true;
        break;
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        logger.println("WiFi lost connection");
        connected = false;
        break;
      default: break;
    }
  }
