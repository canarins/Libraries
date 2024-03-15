
#include <Canarin_Utilities.h>
#include <Canarin_SGP.h>

char arraySGP[32];

void setup() {
  // put your setup code here, to run once:

  setup_SGP = callWithTimeout(SGP_Canarin_setup, 30, SGP_Canarin_timeout, "ERROR SETUP");

}

void loop() {
  // put your main code here, to run repeatedly:


 if (setup_SGP)
    callWithTimeout(SGP_Canarin_exec, 30, SGP_Canarin_timeout, "ERROR EXEC");
  else
  {
    logger.println();
    logger.Sep();
    logger.println("SGP reading FAILED!");
    logger.Sep();
  }

  long TVOC = sgp_TVOC;
  long eCO2 = sgp_eCO2;

  arraySGP[0] = '\0';
 snprintf(arraySGP, 50, "41:%lu,42:%lu,43:%lu,44:%lu", sgp_TVOC, sgp_eCO2,sgp_rawEthanol,sgp_rawH2); // ppb and ppm



}
