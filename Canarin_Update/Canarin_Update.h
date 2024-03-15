

#ifndef CANARIN_UPDATE_H
#define CANARIN_UPDATE_H

#include <Update.h>


void performUpdate(Stream &updateSource, size_t updateSize) {
  if (Update.begin(updateSize)) {
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      logger.println("Writes : " + String(written) + " successfully");  //OTA WORKED
    } else {
      logger.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");  //OTA didnt WORKED
    }
    if (Update.end()) {
      logger.println("OTA finished!");
      if (Update.isFinished()) {
        listDir(SPIFFS,"/",2);
        deleteFile(SPIFFS,"/update.bin");
        listDir(SPIFFS,"/",2);
        logger.println("Restart ESP device!");
        ESP.restart();
      } else {
        logger.println("OTA not finished");
      }
    } else {
      logger.println("Error occured #: " + String(Update.getError()));
      logger.println("Restart ESP device! in 3s");
      delay(3000);
      ESP.restart();
    }
  } else {
    logger.println("Cannot beggin update");
  }
}


void updateFromFS() {
  File updateBin = SPIFFS.open("/update.bin");
  if (updateBin) {
    if (updateBin.isDirectory()) {
      logger.println("Directory error");
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (updateSize > 0) {
      logger.println("Starting update");
      performUpdate(updateBin, updateSize);
    } else {
      logger.println("Error, Fichier vide");
    }

    updateBin.close();

    // whe finished remove the binary from sd card to indicate end of the process
    //fs.remove("/update.bin");
  } else {
    logger.println("no such binary");
  }
}
#endif