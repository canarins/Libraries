
#ifndef CANARIN_SAMPLING_H_
#define CANARIN_SAMPLING_H_



// SENSOR
#include <Canarin_PM.h>

#include <Canarin_SGP.h>


// #include <Adafruit_Sensor.h>
#include <Canarin_BME.h>

// TIME
#include <Canarin_GPS.h>

bool sampling_setup = false;

void Canarin_Sampling_setup()
{
if(sampling_setup)
return;
  // TODO ADD THREADS MANAGEMENT
  pm_setup = callWithTimeout(Canarin_PM_setup, 70, PM_Canarin_timeout, "ERROR SETUP");

  sgp_setup = callWithTimeout(Canarin_SGP_setup, 30, Canarin_SGP_timeout, "ERROR SETUP");

  bme_setup = callWithTimeout(Canarin_BME_setup, 30, BME_Canarin_timeout, "ERROR SETUP");

sampling_setup = true;

}
int value = 0; // TODO to debug

char arrayPM[32];
char arrayBME[50];
char arraySGP[32];
char arrayBattery[16];


void Sampling()
{

  // PM25_AQI_Data data;
  if (pm_setup)
  {

    callWithTimeout(PM_Canarin_sample, 30, PM_Canarin_timeout, "ERROR EXEC");
    arrayPM[0] = '\0'; // Clearing the buffer at the beginning.

    size_t offset = 0; // Keeps track of the current position in the buffer.
    // We assume pm25_standard, pm100_standard, and pm10_standard have been declared elsewhere.

    //   if ((value % 8 & (1 << 0)) != 0)
    //   pm25_standard = -1;

    if (pm25_standard != -1)
    {
      offset += snprintf(arrayPM + offset, sizeof(arrayPM) - offset, (offset == 0) ? "7:%lu" : ",7:%lu", pm25_standard/Node_Config.Sample_size);
    }
    //    if ((value % 8 & (1 << 1)) != 0)
    //      pm100_standard = -1;

    if (pm100_standard != -1)
    {
      // Append only if pm100_standard has a valid non-negative value.
      offset += snprintf(arrayPM + offset, sizeof(arrayPM) - offset, (offset == 0) ? "8:%lu" : ",8:%lu", pm100_standard/Node_Config.Sample_size);
    }

    //    if ((value % 8 & (1 << 2)) != 0)
    //      pm10_standard = -1;

    if (pm10_standard != -1)
    {
      // Append only if pm10_standard has a valid non-negative value.
      offset += snprintf(arrayPM + offset, sizeof(arrayPM) - offset, (offset == 0) ? "9:%lu" : ",9:%lu", pm10_standard/Node_Config.Sample_size);
    }

    // Since checking the size of the array using 'sizeof' will always yield the total size (32),
    // regardless of the content, we need to print the length of the C-string instead using strlen().
    logger.print(value);
    logger.print(": (");
    logger.print(strlen(arrayPM)); // Print the length of the content, not the size of the buffer.
    logger.print(" - ");
    logger.print(offset);
    logger.print(") = ");
    logger.println(arrayPM);
  }
  else
  {
    logger.println();
    logger.Sep();
    logger.println("PM reading FAILED!");
    logger.Sep();
  }

  // long PM25 = pm25_standard;
  // long PM10 = pm100_standard;
  // long PM01 = pm10_standard;

  // snprintf(arrayPM, 32, "7:%lu,8:%lu,9:%lu", pm25_standard, pm100_standard, pm10_standard); // TODO CHECK order
  // sgp_setup = false;



  if (sgp_setup)
  {

    arraySGP[0] = '\0';
    callWithTimeout(Canarin_SGP_sample, 30, Canarin_SGP_timeout, "ERROR EXEC");

    size_t offset = 0; // Keeps track of the current position in the buffer.
    // We assume pm25_standard, pm100_standard, and pm10_standard have been declared elsewhere.

    if (sgp_TVOC != -1)
    {
      // Append only if pm25_standard has a valid non-negative value.
      offset += snprintf(arraySGP + offset, sizeof(arraySGP) - offset, (offset == 0) ? "41:%lu" : ",41:%lu", sgp_TVOC/Node_Config.Sample_size);
    }

    if (sgp_eCO2 != -1)
    {
      // Append only if pm100_standard has a valid non-negative value.
      offset += snprintf(arraySGP + offset, sizeof(arraySGP) - offset, (offset == 0) ? "42:%lu" : ",42:%lu", sgp_eCO2/Node_Config.Sample_size);
    }

    if (sgp_rawH2 != -1)
    {
      // Append only if pm25_standard has a valid non-negative value.
      offset += snprintf(arraySGP + offset, sizeof(arraySGP) - offset, (offset == 0) ? "43:%lu" : ",43:%lu", sgp_rawH2/Node_Config.Sample_size);
    }

    if (sgp_rawEthanol != -1)
    {
      // Append only if pm100_standard has a valid non-negative value.
      offset += snprintf(arraySGP + offset, sizeof(arraySGP) - offset, (offset == 0) ? "44:%lu" : ",44:%lu", sgp_rawEthanol/Node_Config.Sample_size);
    }


    logger.print(value);
    logger.print(": (");
    logger.print(strlen(arraySGP)); // Print the length of the content, not the size of the buffer.
    logger.print(" - ");
    logger.print(offset);
    logger.print(") = ");
    logger.println(arraySGP);

    // snprintf(arraySGP, 50, "41:%lu,42:%lu", sgp_TVOC, sgp_eCO2); // ppb and ppm
  }
  else
  {
    logger.println();
    logger.Sep();
    logger.println("SGP reading FAILED!");
    logger.Sep();
  }

  // long TVOC = sgp_TVOC;
  // long eCO2 = sgp_eCO2;

  // bme_setup = false;


  if (bme_setup)
  {
    // BME_Canarin_sample(); // TODO add callWithTimeout
    arrayBME[0] = '\0';
    callWithTimeout(BME_Canarin_sample, 30, BME_Canarin_timeout, "ERROR EXEC");

    size_t offset = 0; // Keeps track of the current position in the buffer.
    if (bme_temperature != -1)
    {
      // Append only if pm25_standard has a valid non-negative value.
      offset += snprintf(arrayBME + offset, sizeof(arrayBME) - offset, (offset == 0) ? "4:%.3f" : ",4:%.3f", bme_temperature/Node_Config.Sample_size);
    }

    if (bme_humidity != -1)
    {
      // Append only if pm100_standard has a valid non-negative value.
      offset += snprintf(arrayBME + offset, sizeof(arrayBME) - offset, (offset == 0) ? "5:%.3f" : ",5:%.3f", bme_humidity/Node_Config.Sample_size);
    }

    if (bme_pressure != -1)
    {
      // Append only if pm25_standard has a valid non-negative value.
      offset += snprintf(arrayBME + offset, sizeof(arrayBME) - offset, (offset == 0) ? "6:%.3f" : ",6:%.3f", bme_pressure/(Node_Config.Sample_size * 100));
    }

    if (bme_gas_resistance != -1)
    {
      // Append only if pm100_standard has a valid non-negative value.
      offset += snprintf(arrayBME + offset, sizeof(arrayBME) - offset, (offset == 0) ? "40:%.6f" : ",40:%.6f", bme_gas_resistance/(Node_Config.Sample_size * 1000.0));
    }

    logger.print(value);
    logger.print(": (");
    logger.print(strlen(arrayBME)); // Print the length of the content, not the size of the buffer.
    logger.print(" - ");
    logger.print(offset);
    logger.print(") = ");
    logger.println(arrayBME);
    // snprintf(arrayBME, 40, "4:%.3f,5:%.3f,6:%.3f,40:%.6f", bme_temperature, bme_humidity, bme_pressure, bme_gas_resistance);
  }
  else
  {
    logger.println();
    logger.Sep();
    logger.println("BME reading FAILED!");
    logger.Sep();
  }

if(pmu_setup){


PMU_Canarin_status();

logger.print("getBattVoltage:");
  logger.println(PMU.getBattVoltage());
  logger.print("status:");
  logger.println(PMU.status());
  snprintf(arrayBattery, 16, "10:%u,50:%u", PMU.getBattVoltage(), PMU.status());

}

  value++;
}

