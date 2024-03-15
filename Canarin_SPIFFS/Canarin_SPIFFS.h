

#ifndef CANARIN_SPIFFS_H_
#define CANARIN_SPIFFS_H_

#include <Canarin_Utilities.h>
#include "FS.h"
#include "SPIFFS.h"

#include <Canarin_File.h>

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

bool spiffs_setup = false;

void Canarin_SPIFFS_setup(){

  Serial.println("TOTO");logger.println(spiffs_setup);Serial.println("TOTO");

if (spiffs_setup)
return;

Serial.println("TOTO");logger.println(FORMAT_SPIFFS_IF_FAILED);Serial.println("TOTO");

    if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    logger.println("An error has occurred while mounting SPIFFS");
    // Optionally, format SPIFFS if mounting fails
    logger.println("Formatting SPIFFS");
    if (SPIFFS.format()) {
      logger.println("SPIFFS formatted successfully");
      // Try to mount SPIFFS again after formatting
      if (SPIFFS.begin()) {
        logger.println("SPIFFS mounted successfully after formatting");
      } else {
        logger.println("Failed to mount SPIFFS even after formatting");
      }
    } else {
      logger.println("Failed to format SPIFFS");
    }
  } else {
    logger.println("SPIFFS mounted successfully");
  }
spiffs_setup = true;
}


#endif