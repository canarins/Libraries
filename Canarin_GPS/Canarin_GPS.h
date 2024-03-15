
#ifndef CANARIN_GPS_H_
#define CANARIN_GPS_H_

#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a serial connection to your GPS module
HardwareSerial gpsSerial(2); // Use the appropriate UART for your board

// Create an EphemerisData object
//EphemerisData ephData;

bool gps_setup = false;

char arrayGPS[36] = "\0";


void Canarin_GPS_setup() {
  if(gps_setup)
  return;
  // Start the serial communication with the GPS module
  gpsSerial.begin(9600); // Adjust the baud rate to match your GPS module
  Serial.begin(115200);  // Initialize serial monitor for debugging
  gps_setup = true;
}
/*
void GPS_exec() {
  // Read GPS data from the GPS module
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // Check if we have a valid GPS fix
  if (gps.location.isValid()) {
    // Access GPS data
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();
    float altitude = gps.altitude.meters();
    float speed = gps.speed.kmph();
    float course = gps.course.deg();
    
    // Print GPS data
    logger.print("Latitude: ");
    logger.println(latitude, 6);
    logger.print("Longitude: ");
    logger.println(longitude, 6);
    logger.print("Altitude: ");
    logger.println(altitude, 2);
    logger.print("Speed: ");
    logger.println(speed, 2);
    logger.print("Course: ");
    logger.println(course, 2);

    // Check if we have valid ephemeris data
    if (ephData.isValid()) {
      // Use ephemeris data for more accurate position calculations
      // For example, you can calculate the precise position using the ephemeris data
      // and the GPS pseudorange measurements
      // Implement your ephemeris data processing here
      double preciseLatitude = calculatePreciseLatitude(ephData, gps);
      double preciseLongitude = calculatePreciseLongitude(ephData, gps);
      logger.print("Precise Latitude: ");
      logger.println(preciseLatitude, 6);
      logger.print("Precise Longitude: ");
      logger.println(preciseLongitude, 6);
    }
  }

  // Handle ephemeris data update
  if (updateEphemerisData()) {
    logger.println("Ephemeris data updated.");
  }
}
*/




float speed = -1;
int age = -1;
int gotGPS = 0;



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

void Geolocating()
{

  if (modem.enableGPS() == false)
  {
    logger.println("Modem enable gps function failed!!");
    pixels.setPixelColor(0, pixels.Color(255, 128, 0)); // display Orange color if GPS failed
    pixels.show();
    while (1)
    {
      logger.println("Modem enable gps function failed!!");
      pixels.setPixelColor(0, pixels.Color(255, 128, 0)); // display Orange color if GPS failed
      pixels.show();
    }
  }

  if (gps_setup)
  {
    get_GPS();
    modem.disableGPS();
    delay(500); // TODO add callWithTimeout
    // callWithTimeout(get_GPS, 30, get_GPS_timeout, "ERROR EXEC");
  }
  else
  {
    logger.println();
    logger.Sep();
    logger.println("GSM reading FAILED!");
    logger.Sep();
  }

  speed = speed2;
  age = -1;
  gotGPS = (lat2 == -1 && lon2 == -1 && alt2 == -1);

  arrayGPS[0] = '\0';
  snprintf(arrayGPS, 36, "1:%.1f,2:%.1f,3:%d", lat2, lon2, alt2);
}

/*
bool updateEphemerisData() {
  // Replace this with your logic to update ephemeris data
  // For example, you can check if new ephemeris data is available from a source
  // and update the ephData object using ephData.update(ephemerisData)
  // Return true if ephemeris data was successfully updated, otherwise false
  return false; // Replace with your logic
}

double calculatePreciseLatitude(const EphemerisData& ephData, const TinyGPSPlus& gps) {
  // Implement your precise latitude calculation here
  // Use ephemeris data and GPS measurements to calculate a more accurate latitude
  // Return the calculated latitude
  // Example: return somePreciseLatitude;
  return gps.location.lat();
}

double calculatePreciseLongitude(const EphemerisData& ephData, const TinyGPSPlus& gps) {
  // Implement your precise longitude calculation here
  // Use ephemeris data and GPS measurements to calculate a more accurate longitude
  // Return the calculated longitude
  // Example: return somePreciseLongitude;
  return gps.location.lng();
}
*/
#endif