
#include <Canarin_Utilities.h>
#include <Canarin_Sampling.h>
#include <Canarin_SPIFFS.h>
#include "extra.h"
#include "parameter.h"

SemaphoreHandle_t spiffsSemaphore;

TaskHandle_t TaskCommunication;
TaskHandle_t TaskSampling;

char arrayBattery[16];
byte mac[6];

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  spiffsSemaphore = xSemaphoreCreateMutex();
  WiFi.macAddress(mac);

  snprintf(NODE_ID, sizeof(NODE_ID), "%02X_%02X_%02X_%02X_%02X_%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  logger.println(NODE_ID);
  // NODE_ID =

  setup_Sampling();

  Canarin_SPIFFS_setup();
  // SPIFFS.format();
  listDir(SPIFFS, "/", 2);
}

// #define DEBUG_LOGG_SAMPLE

void loop()
{
  // put your main code here, to run repeatedly:

  //--------------------------------------------------Pollution Sensors--------------------------------------------------

  Sampling();

  //--------------------------------------------------GPS--------------------------------------------------

  Geolocating();

  //-------------------------------------Power Management unit-----------------------------------------------

  PMU_Canarin_exec();

  //--------------------------------------version of FW -------------------------------------------------------
  char version[14] = __DATE__;

  //---------------------------------------AGGREGATION-----------------------------------
  Aggregate();

  //--------------------------------------------------------------------------------

  logger.print("WRITING FLASH: ");
  logger.println(offset);
  logger.println(sizeof(final));
  logger.println(final);

  if (xSemaphoreTake(spiffsSemaphore, portMAX_DELAY) == pdTRUE)
  {

    // Append the constructed string to the file
    appendFile(SPIFFS, "/Sample.data", final);

    // Print a message indicating the string has been written to the file

    printFile(SPIFFS, "/Sample.data");
    xSemaphoreGive(spiffsSemaphore);
  }
}
