
#ifndef CANARIN_PM_H_
#define CANARIN_PM_H_


#include "PMS.h"
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
#include <Canarin_Utilities.h>


long int pm10_standard, pm25_standard, pm100_standard, pm10_env, pm25_env, pm100_env, particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;


//-------------------------------------------------------------------------------
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
PM25_AQI_Data data;


//PMS pms(pmSerial2);
//PMS::DATA data2;
SoftwareSerial pmSerial(44, 43);

void PM_value_reset() {
  pm10_standard = 0;
  pm25_standard = 0;
  pm100_standard = 0;
  pm10_env = 0;
  pm25_env = 0;
  pm100_env = 0;
  particles_03um = 0;
  particles_05um = 0;
  particles_10um = 0;
  particles_25um = 0;
  particles_50um = 0;
  particles_100um = 0;
}

bool pm_setup = false;


void Canarin_PM_setup() {
  if (pm_setup)
  return;
Canarin_PMU_setup();
  pmSerial.begin(9600);
  //pmSerial2.begin(9600);
  //logger.println("PMS7003 test");
  logger.println("PMS7003 test");


  if (!aqi.begin_UART(&pmSerial)) {  // connect to the sensor over software serial
    //logger.println("Could not find PMS7003 sensor!");
    logger.println("Could not find PMS7003 sensor!");
    pixels.setPixelColor(0, pixels.Color(255, 0, 255));  //display Purple color Could not find PMS7003.
    pixels.show();
    //while (1) delay(10);
    while (1) {
      pixels.setPixelColor(0, pixels.Color(255, 0, 255));  //display Purple color Could not find PMS7003.
      pixels.show();
      delay(250);
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.show();
      delay(250);
      logger.println("Could not find PMS7003 sensor!");
    }
  }


  logger.println("PMS7003 found!");
  logger.println("Waking up PMS: ");
  //pms.wakeUp();
  //delay(30000);
  //pms.activeMode();
  //logger.println("PMS7003 found!");
  pm_setup = true;
}
//#define DEBUG_LOGG_PM 

