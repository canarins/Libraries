

#include "parameter.h"


#include <Canarin_Configuration.h>

byte mac[6];

#include <Canarin_PMU.h>



void setup()
{
  Serial.begin(115200);

  logger.println("parameter");
  setup_parameters();
  logger.println("Connectic");
  Canarin_Connectic_setup();

  Canarin_SPIFFS_setup();

//get IMEI for NODE_ID
  logger.println("IMEI");
  // WiFi.macAddress(mac);
  String imei = modem.getIMEI();
  logger.println(imei);
  snprintf(NODE_ID, 16, "%s", imei.c_str());
  logger.println(NODE_ID);


//


  // SPIFFS.format();

  logger.println("--------------------------------------------------------");

  printDeviceConfig();

  logger.println("--------------------------------------------------------");




  strncpy(PROJECT_ID, "12", sizeof(PROJECT_ID) - 1);
  PROJECT_ID[sizeof(PROJECT_ID) - 1] = '\0'; // Ensure null termination

if (!PMU.isCharging()){

logger.println("VERSION : ");
logger.println(versionString );

  //if (Config.Version_to_install != "" || Config.Version_to_install != VERSION ){

  //} 

}

 get_configuration();



}

void loop() {}