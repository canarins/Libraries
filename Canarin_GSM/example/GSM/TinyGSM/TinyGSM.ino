/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/lilygo-t-sim7000g-esp32-lte-gprs-gps/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Original code: https://github.com/Xinyuan-LilyGO/LilyGO-T-SIM7000G/blob/master/examples/Arduino_NetworkTest/Arduino_NetworkTest.ino

#define TINY_GSM_MODEM_SIM7080
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#define SerialAT Serial1
// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[]  = "TM";     //SET TO YOUR APN
const char gprsUser[] = "";
const char gprsPass[] = "";

#include <TinyGsmClient.h>
#include <SPI.h>
#include <SD.h>
#include <Ticker.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

// LilyGO T-SIM7000G Pinout
#define UART_BAUD           115200
#define PIN_DTR             25
#define PIN_TX              27
#define PIN_RX              26
#define PWR_PIN             4

#define SD_MISO             2
#define SD_MOSI             15
#define SD_SCLK             14
#define SD_CS               13
#define LED_PIN             12


void modemPowerOn(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1000);
  digitalWrite(PWR_PIN, HIGH);
}

void modemPowerOff(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1500);
  digitalWrite(PWR_PIN, HIGH);
}


void modemRestart(){
  modemPowerOff();
  delay(1000);
  modemPowerOn();
}

void setup(){
  // Set console baud rate
  SerialMon.begin(115200);

  delay(10);

  // Set LED OFF
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  modemPowerOn();

  logger.println("========SDCard Detect.======");
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
  if (!SD.begin(SD_CS)) {
      logger.println("SDCard MOUNT FAIL");
  } else {
    uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    String str = "SDCard Size: " + String(cardSize) + "MB";
    logger.println(str);
  }
  logger.println("===========================");

  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);


  logger.println("/**********************************************************/");
  logger.println("To initialize the network test, please make sure your LTE ");
  logger.println("antenna has been connected to the SIM interface on the board.");
  logger.println("/**********************************************************/\n\n");

  delay(10000);
}

void loop(){
  String res;

  logger.println("========INIT========");

  if (!modem.init()) {
    modemRestart();
    delay(2000);
    logger.println("Failed to restart modem, attempting to continue without restarting");
    return;
  }

  logger.println("========SIMCOMATI======");
  modem.sendAT("+SIMCOMATI");
  modem.waitResponse(1000L, res);
  res.replace(GSM_NL "OK" GSM_NL, "");
  logger.println(res);
  res = "";
  logger.println("=======================");

  logger.println("=====Preferred mode selection=====");
  modem.sendAT("+CNMP?");
  if (modem.waitResponse(1000L, res) == 1) {
    res.replace(GSM_NL "OK" GSM_NL, "");
    logger.println(res);
  }
  res = "";
  logger.println("=======================");


  logger.println("=====Preferred selection between CAT-M and NB-IoT=====");
  modem.sendAT("+CMNB?");
  if (modem.waitResponse(1000L, res) == 1) {
    res.replace(GSM_NL "OK" GSM_NL, "");
    logger.println(res);
  }
  res = "";
  logger.println("=======================");


  String name = modem.getModemName();
  logger.println("Modem Name: " + name);

  String modemInfo = modem.getModemInfo();
  logger.println("Modem Info: " + modemInfo);

  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }

  for (int i = 0; i <= 4; i++) {
    uint8_t network[] = {
        2,  /*Automatic*/
        13, /*GSM only*/
        38, /*LTE only*/
        51  /*GSM and LTE only*/
    };
    logger.printf("Try %d method\n", network[i]);
    modem.setNetworkMode(network[i]);
    delay(3000);
    bool isConnected = false;
    int tryCount = 60;
    while (tryCount--) {
      int16_t signal =  modem.getSignalQuality();
      logger.print("Signal: ");
      logger.print(signal);
      logger.print(" ");
      logger.print("isNetworkConnected: ");
      isConnected = modem.isNetworkConnected();
      logger.println( isConnected ? "CONNECT" : "NO CONNECT");
      if (isConnected) {
        break;
      }
      delay(1000);
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
    if (isConnected) {
        break;
    }
  }
  digitalWrite(LED_PIN, HIGH);

  logger.println();
  logger.println("Device is connected .");
  logger.println();

  logger.println("=====Inquiring UE system information=====");
  modem.sendAT("+CPSI?");
  if (modem.waitResponse(1000L, res) == 1) {
    res.replace(GSM_NL "OK" GSM_NL, "");
    logger.println(res);
  }

  logger.println("/**********************************************************/");
  logger.println("After the network test is complete, please enter the  ");
  logger.println("AT command in the serial terminal.");
  logger.println("/**********************************************************/\n\n");

  while (1) {
    while (SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
    while (SerialMon.available()) {
      SerialAT.write(SerialMon.read());
    }
  }
}