void Aggregating(char * message){

  char buffer[1024] = "\0";

  logger.println("------------->2");
  // Geolocating();

  int nowToSend = millis(); // TODO Synchronize

  logger.println("------------->3");
  // Geolocating();

  size_t offset = 0;
#ifdef DEBUG_LOGG_SAMPLE
  logger.print("------------->DEFAULT_COMMAND ");
  logger.println(DEFAULT_COMMAND);
  logger.println(sizeof(DEFAULT_COMMAND));

  // Geolocating();
#endif
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",%d" : "%d", DEFAULT_COMMAND);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->NODE_ID ");
  logger.println(NODE_ID);
  logger.println(sizeof(NODE_ID));
#endif
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",%s" : ",%s", NODE_ID);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);

  logger.print("------------->PROJECT_ID ");
  logger.println(PROJECT_ID);
  logger.println(sizeof(PROJECT_ID));
#endif
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",%d" : "%d", PROJECT_ID);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->nowToSend ");
  logger.println(nowToSend);
  logger.println(sizeof(nowToSend));
  logger.println(offset);
#endif
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",%d" : "%d", nowToSend);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->arrayGPS ");
  logger.println(arrayGPS);
  logger.println(*arrayGPS);
  logger.println(sizeof(arrayGPS));
#endif
  if (arrayGPS[0] != '\0')
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",%s" : "%s", arrayGPS);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->arrayPM ");
  logger.println(arrayPM);
  logger.println(sizeof(arrayPM));
