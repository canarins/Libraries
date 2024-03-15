#ifndef CANARIN_SGP_H_
#define CANARIN_SGP_H_

#include "Adafruit_SGP30.h"
#include <Canarin_Utilities.h>


long int sgp_TVOC, sgp_eCO2, sgp_rawH2, sgp_rawEthanol;
bool sgp_setup = false;

Adafruit_SGP30 sgp;


int counterBaseline = 0;

void SGP_values_reset() {
  sgp_TVOC = 0;
  sgp_eCO2 = 0;
  sgp_rawH2 = 0;
  sgp_rawEthanol = 0;
}

void Canarin_SGP_setup() {
  if (sgp_setup)
  return;
Canarin_PMU_setup();
  if (!sgp.begin()) {
    //logger.println("SGP30 Sensor not found :(");
    //while (1);
    while (1) {
      pixels.setPixelColor(0, pixels.Color(255, 0, 255));  //display Purple color Could not find SGP30.
      pixels.show();
      delay(500);
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.show();
      delay(500);

      logger.println("Could not find SGP30 sensor!");
      sgp_setup = false;
    }
  }
  logger.print("Found SGP30 serial #");
  logger.print(sgp.serialnumber[0], HEX);
  logger.print(sgp.serialnumber[1], HEX);
  logger.println(sgp.serialnumber[2], HEX);
  sgp_setup = true;
  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  //sgp.setIAQBaseline(0x8E68, 0x8F41);  // Will vary for each sensor!
}




//#define DEBUG_LOGG_SGP 

void Canarin_SGP_sample() {
  // If you have a temperature / humidity sensor, you can set the absolute humidity to enable the humditiy compensation for the air quality signals
  //float temperature = 22.1; // [°C]
  //float humidity = 45.2; // [%RH]
  //sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

  SGP_values_reset();
  int SGP_counter = 0;


  while (SGP_counter++ < Node_Config.Sample_size) {
    if (!sgp.IAQmeasure()) {
      logger.println("Measurement failed");
      SGP_counter--;
    } else {
      sgp_TVOC += sgp.TVOC;
      sgp_eCO2 += sgp.eCO2;


#ifdef DEBUG_LOGG_SGP
      logger.print(sgp.TVOC);
      logger.print(" ");
      logger.println(sgp.eCO2);

          #endif
    }
  }

  logger.println();
  logger.Sep();
  logger.println("SGP reading success!");
  logger.Sep();
  logger.print("TVOC ");
  logger.print(sgp_TVOC / Node_Config.Sample_size);
  logger.println(" ppb");
  logger.print("eCO2 ");
  logger.print(sgp_eCO2 / Node_Config.Sample_size);
  logger.println(" ppm");
  logger.Sep();
  SGP_counter = 0;

  while (SGP_counter++ < Node_Config.Sample_size) {
    if (!sgp.IAQmeasureRaw()) {
      logger.println("Measurement failed");
      SGP_counter--;
    } else {

      sgp_rawH2 += sgp.rawH2;
      sgp_rawEthanol += sgp.rawEthanol;

      #ifdef DEBUG_LOGG_SGP
            logger.print(sgp.rawH2);
      logger.print(" ");
      logger.println(sgp.rawEthanol);
       #endif
    }
  }
  logger.Sep();//TODO find unit
  logger.print("Raw H2 ");
  logger.print(sgp_rawH2 / Node_Config.Sample_size);
  logger.println("");
  logger.print("Raw Ethanol ");
  logger.print(sgp_rawEthanol / Node_Config.Sample_size);
  logger.println("");
  logger.Sep();
  delay(50);


  delay(1000);

  counterBaseline++;
  if (counterBaseline == 30) {
    counterBaseline = 0;
    logger.println(""+(String)counterBaseline);
    uint16_t TVOC_base, eCO2_base;
    if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      logger.println("Failed to get baseline readings");
      return;
    }
    logger.print("****Baseline values: eCO2: 0x");
    logger.print(eCO2_base, HEX);
    logger.print(" & TVOC: 0x");
    logger.println(TVOC_base, HEX);
  }
sgp_TVOC = sgp_TVOC / Node_Config.Sample_size;
sgp_eCO2 = sgp_eCO2 / Node_Config.Sample_size;
sgp_rawH2 = sgp_rawH2 / Node_Config.Sample_size;
sgp_rawEthanol = sgp_rawEthanol / Node_Config.Sample_size;


      logger.println("End - exec");
}


void Canarin_SGP_timeout(char * signal) {

  sgp_TVOC = -1;
  sgp_eCO2 = -1;
  logger.Sep();
  logger.println("TVOC " + (String)signal);

  logger.println("eCO2 " + (String)signal);
  logger.Sep();



  sgp_rawH2 = -1;
  sgp_rawEthanol = -1;

  logger.Sep();
  logger.println("Raw H2 " + (String)signal);

  logger.println("Raw Ethanol " + (String)signal);

  logger.Sep();
  delay(50);
}

#endif