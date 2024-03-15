/**
 * @file      MinimalModemNBIOTExample.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2022-09-16
 *
 */
//#include <Arduino.h>
//#define XPOWERS_CHIP_AXP2101
//#include "XPowersLib.h"
#include <Canarin_Utilities.h>

XPowersPMU  PMU;

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

#define TINY_GSM_RX_BUFFER 1024

#define TINY_GSM_MODEM_SIM7080
#include <TinyGsmClient.h>
//#include "utilities.h"


#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(Serial1, Serial);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif

const char *register_info[] = {
    "Not registered, MT is not currently searching an operator to register to.The GPRS service is disabled, the UE is allowed to attach for GPRS if requested by the user.",
    "Registered, home network.",
    "Not registered, but MT is currently trying to attach or searching an operator to register to. The GPRS service is enabled, but an allowable PLMN is currently not available. The UE will start a GPRS attach as soon as an allowable PLMN is available.",
    "Registration denied, The GPRS service is disabled, the UE is not allowed to attach for GPRS if it is requested by the user.",
    "Unknown.",
    "Registered, roaming.",
};


enum {
    MODEM_CATM = 1,
    MODEM_NB_IOT,
    MODEM_CATM_NBIOT,
};

void getPsmTimer();


// const char apn[] = "ibasis.iot";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Server details to test TCP/SSL
const char server[]   = "vsh.pp.ua";
const char resource[] = "/TinyGSM/logo.txt";

//!! Set the APN manually. Some operators need to set APN first when registering the network.
//!! Set the APN manually. Some operators need to set APN first when registering the network.
//!! Set the APN manually. Some operators need to set APN first when registering the network.
// Using 7080G with Hologram.io , https://github.com/Xinyuan-LilyGO/LilyGo-T-SIM7080G/issues/19
// const char *apn = "hologram";

const char *apn = "TM";

