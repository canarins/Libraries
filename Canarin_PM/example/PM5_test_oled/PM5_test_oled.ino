#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "Adafruit_PM25AQI.h"

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();
Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);


void setup() {
  // Wait for serial monitor to open
  Serial.begin(115200);
  //while (!Serial) delay(10);

  logger.println("Adafruit PMSA003I Air Quality Sensor");

  logger.println("128x64 OLED FeatherWing test");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(0x3C, true); // Address 0x3C default

  logger.println("OLED begun");
  
  display.display();
  delay(1000);
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  // There are 3 options for connectivity!
  if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
    logger.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  logger.println("PM25 found!");

  display.setTextSize(1);
  display.setRotation(1);
  display.setTextColor(SH110X_WHITE);
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
  logger.print(F("Particles > 50 um / 0.1L air:")); logger.println(data.particles_100um);
  logger.println(F("---------------------------------------"));

  display.clearDisplay();
  display.setCursor(0,0);
  display.print("PM 1.0: "); display.println(data.pm10_env);
  display.print("PM 2.5: "); display.println(data.pm25_env);
  display.print("PM 10: "); display.println(data.pm100_env);
  
  display.print("Part's >0.3um: "); display.println(data.particles_03um);
  display.print("Part's >0.5um: "); display.println(data.particles_05um);
  display.print("Part's >1.0um: "); display.println(data.particles_10um);
  display.print("Part's >2.5um: "); display.println(data.particles_25um);
  display.print("Part's >5.0um: "); display.println(data.particles_50um);
  display.print("Part's >10um: "); display.println(data.particles_100um);
  display.display(); // actually display all of the above

  delay(500);
}