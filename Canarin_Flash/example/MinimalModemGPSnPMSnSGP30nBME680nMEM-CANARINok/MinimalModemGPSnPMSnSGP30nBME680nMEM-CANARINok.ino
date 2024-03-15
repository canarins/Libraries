/*
    Board -> ESP32S3 Dev Module
    USB CDC On Boot -> Enable ## Note that you need to change Enable to Disable when USB is not connected, so USB CDC will not prevent the board from starting
    CPU Frequency -> 240MHz
    USB DFU On Boot -> Disable
    Flash Mode -> QIO 80MHz
    Flash Size -> 16MB(128Mb)
    USB Firmware MSC On Boot -> Disable
    PSRAM -> OPI PSRAM
    Partition Scheme -> 16M Flash(3MB APP/9.9MB FATFS)
    USB Mode -> Hardware CDC and JIAG
    Upload Mode -> UART0/Hardware CDC
    Upload Speed -> 921600

  WATCH OUT : need to format the external SPI flash before use.
  Was the flash chip formatted with the SdFat_format example?
 */
#include "Adafruit_PM25AQI.h"
#include "Adafruit_SGP30.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include <SoftwareSerial.h>


#include "arduino.h"

SoftwareSerial pmSerial(44, 43);



#include <Canarin_Utilities.h>
//#include "utilities.h"


// for flashTransport definition
//#include "flash_config.h"

#include <Canarin_Flash.h>



//Adafruit_FlashTransport_SPI flashTransport(8, SPI); // Set CS and SPI interface
Adafruit_SPIFlash flash(&flashTransport);
 
// file system object from SdFat
FatFileSystem fatfs;
File32 myFile;


//Adafruit_NeoPixel pixels(1, PIN_RGB, NEO_GRB + NEO_KHZ800);
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
Adafruit_SGP30 sgp;
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME680 bme; // I2C

XPowersPMU  PMUx;

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

#define TINY_GSM_RX_BUFFER 1024
#define TINY_GSM_MODEM_SIM7080
#include <TinyGsmClient.h>


#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(Serial1, Serial);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

