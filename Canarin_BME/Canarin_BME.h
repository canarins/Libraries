

#ifndef CANARIN_BME_H_
#define CANARIN_BME_H_


long int bme_temperature, bme_pressure, bme_humidity, bme_gas_resistance, bme_readAltitude;




#include "Adafruit_BME680.h"
#include <Canarin_Utilities.h>


#define SEALEVELPRESSURE_HPA (1013.25)

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature));  // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);                                                                 // [mg/m^3]
  return absoluteHumidityScaled;
}

Adafruit_BME680 bme;
void BME_values_reset() {//TODO is reset -1 or 0
  bme_temperature = 0;

  bme_pressure = 0;

  bme_humidity = 0;

  bme_gas_resistance = 0;

  bme_readAltitude = 0;
}


bool bme_setup = false;

void Canarin_BME_setup() {
  if (bme_setup)
    return;
  Canarin_PMU_setup();

  if (!bme.begin(0x77)) {
    logger.println("Could not find a valid BME680 sensor, check wiring!");
    //while (1);

    while (1) {
      pixels.setPixelColor(0, pixels.Color(255, 0, 255));  //display Purple color Could not find SBME680GP30.
      pixels.show();
      delay(1000);
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.show();
      delay(1000);
      logger.println("Could not find BME680 sensor!");
      bme_setup = false;


      //int threshold = 10;
    }
  }
  logger.println("Found bme680");
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_16X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);  // 320*C for 150 ms
  bme_setup = true;
}


//#define DEBUG_LOGG_BME 

void BME_Canarin_sample() {
  logger.println("Exec begin() - BME");
  u_int BME_counter = 0;
  BME_values_reset();
  while (BME_counter++ < Node_Config.Sample_size) {
    if (!bme.performReading()) {
      //logger.println("->1");
      logger.println("Failed to perform reading :(");
      BME_counter--;
    } else {
      bme_temperature += bme.temperature;
      bme_pressure += bme.pressure;
      bme_humidity += bme.humidity;
      bme_gas_resistance += bme.gas_resistance;
      bme_readAltitude += bme.readAltitude(SEALEVELPRESSURE_HPA);
      #ifdef DEBUG_LOGG_BME
      logger.print(bme.temperature);
      logger.print(" ");//TODO add / Node_Config.Sample_size
      logger.print(bme.pressure); 
      logger.print( " " );
      logger.print(bme.humidity); 
      logger.print( " " );
      logger.print(bme.gas_resistance);
      logger.print( " " );
      logger.println(bme.readAltitude(SEALEVELPRESSURE_HPA));
      #endif

    }
  }
 #ifdef DEBUG_LOGG_BME
  logger.println();
  logger.Sep();
  logger.println("BME reading success!");
  logger.Sep();
  logger.print("Temperature = ");
  delay(150);
  logger.print(bme_temperature / Node_Config.Sample_size);
  delay(150);
  logger.println(F(" °C"));

  logger.print("Pressure = ");
  delay(150);
  logger.print(bme_pressure / (Node_Config.Sample_size * 100.0));
  delay(150);
  logger.println(F(" hPa"));
  delay(50);

  logger.print("Humidity = ");
  delay(150);
  logger.print(bme_humidity / Node_Config.Sample_size);
  delay(150);
  logger.println(F(" %"));
  delay(150);

  logger.print("Gas = ");
  delay(50);
  logger.print(bme_gas_resistance / (Node_Config.Sample_size * 1000.0));
  delay(50);
  logger.println(F(" KOhms"));
  delay(150);

  logger.print("Approx. Altitude = ");//TODO add to the protocole communication
  delay(50);
  logger.print(bme_readAltitude / Node_Config.Sample_size);
  delay(50);
  logger.println(F(" m"));
  logger.Sep();
  delay(150);
   #endif
    
}



void BME_Canarin_timeout(char * signal) {


      bme_temperature = -1;

      bme_pressure = -1;

      bme_humidity = -1;

      bme_gas_resistance = -1;

      bme_readAltitude = -1;
 #ifdef DEBUG_LOGG_BME
  logger.Sep();
  logger.print("Temperature = ERROR ");
  logger.println(signal);
  logger.print("Pressure = ERROR");
  logger.println(signal);
  logger.print("Humidity = ERROR");
  logger.println(signal);
  logger.print("Gas = ERROR");
  logger.println(signal);
  logger.print("Approx. Altitude = ERROR");
  logger.println(signal);
  logger.Sep();
#endif
}

#endif /* CANARIN_BME_680_H_ */