void setup()
{

    Serial.begin(115200);

    //Start while waiting for Serial monitoring
    //while (!Serial);

    delay(3000);

    logger.println();

    /*********************************
     *  step 1 : Initialize power chip,
     *  turn on modem and gps antenna power channel
    ***********************************/
    if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, I2C_SDA, I2C_SCL)) {
        logger.println("Failed to initialize power.....");
        while (1) {
            delay(5000);
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
    pinMode(BOARD_MODEM_DTR_PIN, OUTPUT);
    pinMode(BOARD_MODEM_RI_PIN, INPUT);
  
    int retry = 0;
    while (!modem.testAT(1000)) {
        logger.print(".");
        if (retry++ > 6) {
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
    logger.println("Modem started!");


    /*********************************
     * step 3 : Check if the SIM card is inserted
    ***********************************/
    String result ;

    logger.println("---------->"+(String)modem.getSimStatus());
    //logger.println("---------->"+(String)modem.simUnlock("0000"));
    //modem.simUnlock("0000");
    logger.println("---------->"+(String)modem.getSimStatus());


    if (modem.getSimStatus() != SIM_READY) {
        logger.println("SIM Card is not insert!!!");
        return ;
    }

    // Disable RF
    modem.sendAT("+CFUN=0");
    if (modem.waitResponse(20000UL) != 1) {
        logger.println("Disable RF Failed!");
    }

  

    /*********************************
     * step 4 : Set the network mode to NB-IOT
    ***********************************/
    logger.println("test the network mode ");
    logger.println(modem.getNetworkModes());
    logger.println(modem.getPreferredModes() );
    delay(10000);

/*
for (int i = 0; i <= 4; i++) {
      logger.println("    test the network mode "+(String)i);
    uint8_t network[] = {
        2,  //Automatic
        13, //GSM only
        38, //LTE only
        51  //GSM and LTE only
    };
    logger.printf("Try %d method\n", network[i]);
    while (modem.getNetworkMode() != network[i])
    {
      logger.println("SETTING NETWORK TO "+(String)network[i]); 
      modem.setNetworkMode(network[i]);
      delay(3000);
    }

    delay(3000);
    bool isConnected = false;
    int tryCount = 60;
    while (tryCount--) {
      int16_t signal =  modem.getSignalQuality();
      logger.print(tryCount + 1);
      logger.print(" ");
      logger.print(network[i]);
      logger.print(" ");
      logger.print(modem.getNetworkMode() );
      logger.print(" ");
      logger.print(modem.getPreferredMode() );
      logger.print(" ");
      logger.print("Signal: ");
      logger.print(signal);
      logger.print(" ");
      logger.print("isNetworkConnected: ");
      isConnected = modem.isNetworkConnected();
      logger.println( isConnected ? "CONNECT" : "NO CONNECT");
      if (isConnected) {
        break;
      }
      delay(500);
    }
    if (isConnected) {
        break;
    }
  }

  logger.println();
  logger.println("Device is connected .");
  logger.println();
*/

    logger.println("Set the network mode to NB-IOT");

    modem.setNetworkMode(2);    //use automatic

    modem.setPreferredMode(MODEM_NB_IOT);
    modem.setPreferredMode(3);

    uint8_t pre = modem.getPreferredMode();

    uint8_t mode = modem.getNetworkMode();

    logger.printf("getNetworkMode:%u getPreferredMode:%u\n", mode, pre);


    //Set the APN manually. Some operators need to set APN first when registering the network.
    modem.sendAT("+CGDCONT=1,\"IP\",\"", apn, "\"");
    if (modem.waitResponse() != 1) {
        logger.println("Set operators apn Failed!");
        return;
    }

    //!! Set the APN manually. Some operators need to set APN first when registering the network.
    modem.sendAT("+CNCFG=0,1,\"", apn, "\"");
    if (modem.waitResponse() != 1) {
        logger.println("Config apn Failed!");
        return;
    }

    // Enable RF
    modem.sendAT("+CFUN=1");
    if (modem.waitResponse(20000UL) != 1) {
        logger.println("Enable RF Failed!");
    }

    /*********************************
    * step 5 : Wait for the network registration to succeed
    ***********************************/
    logger.println("Wait for the network registration to succeed");
    RegStatus s;
    do {
        s = modem.getRegistrationStatus();
        logger.print("=============>");
        logger.println(s);
        if (s != REG_OK_HOME && s != REG_OK_ROAMING) {
            logger.print(".");
            PMU.setChargingLedMode(level ? XPOWERS_CHG_LED_ON : XPOWERS_CHG_LED_OFF);
            level ^= 1;
            delay(1000);
        }

    } while (s != REG_OK_HOME && s != REG_OK_ROAMING) ;

    logger.println();
    logger.println("Network register info:");
    logger.println(register_info[s]);





    // Activate network bearer, APN can not be configured by default,
    // if the SIM card is locked, please configure the correct APN and user password, use the gprsConnect() method
    modem.sendAT("+CNACT=0,1");
    if (modem.waitResponse() != 1) {
        logger.println("Activate network bearer Failed!");
        return;
    }

    // if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    //     return ;
    // }

    bool res = modem.isGprsConnected();
    logger.print("GPRS status:");
    logger.println(res ? "connected" : "not connected");

    String ccid = modem.getSimCCID();
    logger.print("CCID:");
    logger.println(ccid);

    String imei = modem.getIMEI();
    logger.print("IMEI:");
    logger.println(imei);

    String imsi = modem.getIMSI();
    logger.print("IMSI:");
    logger.println(imsi);

    String cop = modem.getOperator();
    logger.print("Operator:");
    logger.println(cop);

    IPAddress local = modem.localIP();
    logger.print("Local IP:");
    logger.println(local);

    int csq = modem.getSignalQuality();
    logger.print("Signal quality:");
    logger.println(csq);

    /*********************************
    * step 6 : Send HTTP request
    ***********************************/
    logger.println("Send HTTP request");

    TinyGsmClient client(modem, 0);
    const int     port = 80;
    logger.printf("Connecting to", server);
    if (!client.connect(server, port)) {
        logger.println("... failed");
    } else {
        // Make a HTTP GET request:
        client.print(String("GET ") + resource + " HTTP/1.0\r\n");
        client.print(String("Host: ") + server + "\r\n");
        client.print("Connection: close\r\n\r\n");

        // Wait for data to arrive
        uint32_t start = millis();
        while (client.connected() && !client.available() &&
                millis() - start < 30000L) {
            delay(100);
        };

        // Read data
        start          = millis();
        char logo[640] = {
            '\0',
        };
        int read_chars = 0;
        while (client.connected() && millis() - start < 10000L) {
            while (client.available()) {
                logo[read_chars]     = client.read();
                logo[read_chars + 1] = '\0';
                read_chars++;
                start = millis();
            }
        }
        logger.println(logo);
        logger.print("#####  RECEIVED:");
        logger.print(strlen(logo));
        logger.println("CHARACTERS");
        client.stop();
    }

    /*********************************
    * step 6 : Send HTTPS request
    ***********************************/
    logger.println("Send HTTPS request");
    TinyGsmClientSecure secureClient(modem, 1);
    const int           securePort = 443;
    logger.printf("Connecting securely to", server);
    if (!secureClient.connect(server, securePort)) {
        logger.println("... failed");
    } else {
        // Make a HTTP GET request:
        secureClient.print(String("GET ") + resource + " HTTP/1.0\r\n");
        secureClient.print(String("Host: ") + server + "\r\n");
        secureClient.print("Connection: close\r\n\r\n");

        // Wait for data to arrive
        uint32_t startS = millis();
        while (secureClient.connected() && !secureClient.available() &&
                millis() - startS < 30000L) {
            delay(100);
        };

        // Read data
        startS          = millis();
        char logoS[640] = {
            '\0',
        };
        int read_charsS = 0;
        while (secureClient.connected() && millis() - startS < 10000L) {
            while (secureClient.available()) {
                logoS[read_charsS]     = secureClient.read();
                logoS[read_charsS + 1] = '\0';
                read_charsS++;
                startS = millis();
            }
        }
        logger.println(logoS);
        logger.print("#####  RECEIVED:");
        logger.print(strlen(logoS));
        logger.println("CHARACTERS");
        secureClient.stop();
    }
}

void loop()
{
  
    while (Serial1.available()) {
        Serial.write(Serial1.read());
    }
    while (Serial.available()) {
        Serial1.write(Serial.read());
    }
    
}
/*
void test(){

    logger.println("*** Fin setup ****");;
    delay(1000);
   
    
    Serial1.write("AT+CMNB=3");
    delay(1000);
    Serial1.
    Serial1.waitResponse() == 1;
    
    Serial1.write("AT+CFUN=1");
    delay(1000);
    Serial1.waitResponse() == 1;
    Serial1.write("AT+CGREG?");
    delay(1000);
    Serial1.waitResponse() == 1;
    Serial1.write("AT+CSQ");
    delay(1000);
    Serial1.waitResponse() == 1;
    Serial1.write("AT+CMGF=1");
    delay(1000);
    Serial1.waitResponse() == 1;
    Serial1.write("AT+CSCS=\"GSM\"");
    delay(1000);
    Serial1.waitResponse() == 1;
    Serial1.write("AT+CMGS=+33674562432");
    delay(1000);
    Serial1.waitResponse() == 1;
    Serial1.write("Hello from Canarin");
    Serial1.write(0x1A);
    delay(1000);
    Serial1.waitResponse() == 1;
    modem.sendSMS("+33630807628", "TEST SMS CANARIN POST");
}*/