void setup()
{
  pinMode(CS_MPU, OUTPUT);
  digitalWrite(CS_MPU, LOW);
  digitalWrite(CS_MPU, HIGH);
  pinMode(CS_MEM, OUTPUT);
  digitalWrite(CS_MEM, LOW);
  digitalWrite(CS_MEM, HIGH);
  pinMode(SET_PMS, OUTPUT);
  digitalWrite(SET_PMS, LOW);
  digitalWrite(SET_PMS, HIGH);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

    Serial.begin(115200);

    //Start while waiting for Serial monitoring
    //while (!Serial) delay(10);
    logger.println();
    //SPI.begin(45, 48, 47, 8);
    SPI.begin(47, 48, 45, 8);

logger.println("Serial Flash test");

  if (flash.begin()) {
      logger.println(F("Device found and supported!"));
  } else {
      logger.println(F("Problem to discover and configure device, check wiring also!"));
  }
  // Set 24Mhz SPI speed
  flashTransport.setClockSpeed(24000000, 24000000); // added to prevent speed problem
 
  //pinMode(16, OUTPUT);
  //flash.setIndicator(16, true);
  logger.print("JEDEC ID: "); logger.println(flash.getJEDECID(), HEX);
  logger.print("Flash size: "); logger.println(flash.size());
  Serial.flush();
 
  // First call begin to mount the filesystem.  Check that it returns true
  // to make sure the filesystem was mounted.
  if (!fatfs.begin(&flash)) {
    logger.println("Error, failed to mount newly formatted filesystem!");
    logger.println("Was the flash chip formatted with the SdFat_format example?");
    while(1) yield();
  }
  logger.println("Mounted filesystem!");
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.

  #define FILE_WRITE_2 (O_RDWR | O_CREAT | O_AT_END)

  myFile = fatfs.open("canarin1.txt", FILE_WRITE_2);
  // #define FILE_WRITE (O_RDWR | O_CREAT | O_AT_END)
 
  // if the file opened okay, write to it:
  if (myFile) {
    logger.print("Writing to canarin1.txt...");
    myFile.println(" #-----------------------------------#");
    // close the file:
    myFile.close();
    logger.println("done.");
  } else {
    // if the file didn't open, print an error:
    logger.println("error opening canarin1.txt");
  }
  
  logger.println();
    logger.println("Adafruit PMSA003I Air Quality Sensor");

  // Wait one second for sensor to boot up!
  delay(1000);
    pmSerial.begin(9600);
  if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    logger.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }
  logger.println("PM25 found!");

    logger.println("SGP30 test");
  Wire.begin(13,21);
  Wire1.begin(15,7);
  //Wire1.begin(I2C_SDA_2, I2C_SCL_2);
  if (! sgp.begin()){
    logger.println("SGP30 Sensor not found :(");
      pixels.setPixelColor(0, pixels.Color(150, 150, 0));
      pixels.show();
    while (1) {
      };
  }
  logger.print("Found SGP30 serial #");
  logger.print(sgp.serialnumber[0], HEX);
  logger.print(sgp.serialnumber[1], HEX);
  logger.println(sgp.serialnumber[2], HEX);

  if (!bme.begin(0x76)) {
    logger.println("Could not find a valid BME680 sensor, check wiring!");
      pixels.setPixelColor(0, pixels.Color(150, 0, 0));
      pixels.show();
   while (1) {
      };
  }
   logger.println("Found bme680");
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_16X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

    /*********************************
     *  step 1 : Initialize power chip,
     *  turn on modem and gps antenna power channel
    ***********************************/
    //if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, I2C_SDA, I2C_SCL)) {
     if (!PMU.begin(Wire1, AXP2101_SLAVE_ADDRESS, 15, 7)) {
        logger.println("Failed to initialize power.....");
        while (1) {
            delay(1000);
        }
    }
    //Set the working voltage of the modem, please do not modify the parameters
    PMU.setDC3Voltage(3000);    //SIM7080 Modem main power channel 2700~ 3400V
    PMU.enableDC3();

    //Modem GPS Power channel
    PMU.setBLDO2Voltage(3300);
    PMU.enableBLDO2();      //The antenna power must be turned on to use the GPS function

    // TS Pin detection must be disable, otherwise it cannot be charged
    PMU.disableTSPinMeasure();

    /*********************************
     * step 2 : start modem
    ***********************************/

    Serial1.begin(115200, SERIAL_8N1, BOARD_MODEM_RXD_PIN, BOARD_MODEM_TXD_PIN);

    pinMode(BOARD_MODEM_PWR_PIN, OUTPUT);

    digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
    delay(100);
    digitalWrite(BOARD_MODEM_PWR_PIN, HIGH);
    delay(1000);
    digitalWrite(BOARD_MODEM_PWR_PIN, LOW);

    int retry = 0;
    while (!modem.testAT(1000)) {
        logger.print(".");
        if (retry++ > 15) {
            // Pull down PWRKEY for more than 1 second according to manual requirements
            digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
            delay(100);
            digitalWrite(BOARD_MODEM_PWR_PIN, HIGH);
            delay(1000);
            digitalWrite(BOARD_MODEM_PWR_PIN, LOW);
            retry = 0;
            logger.println("Retry start modem .");
        }
    }
    logger.println();
    logger.print("Modem started!");

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
        logger.print("Modem enable gps function failed!!");
        while (1) {
            delay(5000);
        }
    }

}

