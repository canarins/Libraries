
#ifndef CANARIN_SETUP_H
#define CANARIN_SETUP_H
#include "parameter.h"

#include <Canarin_Utilities.h>
#include <Canarin_Configuration.h>
#include <time.h> // ESP32-S3 RTC library (example library name)

unsigned long myTime; // TODO check best Time source

// #include "timeout.h"

#include <Canarin_Connect.h>

#include <Canarin_File.h>

#include <Canarin_Update.h>
#include <Canarin_Sampling.h>


bool first_loop = true;

#include "Canarin_Tasks.h"

// #include "FS.h"
// #include "OTA_Canarin.h"

bool previously_charging = true;

int cycle_length = 0;

#define DEBUG_CANARIN_SETUP true

unsigned long stop_cycle_time, start_cycle_time;
int last_operation_time = 60000;
int Counter_loop = 0;

void Flash_Canarin_setup()
{
  return;
}

void OTA_Canarin_setup()
{
  return;
}

void setup_prim_part_1()
{
  Canarin_Connectic_setup();
  Canarin_SPIFFS_setup();
}

void MPU_Canarin_setup()
{ // TODO update
  return;
}

void BME280_Canarin_setup()
{ // TODO UPDATE
  return;
}

// #include "GPS_Canarin.h"

// bool setup_GPS = false;



void setup_Primary()
{

Canarin_Utilities_setup();
  logger.println("Setup Primary");

  myTime = millis();
  logger.println(myTime);


  logger.println( " Default  " );

  setup_prim_part_1();
  logger.println("--------------------------------------------------------");
  logger.println("IMEI");
  // WiFi.macAddress(mac);
  String imei = modem.getIMEI();
  logger.println(imei);
  snprintf(NODE_ID, 16, "%s", imei.c_str());
  logger.println(NODE_ID);

  logger.println("--------------------------------------------------------");
  logger.println("GET CONFIGURATION FROM FLASH");
  logger.println("--------------------------------------------------------");



  printDeviceConfig();
  printFile(SPIFFS, "/Configuration.txt");

  File configFile = SPIFFS.open("/Configuration.txt", "r");
  if (configFile && configFile.size() == 0)
  {
    logger.println("CONFIGURATION FILE EMPTY");
    // If the file is empty, serialize and write the device configuration
    char config_char[1024] = {'\0'};
    serializeDeviceConfig(config_char);
    logger.println(config_char);
    logger.println(sizeof(config_char));
    writeFile(SPIFFS, "/Configuration.txt", config_char);
  }
  else
  {
    // If the file isn't empty, deserialize the device configuration
    logger.println("CONFIGURATION FILE EXISTS");
    char config_char2[1024];
    char *readfile = readFile(SPIFFS, "/Configuration.txt");

    strncpy(config_char2, readfile, strlen(readfile));
    config_char2[strlen(readfile)] = '\0';
    deserializeDeviceConfig(config_char2);
    logger.println(config_char2);
  }
  configFile.close();
  printDeviceConfig();

  logger.println("--------------------------------------------------------");
  logger.println("UPDATE FROM FS if ");
  logger.println("--------------------------------------------------------");
  // getconfiguration from flash


 

  // Wait one second for sensor to boot up!
  delay(1000);

  if (PMU.isCharging())
  { // TODO replace by is_charging()

    updateFromFS();
  }

  logger.println("SETUP END: ");

  //modem.sendAT("+CNACT?");
  //int responseCode = modem.waitResponse();

  //logger.println(responseCode);
}

#endif