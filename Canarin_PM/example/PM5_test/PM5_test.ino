/* Test sketch for Adafruit PM2.5 sensor with UART or I2C */

#include "Adafruit_PM25AQI.h"

// If your PM2.5 is UART only, for UNO and others (without hardware serial) 
// we must use software serial...
// pin #2 is IN from sensor (TX pin on sensor), leave pin #3 disconnected
// comment these two lines if using hardware serial
#include <SoftwareSerial.h>
SoftwareSerial pmSerial(2, 3);

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

void setup() {
  // Wait for serial monitor to open
  Serial.begin(115200);
  //while (!Serial) delay(10);

  logger.println("Adafruit PMSA003I Air Quality Sensor");

  // Wait one second for sensor to boot up!
  delay(1000);

  // If using serial, initialize it and set baudrate before starting!
  // Uncomment one of the following
  //Serial1.begin(9600);
  pmSerial.begin(9600);

  // There are 3 options for connectivity!
  //if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
  //if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
  if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    logger.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  logger.println("PM25 found!");
}

void loop() {
  PM25_AQI_Data data;
  
  if (! aqi.read(&data)) {
    logger.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    return;
  }
  logger.println("AQI reading success");

  logger.println();
  logger.println(F("---------------------------------------"));
  logger.println(F("Concentration Units (standard)"));
  logger.println(F("---------------------------------------"));
  logger.print(F("PM 1.0: ")); logger.print(data.pm10_standard);
  logger.print(F("\t\tPM 2.5: ")); logger.print(data.pm25_standard);
  logger.print(F("\t\tPM 10: ")); logger.println(data.pm100_standard);
  logger.println(F("Concentration Units (environmental)"));
  logger.println(F("---------------------------------------"));
  logger.print(F("PM 1.0: ")); logger.print(data.pm10_env);
  logger.print(F("\t\tPM 2.5: ")); logger.print(data.pm25_env);
  logger.print(F("\t\tPM 10: ")); logger.println(data.pm100_env);
  logger.println(F("---------------------------------------"));
  logger.print(F("Particles > 0.3um / 0.1L air:")); logger.println(data.particles_03um);
  logger.print(F("Particles > 0.5um / 0.1L air:")); logger.println(data.particles_05um);
  logger.print(F("Particles > 1.0um / 0.1L air:")); logger.println(data.particles_10um);
  logger.print(F("Particles > 2.5um / 0.1L air:")); logger.println(data.particles_25um);
  logger.print(F("Particles > 5.0um / 0.1L air:")); logger.println(data.particles_50um);
  logger.print(F("Particles > 10 um / 0.1L air:")); logger.println(data.particles_100um);
  logger.println(F("---------------------------------------"));
  

  delay(1000);
}