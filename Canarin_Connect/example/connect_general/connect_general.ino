/**                              TODO LICENSE !!!!!!!!!!!
 * Important claim -
 *      I am not the original author of this code.
 *      This repository includes codes that was created using examples from https://github.com/Xinyuan-LilyGO/LilyGo-T-SIM7080G/tree/master/examples
 *      I have adapted and modified this code to suit my specific needs, but I want to acknowledge and give credit to the original authors Lewis He (lewishe@outlook.com) for his work.
 *      Thank Lewis He (lewishe@outlook.com) for providing these helpful examples.
 *      Please note that my use of this code falls under MIT license or terms of use of the original source,
 *      I am not responsible for any damages or issues that may arise from the use of this code.
 * @brief     Support T-SIM7080G to access a testing HTTP/HTTPS server through AT&T NB-IOT radio
 * @license   MIT
 * @date      2023-04-10
 * @version   1.0.0
 */

//#include <Arduino.h>
//#define XPOWERS_CHIP_AXP2101
//#include "XPowersLib.h"
#include <Canarin_Utilities.h>
//#include <Canarin_Logger.h>

#include "parameter.h"

#include <Canarin_Connect.h>

#include <SPI.h>
#include <chrono>



const IPAddress REMOTE_UDP_ADDR = IPAddress(18, 184, 137, 24);

