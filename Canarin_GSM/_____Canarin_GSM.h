
#ifndef CANARIN_GSM_H_
#define CANARIN_GSM_H_


// See all AT commands, if wanted



#define DUMP_AT_COMMANDS

#define TINY_GSM_RX_BUFFER 1024

#define TINY_GSM_MODEM_SIM7080
#include <TinyGsmClient.h>



#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

void GSM_Canarin_setup() {


  SerialAT.begin(115200, SERIAL_8N1, BOARD_MODEM_RXD_PIN, BOARD_MODEM_TXD_PIN);

  pinMode(BOARD_MODEM_PWR_PIN, OUTPUT);

  digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
  delay(100);
  digitalWrite(BOARD_MODEM_PWR_PIN, HIGH);
  delay(1000);
  digitalWrite(BOARD_MODEM_PWR_PIN, LOW);

  int retry = 0;
  while (!modem.testAT(1000)) {
    if (retry++ > 15) {
      // Pull down PWRKEY for more than 1 second according to manual requirements
      digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
      delay(100);
      digitalWrite(BOARD_MODEM_PWR_PIN, HIGH);
      delay(1000);
      digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
      retry = 0;
      logger.println("Retry start modem .");
      pixels.setPixelColor(0, pixels.Color(255, 128, 0));  //display Blue color if try to restart modem
      pixels.show();
    }
  }
  logger.println();
  logger.println("Modem started!");

  /*********************************
     * step 3 : start modem gps function
    ***********************************/

  //  When configuring GNSS, you need to stop GPS first
  modem.disableGPS();
  delay(500);
#if 0
    /*
    ! GNSS Work Mode Set
     <gps mode> GPS work mode.
        1 Start GPS NMEA out.
    <glo mode> GLONASS work mode.
        0 Stop GLONASS NMEA out.
        1 Start GLONASS NMEA out.
    <bd mode> BEIDOU work mode.
        0 Stop BEIDOU NMEA out.
        1 Start BEIDOU NMEA out.
    <gal mode> GALILEAN work mode.
        0 Stop GALILEAN NMEA out.
        1 Start GALILEAN NMEA out.
    <qzss mode> QZSS work mode.
        0 Stop QZSS NMEA out.
        1 Start QZSS NMEA out.*/

    //GNSS Work Mode Set GPS+BEIDOU
    modem.sendAT("+CGNSMOD=1,1,0,0,0");
    modem.waitResponse();


    /*
    GNSS Command,For more parameters, see <SIM7070_SIM7080_SIM7090 Series_AT Command Manual> 212 page.
    <minInterval> range: 1000-60000 ms
     minInterval is the minimum time interval in milliseconds that must elapse between position reports. default value is 1000.
    <minDistance> range: 0-1000
     Minimum distance in meters that must be traversed between position reports. Setting this interval to 0 will be a pure time-based tracking/batching.
    <accuracy>:
        0  Accuracy is not specified, use default.
        1  Low Accuracy for location is acceptable.
        2 Medium Accuracy for location is acceptable.
        3 Only High Accuracy for location is acceptable.
    */
    // minInterval = 1000,minDistance = 0,accuracy = 0
    modem.sendAT("+SGNSCMD=2,1000,0,0");
    modem.waitResponse();

    // Turn off GNSS.
    modem.sendAT("+SGNSCMD=0");
    modem.waitResponse();
#endif
  delay(500);

  // GPS function needs to be enabled for the first use
  if (modem.enableGPS() == false) {
    logger.println("Modem enable gps function failed!!");
    pixels.setPixelColor(0, pixels.Color(255, 128, 0));  //display Orange color if GPS failed
    pixels.show();
    while (1) {
      logger.println("Modem enable gps function failed!!");
      pixels.setPixelColor(0, pixels.Color(255, 128, 0));  //display Orange color if GPS failed
      pixels.show();
    }
  }
}


void get_GPS() {
  if (modem.getGPS(&lat2, &lon2, &speed2, &alt2, &vsat2, &usat2, &accuracy2,
                   &year2, &month2, &day2, &hour2, &min2, &sec2)) {

    logger.println("##############  FOUND GPS POSITION  ##############");
    logger.print("lat:");
    logger.println(String(lat2, 8));
    logger.print("lon:");
    logger.println(String(lon2, 8));
    logger.print("speed:");
    logger.println(speed2);
    logger.print("altitude:");
    logger.println(alt2);
    delay(150);
    logger.print(" Date ="+(String)hour2+":"+(String)min2+" "+(String)sec2+" - "+(String)day2+"/"+(String)month2+"/"+(String)year2);
    logger.println("##########################################");
    logger.println();

    // After successful positioning, the PMU charging indicator flashes quickly
    //  PMU.setChargingLedMode(XPOWERS_CHG_LED_BLINK_4HZ);
    //  logger.println(" LED_BLINK_4HZ");
    pixels.setPixelColor(0, pixels.Color(0, 0, 150));
    pixels.show();
    delay(200);
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
    delay(200);
    pixels.setPixelColor(0, pixels.Color(150, 0, 0));
    pixels.show();
    delay(200);
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
    //while (1) {
    //    delay(1000);
    //}
  } else {

    delay(500);
    //logger.print(" retrying potionning"); logger.println();
    logger.print(" retrying potionning");
    logger.println();
    delay(500);
  }
}


void get_GPS_timeout(char* signal) {
  lat2 = -1;
  lon2 = -1;
  speed2 = -1;
  alt2 = -1;
  year2 = -1;
  month2 = -1;
  day2 = -1;
  hour2 = -1;
  min2 = -1;
  sec2 = -1;

  logger.print(" lat2"+(String)lat2);
  logger.print(" lon2"+(String)lon2);
  logger.print(" speed2"+(String)speed2);
  logger.print(" alt2"+(String)alt2);
  logger.print(" Date ="+(String)hour2+":"+(String)min2+" "+(String)sec2+" - "+(String)day2+"/"+(String)month2+"/"+(String)year2);


}

#endif