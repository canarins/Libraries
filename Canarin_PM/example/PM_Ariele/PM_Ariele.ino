


// SENSOR
#include <Canarin_Utilities.h>
#include <Canarin_PM.h>


char arrayPM[32];
// TIME

void setup() {

  //TODO ADD THREADS MANAGEMENT
  setup_PM = callWithTimeout(PM_Canarin_setup, 70, PM_Canarin_timeout, "ERROR SETUP");

}




void loop(){

 // PM25_AQI_Data data;
  if (setup_PM)
    callWithTimeout(PM_Canarin_exec, 30, PM_Canarin_timeout, "ERROR EXEC");
  else
  {
    logger.println();
    logger.Sep();
    logger.println("PM reading FAILED!");
    logger.Sep();
  }


long PM25 = pm25_standard;
long PM10 = pm100_standard;
long PM01 = pm10_standard;

arrayPM[0] = '\0';

snprintf(arrayPM, 32, "7:%lu,8:%lu,9:%lu", PM25 , PM10, PM01);//TODO CHECK order

}