/* Initialization modem */
void setup() {

  // add__("Ariele_hotspot_android", SSID_array, currentSize);

  setup_parameters();

  /* Serial.begin(115200);

   // Start while waiting for Serial monitoring
   //while (!Serial);
   delay(3000);
   logger.println();*/

  /***********************************
   *  step 1 : Initialize power chip, turn on modem and gps antenna power channel
   ***********************************/

  logger.println("............................................................................Step 1");
  logger.println("start to initialize power chip, turn on modem and gps antenna power channel ");

  

  logger.println("Step 1 done !");
  /***********************************
   * step 2 : start modem
   ***********************************/

  logger.println("............................................................................Step 2");
  logger.println("Step 2 setup connectics !");



  Connectic_setup();

  logger.println("Step 2 done !");

  logger.println("-----------------------------------------------------------------------------------------------------------------");

  while (1) {
    logger.println("-----------------------------------------------------------------------------------------------------------------");

    const auto p1 = std::chrono::system_clock::now();//TODO use NTP if Configuration Variable is true
    // logger.println(p1);
    auto duration = p1.time_since_epoch();

    char buffer[20];  // Ensure this buffer is large enough to hold the number
    sprintf(buffer, "%lld", duration);

    //execute_with_minimum_duration(Connect, 1);

    connect();
    logger.println("-----------------------------------------------------------------------------------------------------------------");

    char dataS[680];


//---------------------------------------------------------------------------------------------------------------------------------------

    char *str0 = "{0,0,12}";
    snprintf(dataS, sizeof(dataS), "%s", str0);
    logger.println(dataS);
    logger.println(sizeof(dataS));

    //execute_with_minimum_duration(Send, 1, REMOTE_UDP_ADDR, 60004, dataS);
    String RESPONSE_ = Send( REMOTE_UDP_ADDR, 60004, dataS);

    
    logger.println("---->");
    logger.println("RESPONSE 0 : " + (String)RESPONSE_);
    logger.println("-----------------------------------------------------------------------------------------------------------------");

//---------------------------------------------------------------------------------------------------------------------------------------
  /*



    char *str1 = "{1,0,12,";
    char *str2 = ",1:0,2:0,3:0,7:0,8:0,9:0,4:0,5:0,6:0,40:0,41:0,42:0,10:0,50:0,45:0,46:0,47:0,48:0}";

    size_t totalLen = strlen(str1) + sizeof(buffer) + strlen(str2);

    // Format and concatenate the string safely
    snprintf(dataS, sizeof(dataS), "%s%s%s", str1, buffer, str2);

    logger.println(dataS);
    logger.println(sizeof(dataS));
    execute_with_minimum_duration(Send, 1, REMOTE_UDP_ADDR, 60004, dataS);

    logger.println("RESPONSE 1 : " + (String)RESPONSE);

    logger.println("-----------------------------------------------------------------------------------------------------------------");
//---------------------------------------------------------------------------------------------------------------------------------------


    char *str3 = "{2,0,12}";
    totalLen = strlen(str1);
    dataS[totalLen] = { '\0' };
    // Format and concatenate the string safely
    snprintf(dataS, sizeof(dataS), "%s", str3);

    logger.println(dataS);
    logger.println(sizeof(dataS));
    execute_with_minimum_duration(Send, 1, REMOTE_UDP_ADDR, 60004, dataS);



    logger.println("RESPONSE 2 : " + (String)RESPONSE);

    logger.println("-----------------------------------------------------------------------------------------------------------------");

//---------------------------------------------------------------------------------------------------------------------------------------
*/

    execute_with_minimum_duration(Close, 1);

    logger.println("-----------------------------------------------------------------------------------------------------------------");
    logger.println("-----------------------------------------------------------------------------------------------------------------");
  }


  return;


  /***********************************
   * step 6 : Check if the network bearer is activated
   ***********************************/

  logger.println("............................................................................Step 6");
  logger.println("start to activate the network bearer");

  // Check the status of the network bearer
  logger.println("Checking the status of network bearer ...");
  modem.sendAT("+CNACT?");  // Send the AT command to query the network bearer status
  String response;
  int8_t ret = modem.waitResponse(10000UL, response);  // Wait for the response with a 10-second timeout

  bool alreadyActivated = false;
  if (response.indexOf("+CNACT: 0,1") >= 0)  // Check if the response contains "+CNACT: 0,1" indicating bearer is activated
  {
    logger.println("Network bearer is already activated");
    alreadyActivated = true;
  } else if (response.indexOf("+CNACT: 0,0") >= 0)  // Check if the response contains "+CNACT: 0,0" indicating bearer is deactivated
  {
    logger.println("Network bearer is not activated");
  }

  if (!alreadyActivated) {
    // Activating network bearer
    logger.println("Activating network bearer ...");
    modem.sendAT("+CNACT=0,1");  // Send the AT command to activate the network bearer
    response = "";
    ret = modem.waitResponse(10000UL, response);  // Wait for the response with a 10-second timeout

    if (response.indexOf("ERROR") >= 0)  // Check if the response contains "ERROR"
    {
      logger.println("Network bearer activation failed");
    } else if (response.indexOf("OK") >= 0)  // Check if the response contains "OK"
    {
      logger.println("Activation in progress, waiting for network response...");

      // Wait for the "+APP PDP: 0,ACTIVE" response
      bool activationConfirmed = false;
      unsigned long startTime = millis();
      while (millis() - startTime < 60000UL)  // Wait for 60 seconds
      {
        if (modem.stream.available()) {
          response = modem.stream.readString();
          if (response.indexOf("+APP PDP: 0,ACTIVE") >= 0) {
            activationConfirmed = true;
            break;
          }
        }
        delay(100);
      }
      if (activationConfirmed) {
        logger.println("Network bearer is activated successfully !");
      } else {
        logger.println("No network response within the timeout");
      }
    } else {
      logger.println("No valid response");
    }
  }
  // Ping the Google DNS server
  modem.sendAT("+SNPING4=\"8.8.8.8\",1,16,5000");
  if (modem.waitResponse(10000L) != 1) {
    logger.println("Ping Failed!");
    return;
  } else {
    logger.println(response);
  }

  logger.println("Step 6 done !");
  /***********************************
   * step 7 : Get the modem, SIM and network information
   ***********************************/

  logger.println("............................................................................Step 7");
  logger.println("show the information of the Modem, SIM and network  ");

  logger.println("T-SIM7080G Firmware Version: ");
  modem.sendAT("+CGMR");
  if (modem.waitResponse(10000L) != 1) {
    logger.println("Get Firmware Version Failed!");
  } else {
    logger.println(response);
  }
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

  modem.sendAT("+CGNAPN");
  if (modem.waitResponse(10000L) != 1) {
    logger.println("Get APN Failed!");
    return;
  }

  modem.sendAT("+CCLK?");
  if (modem.waitResponse(10000L) != 1) {
    logger.println("Get time Failed!");
    return;
  }
  logger.println("Step 7 done !");

  /***********************************
   * step 8 : Access HTTP Server at demo4570913.mockable.io (this is a test server created by my account on mockable.io)
   ***********************************/
  const char *Resource = "/";  // Assuming you want to access the root path
  /*
  logger.println("............................................................................Step 8");
  logger.println("start to connect the HTTP server ");

  const int HTTP_Port = 80;
  const char *HTTP_Server = "demo4570913.mockable.io";




  GetHTTPData(HTTP_Server, HTTP_Port, Resource);

  logger.println("Step 8 done !");*/
  /***********************************
   * step 9 : Access HTTPS Server at demo4570913.mockable.io (this is a test server created by my account on mockable.io)
   ***********************************/

  logger.println("............................................................................Step HTTPS CLASSIC");
  logger.println("start to connect the HTTPS server ");

  const int HTTPS_Port = 443;
  const char *HTTPS_Server = "demo4570913.mockable.io";

  GetHTTPSData(HTTPS_Server, HTTPS_Port, Resource, "");

  logger.println("Step 9 done !");
  logger.println("............................................................................Step 9 HTTP pour OTA");

  const char *amazon_root_ca = /* Amazon Root CA 1 */
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsFADA5MQswCQYD\n"
    "VQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6b24gUm9vdCBDQSAxMB4XDTE1\n"
    "MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTELMAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpv\n"
    "bjEZMBcGA1UEAxMQQW1hem9uIFJvb3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\n"
    "ggEBALJ4gHHKeNXjca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgH\n"
    "FzZM9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qwIFAGbHrQ\n"
    "gLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6VOujw5H5SNz/0egwLX0t\n"
    "dHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L93FcXmn/6pUCyziKrlA4b9v7LWIbxcce\n"
    "VOF34GfID5yHI9Y/QCB/IIDEgEw+OyQmjgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB\n"
    "/zAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3\n"
    "DQEBCwUAA4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDIU5PM\n"
    "CCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUsN+gDS63pYaACbvXy\n"
    "8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vvo/ufQJVtMVT8QtPHRh8jrdkPSHCa\n"
    "2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2\n"
    "xJNDd2ZhwLnoQdeXeGADbkpyrqXRfboQnoZsG4q5WTP468SQvvG5\n"
    "-----END CERTIFICATE-----\n" /* Amazon Root CA 2 */
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "-----END CERTIFICATE-----\n" /* Amazon Root CA 3 */
    "-----BEGIN CERTIFICATE-----\n"
    "MIIBtjCCAVugAwIBAgITBmyf1XSXNmY/Owua2eiedgPySjAKBggqhkjOPQQDAjA5MQswCQYDVQQG\n"
    "EwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6b24gUm9vdCBDQSAzMB4XDTE1MDUy\n"
    "NjAwMDAwMFoXDTQwMDUyNjAwMDAwMFowOTELMAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZ\n"
    "MBcGA1UEAxMQQW1hem9uIFJvb3QgQ0EgMzBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABCmXp8ZB\n"
    "f8ANm+gBG1bG8lKlui2yEujSLtf6ycXYqm0fc4E7O5hrOXwzpcVOho6AF2hiRVd9RFgdszflZwjr\n"
    "Zt6jQjBAMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/BAQDAgGGMB0GA1UdDgQWBBSrttvXBp43\n"
    "rDCGB5Fwx5zEGbF4wDAKBggqhkjOPQQDAgNJADBGAiEA4IWSoxe3jfkrBqWTrBqYaGFy+uGh0Psc\n"
    "eGCmQ5nFuMQCIQCcAu/xlJyzlvnrxir4tiz+OpAUFteMYyRIHN8wfdVoOw==\n"
    "-----END CERTIFICATE-----\n" /* Amazon Root CA 4 */
    "-----BEGIN CERTIFICATE-----\n"
    "MIIB8jCCAXigAwIBAgITBmyf18G7EEwpQ+Vxe3ssyBrBDjAKBggqhkjOPQQDAzA5MQswCQYDVQQG\n"
    "EwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6b24gUm9vdCBDQSA0MB4XDTE1MDUy\n"
    "NjAwMDAwMFoXDTQwMDUyNjAwMDAwMFowOTELMAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZ\n"
    "MBcGA1UEAxMQQW1hem9uIFJvb3QgQ0EgNDB2MBAGByqGSM49AgEGBSuBBAAiA2IABNKrijdPo1MN\n"
    "/sGKe0uoe0ZLY7Bi9i0b2whxIdIA6GO9mif78DluXeo9pcmBqqNbIJhFXRbb/egQbeOc4OO9X4Ri\n"
    "83BkM6DLJC9wuoihKqB1+IGuYgbEgds5bimwHvouXKNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n"
    "HQ8BAf8EBAMCAYYwHQYDVR0OBBYEFNPsxzplbszh2naaVvuc84ZtV+WBMAoGCCqGSM49BAMDA2gA\n"
    "MGUCMDqLIfG9fhGt0O9Yli/W651+kI0rz2ZVwyzjKKlwCkcO8DdZEv8tmZQoTipPNU0zWgIxAOp1\n"
    "AE47xDqUEpHJWEadIRNyp4iciuRMStuW1KyLa2tJElMzrdfkviT8tQp21KW8EA==\n"
    "-----END CERTIFICATE-----\n";

  static char const OTA_FILE_LOCATION[] = "https://raw.githubusercontent.com/arduino-libraries/Arduino_ESP32_OTA/main/examples/NANO_ESP32_Blink/NANO_ESP32_Blink.ino.ota";

  String res_;
  modem.sendAT("+CFSDFILE=?");
  if (modem.waitResponse(5000UL, res_) != 1) {
    logger.println("CFSINIT");
    return;
  }
  // logger.println(res_);
  //   return;

  Write_FS("certificate.cert", amazon_root_ca);

  modem.sendAT("+CFSDFILE=?");
  if (modem.waitResponse(5000UL, res_) != 1) {
    logger.println("CFSINIT");
    return;
  }

  char *test_read = Read_FS("certificate.cert");

  GetHTTPSData("https://raw.githubusercontent.com", 443, "/arduino-libraries/Arduino_ESP32_OTA/main/examples/NANO_ESP32_Blink/NANO_ESP32_Blink.ino.ota", "certificate.cert");

  return;
  logger.println("............................................................................Step 10");
  logger.println("FS experimentation ");

  const char *toto_content =
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIEAzCCAuugAwIBAgIUBY1hlCGvdj4NhBXkZ/uLUZNILAwwDQYJKoZIhvcNAQEL\r\n"
    "BQAwgZAxCzAJBgNVBAYTAkdCMRcwFQYDVQQIDA5Vbml0ZWQgS2luZ2RvbTEOMAwG\r\n"
    "A1UEBwwFRGVyYnkxEjAQBgNVBAoMCU1vc3F1aXR0bzELMAkGA1UECwwCQ0ExFjAU\r\n"
    "BgNVBAMMDW1vc3F1aXR0by5vcmcxHzAdBgkqhkiG9w0BCQEWEHJvZ2VyQGF0Y2hv\r\n"
    "by5vcmcwHhcNMjAwNjA5MTEwNjM5WhcNMzAwNjA3MTEwNjM5WjCBkDELMAkGA1UE\r\n"
    "BhMCR0IxFzAVBgNVBAgMDlVuaXRlZCBLaW5nZG9tMQ4wDAYDVQQHDAVEZXJieTES\r\n"
    "MBAGA1UECgwJTW9zcXVpdHRvMQswCQYDVQQLDAJDQTEWMBQGA1UEAwwNbW9zcXVp\r\n"
    "dHRvLm9yZzEfMB0GCSqGSIb3DQEJARYQcm9nZXJAYXRjaG9vLm9yZzCCASIwDQYJ\r\n"
    "KoZIhvcNAQEBBQADggEPADCCAQoCggEBAME0HKmIzfTOwkKLT3THHe+ObdizamPg\r\n"
    "UZmD64Tf3zJdNeYGYn4CEXbyP6fy3tWc8S2boW6dzrH8SdFf9uo320GJA9B7U1FW\r\n"
    "Te3xda/Lm3JFfaHjkWw7jBwcauQZjpGINHapHRlpiCZsquAthOgxW9SgDgYlGzEA\r\n"
    "s06pkEFiMw+qDfLo/sxFKB6vQlFekMeCymjLCbNwPJyqyhFmPWwio/PDMruBTzPH\r\n"
    "3cioBnrJWKXc3OjXdLGFJOfj7pP0j/dr2LH72eSvv3PQQFl90CZPFhrCUcRHSSxo\r\n"
    "E6yjGOdnz7f6PveLIB574kQORwt8ePn0yidrTC1ictikED3nHYhMUOUCAwEAAaNT\r\n"
    "MFEwHQYDVR0OBBYEFPVV6xBUFPiGKDyo5V3+Hbh4N9YSMB8GA1UdIwQYMBaAFPVV\r\n"
    "6xBUFPiGKDyo5V3+Hbh4N9YSMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEL\r\n"
    "BQADggEBAGa9kS21N70ThM6/Hj9D7mbVxKLBjVWe2TPsGfbl3rEDfZ+OKRZ2j6AC\r\n"
    "6r7jb4TZO3dzF2p6dgbrlU71Y/4K0TdzIjRj3cQ3KSm41JvUQ0hZ/c04iGDg/xWf\r\n"
    "+pp58nfPAYwuerruPNWmlStWAXf0UTqRtg4hQDWBuUFDJTuWuuBvEXudz74eh/wK\r\n"
    "sMwfu1HFvjy5Z0iMDU8PUDepjVolOCue9ashlS4EB5IECdSR2TItnAIiIwimx839\r\n"
    "LdUdRudafMu5T5Xma182OC0/u/xRlEm+tvKGGmfFcN0piqVl8OrSPBgIlb+1IKJE\r\n"
    "m/XriWr/Cq4h/JfB7NTsezVslgkBaoU=\r\n"
    "-----END CERTIFICATE-----\r\n";
  const char *toto_filename = "TOTO.pem";

  FS_testing(toto_filename, toto_content);

  logger.println("............................................................................Step 10");

  // String GSMLocation = modem.getGsmLocationRaw();
  // if (modem.waitResponse(10000L) != 1) {
  //   logger.println("ERROR GSMLocation");
  //   return;
  // }

  // logger.println("Les Données de Geolocation"+GSMLocation);

  const auto p1 = std::chrono::system_clock::now();
  // logger.println(p1);
  auto duration = p1.time_since_epoch();

  char buffer[20];  // Ensure this buffer is large enough to hold the number
  sprintf(buffer, "%lld", duration);

  logger.println("SENDING CRAP " + (String)buffer + "  " + (String)sizeof(buffer));

  // char dataS[640] = {'\0',};

  // strcpy(dataS, "{1,71350336,9,");  // Copy the first string into the result buffer

  // strcat(dataS, p1);

  // strcat(dataS, ",1:0,2:0,3:0,7:0,8:0,9:0,4:0,5:0,6:0,40:0,41:0,42:0,10:0,50:0,45:0,46:0,47:0,48:0}");

  // example "{1,71350336,9,67265,1:0,2:0,3:0,7:0,8:0,9:0,4:0,5:0,6:0,40:0,41:0,42:0,10:0,50:0,45:0,46:0,47:0,48:0}"

  char *str1 = "{1,71350336,9,";
  logger.println("SENDING CRAP " + (String)str1 + "  " + (String)strlen(str1));
  char *str2 = ",1:0,2:0,3:0,7:0,8:0,9:0,4:0,5:0,6:0,40:0,41:0,42:0,10:0,50:0,45:0,46:0,47:0,48:0}";
  logger.println("SENDING CRAP " + (String)str2 + "  " + (String)strlen(str2));

  size_t totalLen = strlen(str1) + sizeof(buffer) + strlen(str2);
  logger.println("SENDING CRAP " + (String)totalLen);
  char dataS[totalLen] = { '\0' };
  logger.println("SENDING CRAP");
  // Format and concatenate the string safely
  snprintf(dataS, sizeof(dataS), "%s%s%s", str1, buffer, str2);

  logger.println(dataS);
  logger.println(sizeof(dataS));

  char *message1 = sendData(dataS);

  logger.println("message1: " + (String)message1);

  return;

  char *message2 = sendData("{2,71350336,9}");

  logger.println("message2: " + (String)message2);

  sendSMS("+33630807628", "TOTO 1 est un test");
  modem.sendSMS("+33630807628", "TOTO 2 est un test");

  sendSMS("+33681689093", "TOTO 1 est un test d'envoie du module GSM du Lilygo7080 en commande AT");

  modem.sendSMS("+33681689093", "TOTO 2 est un test d'envoie du module GSM du Lilygo7080 en commande AT");

  getGSMLOC();

  logger.println("*** Fin setup ****");
  ;
  //  delay(1000);
  //  while (SerialAT.available()) {
  //      logger.write(SerialAT.read());
  //  }
  //  SerialAT.write("AT+CMNB=3");
  //  delay(1000);
  //  while (SerialAT.available()) {
  //      logger.write(SerialAT.read());
  //  }
  //  SerialAT.write("AT+CFUN=1");
  //  delay(1000);
  //  while (SerialAT.available()) {
  //      logger.write(SerialAT.read());
  //  }
  //  SerialAT.write("AT+CGREG?");
  //  delay(1000);
  //  while (SerialAT.available()) {
  //      logger.write(SerialAT.read());
  //  }

  logger.println("Step UDP done !");

  logger.println("Test Serial write");
  String res;
  SerialAT.write("ATI");

  ret = modem.waitResponse(20000L, res);
  if (ret != 1) {
    logger.println("Error " + (String)ret);
    // return;
  } else
    logger.println("res");
  delay(5000);
  logger.println("Test Serial print");
  SerialAT.print("ATI");
  res = "";
  ret = modem.waitResponse(20000L, res);
  if (ret != 1) {
    logger.println("Error " + (String)ret);
    // return;
  } else
    logger.println(res);
  delay(5000);
  logger.println("Test modem sendAT");
  modem.sendAT("I");
  res = "";
  ret = modem.waitResponse(20000L, res);
  if (ret != 1) {
    logger.println("Error " + (String)ret);
    // return;
  } else
    logger.println(res);

  res = modem.getModemInfo();
  logger.println("------->" + res);

  logger.println("Test Serial END");
}