void PM_Canarin_sample() {
  PM_value_reset();

  int PM_counter = 0;
  while (PM_counter++ < Node_Config.Sample_size) {

    //TODO Timeout if Sampling is too long

    if (!aqi.read(&data)) {
      logger.println("Could not read from AQI");
      delay(250);  // try again in a bit!
      PM_counter--;
    } else {

      pm10_standard += data.pm10_standard;
      pm25_standard += data.pm25_standard;
      pm100_standard += data.pm100_standard;

      pm10_env += data.pm10_env;
      pm25_env += data.pm25_env;
      pm100_env += data.pm100_env;

      particles_03um += data.particles_03um;
      particles_05um += data.particles_05um;
      particles_10um += data.particles_10um;
      particles_25um += data.particles_25um;
      particles_50um += data.particles_50um;
      particles_100um += data.particles_100um;
#ifdef DEBUG_LOGG_PM
      logger.print(data.pm10_standard);
      logger.print(" ");
      logger.print(data.pm25_standard)  ;
logger.print(" ");
      logger.print(data.pm100_standard) ; 
      logger.print(" ");
      logger.print(data.pm10_env)  ;
      logger.print(" ");
      logger.print(data.pm25_env) ; 
      logger.print(" ");
      logger.print(data.pm100_env)  ; 
      logger.print(" ");
      logger.print(data.particles_03um)  ; 
      logger.print(" ");
      logger.print(data.particles_05um)  ; 
      logger.print(" ");
      logger.print(data.particles_10um)  ; 
      logger.print(" ");
      logger.print(data.particles_25um)  ; 
      logger.print(" ");
      logger.print(data.particles_50um)  ; 
      logger.print(" ");
      logger.println(data.particles_100um);
      #endif
    }
    delay(1000);
  }

  //if (!aqi.read(&data)) {
  //  logger.println("Could not read from AQI");
  //  delay(250);  // try again in a bit!
  //  return;
  //}
  logger.println();//TODO units
  logger.Sep();
  logger.println("PM reading success!");
  logger.Sep();
  logger.println(F("Concentration Units (standard)"));
  logger.Sep();
  logger.print(F("\t\tPM 1.0: "));
  logger.print(pm10_standard / Node_Config.Sample_size);
  delay(50);//TODO why the delay
  logger.print(F("\t\tPM 2.5: "));
  logger.print(pm25_standard / Node_Config.Sample_size);
  delay(50);
  logger.print(F("\t\tPM 10: "));
  logger.println(pm100_standard / Node_Config.Sample_size);
  delay(50);
  logger.println(F("Concentration Units (environmental)"));
  logger.Sep();
  logger.print(F("\t\tPM 1.0: "));
  logger.print(pm10_env / Node_Config.Sample_size);
  delay(50);
  logger.print(F("\t\tPM 2.5: "));
  logger.print(pm25_env / Node_Config.Sample_size);
  delay(50);
  logger.print(F("\t\tPM 10: "));
  logger.println(pm100_env / Node_Config.Sample_size);
  delay(50);
  logger.Sep();
  logger.print(F("Particles > 0.3um / 0.1L air:"));
  logger.println(particles_03um / Node_Config.Sample_size);
  delay(50);
  logger.print(F("Particles > 0.5um / 0.1L air:"));
  logger.println(particles_05um / Node_Config.Sample_size);
  delay(50);
  logger.print(F("Particles > 1.0um / 0.1L air:"));
  logger.println(particles_10um / Node_Config.Sample_size);
  delay(50);
  logger.print(F("Particles > 2.5um / 0.1L air:"));
  logger.println(particles_25um / Node_Config.Sample_size);
  delay(50);
  logger.print(F("Particles > 5.0um / 0.1L air:"));
  logger.println(particles_50um / Node_Config.Sample_size);
  delay(50);
  logger.print(F("Particles > 10 um / 0.1L air:"));
  logger.println(particles_100um / Node_Config.Sample_size);
  delay(50);
  logger.Sep();}

void PM_Canarin_timeout(char * signal) {


      pm10_standard = -1;
      pm25_standard = -1;
      pm100_standard = -1;

      pm10_env = -1;
      pm25_env = -1;
      pm100_env = -1;

      particles_03um = -1;
      particles_05um = -1;
      particles_10um = -1;
      particles_25um = -1;
      particles_50um = -1;
      particles_100um = -1;



  //if (!aqi.read(&data)) {
  //  logger.println("Could not read from AQI");
  //  delay(250);  // try again in a bit!
  //  return;
  //}
  logger.Sep();
  logger.println("AQI reading success!");
  logger.Sep();
  logger.println();
  logger.Sep();  
  logger.println(F("Concentration Units (standard)"));
  logger.Sep();  
  logger.print(F("\t\tPM 1.0: " ));
  logger.println((String)signal);
  logger.println(F("Concentration Units (environmental)"));
  logger.Sep();  
  logger.print(F("\t\tPM 1.0: " ));
  logger.println((String)signal);
  logger.print(F("\t\tPM 2.5: " ));
  logger.println((String)signal);
  logger.print(F("\t\tPM 10: " ));
  logger.println((String)signal);
  logger.Sep();  
  logger.print(F("Particles > 0.3um / 0.1L air:" ));
  logger.println((String)signal);
  logger.print(F("Particles > 0.5um / 0.1L air:" ));
  logger.println((String)signal);
  logger.print(F("Particles > 1.0um / 0.1L air:" ));
  logger.println((String)signal);
  logger.print(F("Particles > 2.5um / 0.1L air:" ));
  logger.println((String)signal);
  logger.print(F("Particles > 5.0um / 0.1L air:" ));
  logger.println((String)signal);
  logger.print(F("Particles > 10 um / 0.1L air:" ));
  logger.println((String)signal);

  logger.Sep();
}

//-------------------------------------------------------------------------------

#endif