#endif
  if (arrayPM[0] != '\0')
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",%s" : "%s", arrayPM);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->arrayBME ");
  logger.println(arrayBME);
  logger.println(sizeof(arrayBME));
#endif
  if (arrayBME[0] != '\0')
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",%s" : "%s", arrayBME);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->arraySGP ");
  logger.println(arraySGP);
  logger.println(sizeof(arraySGP));
#endif
  if (arraySGP[0] != '\0')
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",%s" : "%s", arraySGP);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->arrayBattery ");
  logger.println(arrayBattery);
  logger.println(sizeof(arrayBattery));
#endif
  if (arrayBattery[0] != '\0')
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",%s" : "%s", arrayBattery);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->version ");
  logger.println(version);
  logger.println(sizeof(version));
#endif
  if (version[0] != '\0')
    offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",45:%s" : "45:%s", version);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->speed2 ");
  logger.println(speed2);
  logger.println(sizeof(speed2));
#endif
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",46:%.2f" : "46:%.2f", speed2);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->age ");
  logger.println(age);
  logger.println(sizeof(age));
#endif
  offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",47:%d" : "47:%d", age);
#ifdef DEBUG_LOGG_SAMPLE

  logger.println(buffer);
  logger.println(offset);
  logger.print("------------->gotGPS ");
  logger.println(gotGPS);
  logger.println(sizeof(gotGPS));
#endif

  offset += snprintf(buffer + offset, sizeof(buffer) - offset, (offset) ? ",48:%d" : "48:%d", gotGPS);
#ifdef DEBUG_LOGG_SAMPLE
  logger.println(buffer);
  logger.println(offset);
  logger.println(sizeof(buffer));

logger.println(offset + 5);
  
logger.println(sizeof(message));
#endif

  offset = snprintf(message, offset + 5, "{%s}\n\0", buffer);
  

  logger.println(message);



}

#endif