void loop() {
  while (1) {
    while (logger.available()) {
      SerialAT.write(logger.read());
    }
    while (SerialAT.available()) {
      logger.write(SerialAT.read());
    }
  }
}

void Write_FS(const char *file, const char *content) {

  modem.sendAT("+CFSINIT");
  if (modem.waitResponse(5000UL, "OK") != 1) {
    logger.println("CFSINIT");
    return;
  } else
    logger.println("");

  modem.sendAT("+CFSGFRS?");
  if (modem.waitResponse(5000UL, "+CFSGFRS:") == 1) {
    if (modem.waitResponse(5000UL, "OK") != 1) {
      logger.println("Write" + (String)file + "file failed!");
      return;
    } else
      logger.println("");
  } else {
    logger.println("CFSGFRS ERROR");
    return;
  }

  modem.sendAT("+CFSWFILE=0,\"", file, "\",0,", strlen(content), ",", 10000UL);
  if (modem.waitResponse(10000UL, "DOWNLOAD") == 1) {
    modem.stream.write(content);
    if (modem.waitResponse(5000UL, "OK") != 1) {
      logger.println("Write" + (String)file + "file failed!");
      return;
    } else
      logger.println("");
  } else {
    logger.println("CFSWFILE ERROR");
    return;
  }

  modem.sendAT("+CFSGFIS=0,\"", file, "\"");
  if (modem.waitResponse(10000UL, "CFSGFIS:") == 1) {
    if (modem.waitResponse(5000UL, "OK") != 1) {
      logger.println("Write" + (String)file + "file failed!");
      return;
    } else
      logger.println("");
  } else {
    logger.println("CFSGFIS");
    return;
  }

  modem.sendAT("+CFSGFRS?");
  if (modem.waitResponse(5000UL, "+CFSGFRS:") == 1) {
    if (modem.waitResponse(5000UL, "OK") != 1) {
      logger.println("Write" + (String)file + "file failed!");
      return;
    } else
      logger.println("");
  } else {
    logger.println("CFSGFRS ERROR");
    return;
  }

  modem.sendAT("+CFSTERM");
  if (modem.waitResponse(10000UL, "OK") != 1) {
    logger.println("CFSTERM");
    return;
  } else
    logger.println("");
}