int counter = 0;
void loop()
{
    logger.println(" start loop");

      pixels.setPixelColor(0, pixels.Color(0, 150, 0));
    pixels.show();
    
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

  if (! sgp.IAQmeasure()) {
    logger.println("Measurement failed");
    return;
  }
  logger.print("TVOC "); logger.print(sgp.TVOC); logger.print(" ppb\t");
  logger.print("eCO2 "); logger.print(sgp.eCO2); logger.println(" ppm");

  if (! sgp.IAQmeasureRaw()) {
    logger.println("Raw Measurement failed");
    return;
  }
  logger.print("Raw H2 "); logger.print(sgp.rawH2); logger.print(" \t");
  logger.print("Raw Ethanol "); logger.print(sgp.rawEthanol); logger.println("");
 
  delay(1000);

  counter++;
  if (counter == 30) {
    counter = 0;

    uint16_t TVOC_base, eCO2_base;
    if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      logger.println("Failed to get baseline readings");
      return;
    }
    logger.print("****Baseline values: eCO2: 0x"); logger.print(eCO2_base, HEX);
    logger.print(" & TVOC: 0x"); logger.println(TVOC_base, HEX);
  }

  if (! bme.performReading()) {
    logger.println("Failed to perform reading :(");
    return;
  }
  logger.print("Temperature = ");
  logger.print(bme.temperature);
  logger.println(" *C");

  logger.print("Pressure = ");
  logger.print(bme.pressure / 100.0);
  logger.println(" hPa");

  logger.print("Humidity = ");
  logger.print(bme.humidity);
  logger.println(" %");

  logger.print("Gas = ");
  logger.print(bme.gas_resistance / 1000.0);
  logger.println(" KOhms");

  logger.print("Approx. Altitude = ");
  logger.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  logger.println(" m");

  logger.println();
  
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
  delay(1000);
  
    if (modem.getGPS(&lat2, &lon2, &speed2, &alt2, &vsat2, &usat2, &accuracy2,
                     &year2, &month2, &day2, &hour2, &min2, &sec2)) {
        logger.println();
        logger.print("lat:"); logger.print(String(lat2, 8)); logger.print("\t");
        logger.print("lon:"); logger.print(String(lon2, 8)); logger.println();
        logger.print("speed:"); logger.print(speed2); logger.print("\t");
        logger.print("altitude:"); logger.print(alt2); logger.println();
        logger.print("year:"); logger.print(year2);
        logger.print(" month:"); logger.print(month2);
        logger.print(" day:"); logger.print(day2);
        logger.print(" hour:"); logger.print(hour2);
        logger.print(" minutes:"); logger.print(min2);
        logger.print(" second:"); logger.print(sec2); logger.println();
        logger.println();

        // After successful positioning, the PMU charging indicator flashes quickly
        PMU.setChargingLedMode(XPOWERS_CHG_LED_BLINK_4HZ);
        logger.println(" LED_BLINK_4HZ");
        //while (1) {
        //    delay(1000);
        //}
        pixels.setPixelColor(0, pixels.Color(0, 150, 0));
        pixels.show();
        delay(1000);
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        pixels.show();
        delay(700);
        pixels.setPixelColor(0, pixels.Color(0, 150, 0));
        pixels.show();
        delay(1000);
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        pixels.show();
        delay(700);
// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.
  myFile = fatfs.open("canarin1.txt", FILE_WRITE_2);
  // if the file opened okay, write to it:
  if (myFile) {
    logger.print("Writing GPS data to canarin1.txt...");
    myFile.println();
        myFile.print("lat:"); myFile.print(String(lat2, 8)); myFile.print("\t");
        myFile.print("lon:"); myFile.print(String(lon2, 8)); myFile.println();
        myFile.print("speed:"); myFile.print(speed2); myFile.print("\t");
        myFile.print("altitude:"); myFile.print(alt2); myFile.println();
        myFile.print("year:"); myFile.print(year2);
        myFile.print(" month:"); myFile.print(month2);
        myFile.print(" day:"); myFile.print(day2);
        myFile.print(" hour:"); myFile.print(hour2);
        myFile.print(" minutes:"); myFile.print(min2);
        myFile.print(" second:"); myFile.print(sec2); myFile.println();
        myFile.println();
    // close the file:
    myFile.close();
    logger.println("done Writing GPS data.");
  } else {
    // if the file didn't open, print an error:
    logger.println("error opening canarin1.txt");
  }
    } else {
        // Blinking PMU charging indicator
        PMU.setChargingLedMode(level ? XPOWERS_CHG_LED_ON : XPOWERS_CHG_LED_OFF);
        level ^= 1;
        delay(1000);
        logger.println(" CHG_LED_OFF");
    }
}
