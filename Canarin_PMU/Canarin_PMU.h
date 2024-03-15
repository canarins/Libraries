

#ifndef CANARIN_PMU_H_
#define CANARIN_PMU_H_



#include <Adafruit_MPU6050.h>

Adafruit_MPU6050 mpu;



//#ifndef CONFIG_PMU_IRQ
//#define CONFIG_PMU_IRQ 35
//#endif

bool pmu_flag = 0;


//XPowersPMU  PMU;
XPowersAXP2101 PMU;
//const uint8_t pmu_irq_pin = CONFIG_PMU_IRQ;
void setFlag(void) {
  pmu_flag = true;
}


bool pmu_setup = false;
void Canarin_PMU_setup(){
  

  if (pmu_setup)
  return;
  //Wire1.begin(I2C_SDA_2, I2C_SCL_2);
  Wire.begin(13, 21);//TODO change variablev 13 21 15 7
  Wire1.begin(15, 7);

  /*********************************
     *  step 1 : Initialize power chip,
     *  turn on modem and gps antenna power channel
    ***********************************/
  //if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, I2C_SDA_2, I2C_SCL_2)) {
  if (!PMU.begin(Wire1, AXP2101_SLAVE_ADDRESS, 15, 7)) {//TODO change 15 7
    //logger.println("Failed to initialize power.....");
    logger.println("Failed to initialize power.....");
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));  //display Red color Failed to initialize power.
    pixels.show();
    while (1) {
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));  //display Red color Failed to initialize power.
      pixels.show();
      delay(250);
      pixels.setPixelColor(0, pixels.Color(0, 0, 0));
      pixels.show();
      delay(250);
      logger.println("Failed to initialize power!");
      //logger.println("Failed to initialize power");
    }
  }

  // Force add pull-up
  //pinMode(pmu_irq_pin, INPUT_PULLUP);
  //  attachInterrupt(pmu_irq_pin, setFlag, FALLING);


  // Disable all interrupts
  //PMU.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);

  // Print interrupt register
  //PMU.printIntRegister(&Serial);

  // Clear all interrupt flags
  //PMU.clearIrqStatus();

  // Enable the required interrupt function
  // PMU.enableIRQ(
  //     XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
  //     XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
  //     XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |   //POWER KEY
  //     XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ       //CHARGE
  //     // XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ | XPOWERS_AXP2101_PKEY_POSITIVE_IRQ   |   //POWER KEY
  // );

  PMU.enableIRQ(XPOWERS_AXP2101_BAT_NOR_UNDER_TEMP_IRQ);
  PMU.enableIRQ(XPOWERS_AXP2101_VBUS_INSERT_IRQ);


  PMU.enableIRQ(XPOWERS_AXP2101_BAT_OVER_VOL_IRQ);
  // Print AXP2101 interrupt control register
  //PMU.printIntRegister(&Serial);
  //Set the working voltage of the modem, please do not modify the parameters
  PMU.setBLDO1Voltage(3300);  //Set the power supply for level conversion to 3300mV
  PMU.enableBLDO1();

  PMU.setDC3Voltage(3000);  //SIM7080 Modem main power channel 2700~ 3400V
  PMU.enableDC3();
  //Modem GPS Power channel
  PMU.setBLDO2Voltage(3300);
  PMU.enableBLDO2();  //The antenna power must be turned on to use the GPS function



  PMU.setSysPowerDownVoltage(3000);
  // TS Pin detection must be disable, otherwise it cannot be charged
  PMU.disableTSPinMeasure();
  PMU.setSysPowerDownVoltage(3000);
  PMU.disableTSPinMeasure();
  PMU.enableWakeup();

  PMU.enableWakeup();  // Activer le réveil par vbus

pmu_setup = true;

}


void PMU_Canarin_status(){
  
  delay(50);
  //logger.println(F("--------voltage---------------"));
  logger.print("isCharging:");
  logger.println(PMU.isCharging() ? "YES" : "NO");
  delay(150);
  logger.print("isVbusIn:");
  logger.println(PMU.isVbusIn() ? "YES" : "NO");
  delay(150);
  logger.print("getBattVoltage:");
  logger.print(PMU.getBattVoltage());
  delay(50);
  logger.println("mV");
  delay(150);
  // logger.println(F("---------------------------------------"));

  delay(1000);
  
  

}


#endif /* CANARIN_PMU_H_ */