char *Read_FS(const char *file) {

  modem.sendAT("+CFSINIT");
  if (modem.waitResponse(10000UL, "OK") != 1) {
    logger.println("CFSINIT");
    return nullptr;
  } else
    logger.println("");

  int len = 0;
  /*
  logger.println("---------------------------------------------------");

  String test2;
  modem.sendAT("+CFSGFIS=0,\"", file, "\"");
  if (modem.waitResponse(10000UL,test2) == 1) {
    logger.println(test2);

  remove_str(test2, ' ');
  remove_str(test2, 'O');
  remove_str(test2, 'K');
  remove_str(test2, '\n');
    //TODO check delete test
    //if (modem.waitResponse(5000UL, "OK") != 1) {
    //  logger.println("Read " + (String)file + " file failed!");
    //  return nullptr;
    //} else logger.println("");
  } else {
    logger.println("CFSGFIS");
    return nullptr;
  }
  logger.println("-------------------------->" + test2);*/

  logger.println("---------------------------------------------------");

  char *test;
  modem.sendAT("+CFSGFIS=0,\"", file, "\"");
  if (modem.waitResponse(10000UL, "CFSGFIS:") == 1) {
    test = getRawData();
    // TODO check delete test
    // if (modem.waitResponse(5000UL, "OK") != 1) {
    //   logger.println("Read " + (String)file + " file failed!");
    //   return nullptr;
    // } else logger.println("");
  } else {
    logger.println("CFSGFIS");
    return nullptr;
  }
  logger.println("-------------------------->" + (String)test);
  len = atoi(test);
  delete[] test;

  logger.println("---------------------------------------------------");
  delay(10000);

  char *dataFS;
  modem.sendAT("+CFSRFILE=0,\"", file, "\",0,", len, ",", 10000UL);
  if (modem.waitResponse(10000UL, "+CFSRFILE:") == 1) {
    dataFS = getData('\n');
  } else {
    logger.println("CFSRFILE");
    return nullptr;
  }

  modem.sendAT("+CFSTERM");
  if (modem.waitResponse(10000UL, "OK") != 1) {
    logger.println("CFSTERM");
    return nullptr;
  } else
    logger.println("");

  return dataFS;
}

