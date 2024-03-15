

#include <Canarin_Utilities.h>
// #include <Adafruit_Sensor.h>

#include <Canarin_BME.h>


char arrayBME[50];


void setup() {
  // put your setup code here, to run once:
  setup_BME = callWithTimeout(BME_Canarin_setup, 30, BME_Canarin_timeout, "ERROR SETUP");
}

void loop() {
  // put your main code here, to run repeatedly:


 if (setup_BME)
    callWithTimeout(BME_Canarin_exec, 30, BME_Canarin_timeout, "ERROR EXEC");
    //callWithTimeout(BME_Canarin_exec, 30, BME_Canarin_timeout, "ERROR EXEC");
  else
  {
    logger.println();
    logger.Sep();
    logger.println("BME reading FAILED!");
    logger.Sep();
  }

  float temp = bme_temperature;
  float hum = bme_humidity;
  float pres = bme_pressure;
  float gas = bme_gas_resistance;

arrayBME[0] = '\0';
snprintf(arrayBME, 40, "4:%.3f,5:%.3f,6:%.3f,40:%.6f", temp, hum, pres, gas);



}