void FS_testing(const char *file, const char *content) {

  Write_FS(file, content);

  // READ THE FILE

  char *dataFS = Read_FS(file);
  logger.println("Datad in FS:" + (String)dataFS);

  modem.sendAT("+CFSINIT");
  if (modem.waitResponse(10000UL, "OK") != 1) {
    logger.println("CFSINIT");
    return;
  } else
    logger.println("");

  modem.sendAT("+CFSGFRS?");
  if (modem.waitResponse(5000UL, "+CFSGFRS:") == 1) {
    if (modem.waitResponse(5000UL, "OK") != 1) {
      logger.println("Write" + (String)file + "file failed!");
      return;
    } else
      logger.println("");
  } else {
    logger.println("CFSGFRS ERROR");
    return;
  }

  modem.sendAT("+CFSDFILE=0,\"", file, "\"");
  if (modem.waitResponse(10000UL, "OK") != 1) {
    logger.println("CFSDFILE");
    return;
  } else
    logger.println("");

  modem.sendAT("+CFSGFRS?");
  if (modem.waitResponse(5000UL, "+CFSGFRS:") == 1) {
    if (modem.waitResponse(5000UL, "OK") != 1) {
      logger.println("Write" + (String)file + "file failed!");
      return;
    } else
      logger.println("");
  } else {
    logger.println("CFSGFRS ERROR");
    return;
  }

  modem.sendAT("+CFSGFIS=0,\"", file, "\"");
  if (modem.waitResponse(10000UL, "CFSGFIS:") == 1) {
    logger.println("CFSGFIS 2");
    return;
  } else
    logger.println("");

  modem.sendAT("+CFSTERM");
  if (modem.waitResponse(10000UL, "OK") != 1) {
    logger.println("CFSTERM");
    return;
  } else
    logger.println("");

  // modem.sendAT("+CFSRFILE=3,\"", file, "\",0,", strlen(content), ",", 10000UL);
  // if (modem.waitResponse(10000UL, "OK")) {
  //   logger.println("CFSRFILE");
  //   return;
  // }
}

void GetHTTPData(const char *Server, const int Port, const char *Resource) {
  TinyGsmClient client(modem, 0);

  logger.printf("Connecting to %s", Server);
  if (!client.connect(Server, Port)) {
    logger.println("... failed");
  } else {
    // Make a HTTP GET request:
    client.print(String("GET ") + Resource + " HTTP/1.1\r\n");
    client.print(String("Host: ") + Server + "\r\n");
    client.print("Connection: close\r\n\r\n");

    // Wait for data to arrive
    uint32_t start = millis();
    while (client.connected() && !client.available() && millis() - start < 30000L) {
      delay(100);
    };

    // Read data
    start = millis();
    char logo[640] = {
      '\0',
    };
    int read_chars = 0;
    while (client.connected() && millis() - start < 10000L) {
      while (client.available()) {
        logo[read_chars] = client.read();
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
    delay(5000);
  }
}

void GetHTTPSData(const char *Server, const int Port, const char *Resource, const char *cert_ca) {
  TinyGsmClientSecure secureClient(modem, 0);
  logger.printf("Connecting securely to %s", Server);
  logger.println("");
  delay(5000);

  if (cert_ca != "") {
    logger.println("CERTIFICAT");

    modem.sendAT("+CASSLCFG?");
    modem.waitResponse();
    // secureClient.setCertificate((String)cert_ca);
    modem.sendAT("+CASSLCFG?");
    modem.waitResponse();
    logger.println("END CERTIFICAT");
  }

  if (!secureClient.connect(Server, Port)) {
    logger.println("... failed");
  } else {
    // Make a HTTPS GET request:
    secureClient.print(String("GET ") + Resource + " HTTP/1.1\r\n");
    secureClient.print(String("Host: ") + Server + "\r\n");
    secureClient.print("Connection: close\r\n\r\n");

    // Wait for data to arrive
    uint32_t startS = millis();
    while (secureClient.connected() && !secureClient.available() && millis() - startS < 30000L) {
      delay(100);
    };

    // Read data
    startS = millis();
    char logoS[640] = {
      '\0',
    };
    int read_charsS = 0;
    logger.println("Connecting");
    while (secureClient.connected() && millis() - startS < 10000L) {
      while (secureClient.available()) {
        logoS[read_charsS] = secureClient.read();
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
    delay(5000);
  }
}

bool sendSMSImpl(const String &number, const String &text) {
  // Set preferred message format to text mode
  modem.sendAT(GF("+CMGF=1"));
  modem.waitResponse();
  // Set GSM 7 bit default alphabet (3GPP TS 23.038)
  modem.sendAT(GF("+CSCS=\"GSM\""));
  modem.waitResponse();
  modem.sendAT(GF("+CMGS=\""), number, GF("\""));
  if (modem.waitResponse(GF(">")) != 1) {
    return false;
  }
  modem.stream.print(text);                     // Actually send the message
  modem.stream.write(static_cast<char>(0x1A));  // Terminate the message
  modem.stream.flush();
  return modem.waitResponse(60000L) == 1;
}

void sendSMS(const char *numtel, const char *message) {

  size_t totalLength = strlen("AT+CMGS=\"") + strlen(numtel) + strlen("\"") + 1;  // +1 for the null terminator

  char *result = new char[totalLength];
  modem.sendAT("+CSQ");
  if (modem.waitResponse(10000L) != 1) {
    logger.println("CSQ");
    return;
  }
  // SerialAT.write("AT+CSQ");

  modem.sendAT("+CMGF=1");
  // SerialAT.write("AT+CMGF=1");
  if (modem.waitResponse(10000L) != 1) {
    logger.println("CMGF");
    return;
  }
  modem.sendAT("+CSCS=\"GSM\"");
  // SerialAT.write("AT+CSCS=\"GSM\"");
  if (modem.waitResponse(10000L) != 1) {
    logger.println("CSCS");
    return;
  }

  strcpy(result, "+CMGS=\"");  // Copy the first string into the result buffer

  strcat(result, numtel);

  strcat(result, "\"");

  modem.sendAT(result);
  // SerialAT.write(result);
  if (modem.waitResponse(GF(">")) != 1) {
    return;
  }

  //  int response = modem.waitResponse(10000L);
  //  logger.println("------>"+(String)response);
  //    if (modem.waitResponse(10000L) != 1) {
  //    logger.println("CMGS");
  //    return;
  //  }
  modem.stream.print(message);
  modem.stream.write(static_cast<char>(0x1A));
  modem.stream.flush();
  // SerialAT.print(message);
  // SerialAT.write(static_cast<char>(0x1A));
  // SerialAT.flush();

  if (modem.waitResponse(10000L) != 1) {
    logger.println("MESSAGE SMS");
    return;
  }
}

#include <iostream>
#include <string>

void remove(std::string &s, char rem) {
  auto temp = s.begin();

  for (auto cpy = s.begin(); cpy != s.end(); ++cpy) {
    if (*cpy != rem) {
      *temp++ = *cpy;
    }
  }
  s.erase(temp, s.end());
}

void remove_str(String &str, char rem) {
  String result = "";
  for (unsigned int i = 0; i < str.length(); i++) {
    if (str[i] != rem) {
      result += str[i];
    }
  }
  str = result;
}

void remove(char *s, char rem) {
  char *cpy = s;  // an alias to iterate through s without moving s
  char *temp = s;

  while (*cpy) {
    if (*cpy != rem)
      *temp++ = *cpy;
    cpy++;
  }
  *temp = 0;

  // cout << s << endl; // This prints out the desired result: abbcccd
}

char *getRawData() {

  uint32_t start = millis();
  while (!modem.stream.available() && millis() - start < 30000L) {
    delay(100);
  };

  // Read data

  char *logo = new char[64];
  int bufferSize = 64;
  int read_chars = 0;

  start = millis();
  while (millis() - start < 10000L) {
    while (modem.stream.available()) {
      // logger.println(read_chars);
      if (read_chars >= bufferSize - 1) {
        bufferSize *= 2;  // Double the buffer size
        char *temp = new char[bufferSize];
        strcpy(temp, logo);  // Copy existing data to new buffer
        delete[] logo;       // Free old buffer
        logo = temp;         // Update pointer to new buffer
      }
      logo[read_chars++] = modem.stream.read();
      logo[read_chars] = '\0';

      start = millis();
    }
  }
  logger.println("============================================");
  logger.println(logo);
  logger.println("============================================");

  remove(logo, ' ');
  remove(logo, 'O');
  remove(logo, 'K');
  remove(logo, '\n');
  // logo = std::remove(logo, logo + strlen(logo), 'O');
  logger.println(logo);

  // logo = std::remove(logo, logo + strlen(logo), 'K');
  // logger.println(logo);
  //  Remove spaces
  // logo = std::remove(logo, logo + strlen(logo), ' ');

  logger.println("============================================");
  logger.println(logo);
  logger.println("============================================");
  return logo;
}
/*
char *getData(char sep) {

  uint32_t start = millis();
  while (!modem.stream.available() && millis() - start < 30000L) {
    delay(100);
  };

  // Read data

  char *logo = new char[64];
  int bufferSize = 64;
  int read_chars = 0;

  start = millis();
  while (millis() - start < 10000L) {
    while (modem.stream.available()) {
      // logger.println(read_chars);
      if (read_chars >= bufferSize - 1) {
        bufferSize *= 2;  // Double the buffer size
        char *temp = new char[bufferSize];
        strcpy(temp, logo);  // Copy existing data to new buffer
        delete[] logo;       // Free old buffer
        logo = temp;         // Update pointer to new buffer
      }
      logo[read_chars++] = modem.stream.read();
      logo[read_chars] = '\0';

      start = millis();
    }
  }
  // logger.println("#####  RECEIVED");
  // logger.println(logo);
  // logger.print("#####  RECEIVED METADATA: ");
  // logger.println(strlen(logo));

  // splitAtComma(logo);

  const char *st = logo;  // Start of the current substring
  // logger.println("start :" + (String)st);
  const char *end;  // End of the current substring
  end = strchr(st, sep);
  // logger.println("end :" + (String)end);
  size_t length = end - st;
  // logger.println("length :" + (String)length);
  char *substring = new char[length + 1];
  strncpy(substring, st, length);
  substring[length] = '\0';  // Null-terminate the substring
  // logger.println("     ----     " + (String)substring);

  st = end + 1;
  length = atoi(substring);
  delete[] substring;
  // logger.println("start 2 :" + (String)st);
  end = st + length;

  // end = strchr(st, '}');
  // logger.println("end 2 :" + (String)end);
  // length = end - st + 1;
  // logger.println("length :" + (String)length);
  char *substring2 = new char[length + 1];
  strncpy(substring2, st, length);
  substring2[length] = '\0';  // Null-terminate the substring
  // Print or process the substring
  // logger.println("     ----     " + (String)substring2);

  delay(5000);
  delete[] logo;

  return substring2;
}
*/
char *sendData(const char *data) {

  modem.sendAT("+CAOPEN?");

  if (modem.waitResponse(10000L) != 1) {
    logger.println("CAOPEN");
    return "error";
  }

  logger.println("TEST CLIENT 1   ");
  TinyGsmClient clientUDP(modem, 0);
  const int REMOTE_UDP_PORT = 60004;
  const IPAddress REMOTE_UDP_ADDR = IPAddress(18, 184, 137, 24);
  logger.println(REMOTE_UDP_ADDR);
  logger.println(REMOTE_UDP_PORT);

  // if (!clientUDP.connect(REMOTE_UDP_ADDR, REMOTE_UDP_PORT)) {

  modem.sendAT("+CAOPEN=0,0,\"UDP\",\"18.184.137.24\",60004");  // AT+CAOPEN=0,0,"UDP","18.184.137.24",60004
  if (modem.waitResponse(10000L) != 1) {
    logger.println("ERROR CAOPEN=0,0,\"UDP\",\"18.184.137.24\",60004");
    return "error";
  }

  modem.sendAT("+CASEND=0," + (String)strlen(data));  //?? is it the length odf data or data + 1 because of the 0x1A
  // modem.sendAT("+CASEND=0,10");//ERROR with 0

  if (modem.waitResponse(GF(">")) != 1) {
    return "error";
  }

  // modem.stream.write(result);
  // modem.stream.print(result);
  // SerialAT.write(data);
  // SerialAT.write(0x1A);
  modem.stream.print(data);
  modem.stream.write(static_cast<char>(0x1A));
  modem.stream.flush();
  // modem.stream.write("T3\r\n");
  // modem.stream.flush();

  int Sendresponse = modem.waitResponse();
  logger.println(Sendresponse);
  if (Sendresponse != 1) {
    logger.println("ERROR CASEND " + (String)Sendresponse);
    return "error";
  }

  logger.println("CHECK IF THERE IS RESPONSE");
  if (modem.waitResponse(GF("+CADATAIND:")) != 1) {
    return "error";
  }

  logger.println("answer: ");
  char *buffer2[100];
  modem.sendAT("+CARECV=0,265");
  if (modem.waitResponse(10000L, "+CARECV:") != 1) {
    logger.println("ERROR CARECV");
    return "error";
  }

  char *substring2 = getData(',');

  //
  //  modem.sendAT("+CARECV=0,265");
  //  if (modem.waitResponse(10000L) != 1) {
  //    logger.println("ERROR CARECV");
  //    return;
  //  }

  modem.sendAT("+CAACK=0");
  if (modem.waitResponse(10000L) != 1) {
    logger.println("ERROR CAACK");
    return "error";
  }

  modem.sendAT("+CACLOSE=0");  // AT+CACLOSE=0
  return substring2;
}

bool getGSMLOC() {
  logger.println("Get Location GSM");
  modem.sendAT("+CLBS=4,0");
  delay(30000);
  int Response2 = modem.waitResponse(120000L, GF("+CLBS: "));
  if (Response2 != 1) {
    logger.println("ERROR GPS");
    logger.println(Response2);
  }
  //  SerialAT.write("AT+CLBS=4,0");
  //  Response2 = modem.waitResponse(120000L, GF("+CLBS: "));
  // if (Response2 != 1) {
  //    logger.println("ERROR GPS 2");
  //    logger.println(Response2);

  //    }
  /*  int8_t locationCode = modem.streamGetIntLength(2);
    // 0 = success, else, error
    if (locationCode != 0) {
      modem.waitResponse();  // should be an ok after the error
      return false;
    }

    // init variables
    float ilat      = 0;
    float ilon      = 0;
    float iaccuracy = 0;
    int   iyear     = 0;
    int   imonth    = 0;
    int   iday      = 0;
    int   ihour     = 0;
    int   imin      = 0;
    int   isec      = 0;

    ilat      = modem.streamGetFloatBefore(',');  // Latitude
    ilon      = modem.streamGetFloatBefore(',');  // Longitude
    iaccuracy = modem.streamGetIntBefore(',');    // Positioning accuracy

    // Date & Time
    iyear  = modem.streamGetIntBefore('/');
    imonth = modem.streamGetIntBefore('/');
    iday   = modem.streamGetIntBefore(',');
    ihour  = modem.streamGetIntBefore(':');
    imin   = modem.streamGetIntBefore(':');
    isec   = modem.streamGetIntBefore('\n');

    logger.println((String)ilat+","+(String)ilong+","+iaccuracy+","+iday+"/"+imonth+"/"+iyear+","+ihour+":"+imin+":"+isec);

    // Set pointers
    //if (lat != NULL) *lat = ilat;
    //if (lon != NULL) *lon = ilon;
    //if (accuracy != NULL) *accuracy = iaccuracy;
    //if (iyear < 2000) iyear += 2000;
    //if (year != NULL) *year = iyear;
    //if (month != NULL) *month = imonth;
    //if (day != NULL) *day = iday;
    //if (hour != NULL) *hour = ihour;
    //if (minute != NULL) *minute = imin;
    //if (second != NULL) *second = isec;

    // Final OK
    thisModem().waitResponse();
    return true;
  }*/
}

void splitAtComma(const char *input) {
  const char *start = input;  // Start of the current substring
  const char *end;            // End of the current substring

  while ((end = strchr(start, ',')) != nullptr) {
    // Calculate the length of the substring
    size_t length = end - start;

    // Allocate memory for the substring
    char *substring = new char[length + 1];

    // Copy the substring into the new memory
    strncpy(substring, start, length);
    substring[length] = '\0';  // Null-terminate the substring

    // Print or process the substring
    logger.println(substring);
    // std::cout << substring << std::endl;

    // Clean up the allocated memory
    delete[] substring;

    // Move the start pointer past the comma
    start = end + 1;
  }

  // Handle the last substring (or the entire string if no comma is found)
  if (*start != '\0') {
    logger.println(start);
    // std::cout << start << std::endl; // Print the last substring
  }
}

// Calculate the total length needed for the concatenated string
