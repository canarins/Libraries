/**
 * @file      MinimalModemNBIOTExample.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2022  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2022-09-16
 *
 */
#ifndef CANARIN_GSM_H_
#define CANARIN_GSM_H_

#include <Canarin_Utilities.h>
//#include <Canarin_Logger.h>
#include <Canarin_PMU.h>

#define TINY_GSM_MODEM_SIM7080
#include "SPIFFS.h"
char buffer[1024] = {0};

// XPowersPMU PMU;

// See all AT commands, if wanted

#define TINY_GSM_RX_BUFFER 1024
#include <TinyGsmClient.h>

#define SerialAT Serial1

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
// Server details to test TCP/SSL
// const char serverexample[] = "vsh.pp.ua";
// const char resourceexample[] = "/TinyGSM/logo.txt";

// #ifdef DUMP_AT_COMMANDS
// #include <StreamDebugger.h>
// StreamDebugger debugger(Serial1, Serial);
// TinyGsm        modem(debugger);
// #else
// TinyGsm        modem(SerialAT);
// #endif

const char *register_info[] = {
    "Not registered, MT is not currently searching an operator to register to.The GPRS service is disabled, the UE is allowed to attach for GPRS if requested by the user.",
    "Registered, home network.",
    "Not registered, but MT is currently trying to attach or searching an operator to register to. The GPRS service is enabled, but an allowable PLMN is currently not available. The UE will start a GPRS attach as soon as an allowable PLMN is available.",
    "Registration denied, The GPRS service is disabled, the UE is not allowed to attach for GPRS if it is requested by the user.",
    "Unknown.",
    "Registered, roaming.",
};

enum
{
  MODEM_CATM = 1,
  MODEM_NB_IOT,
  MODEM_CATM_NBIOT,
};

/* MODEM_CATM = 1: Represents a Cat-M (LTE-M or eMTC) modem, which is a type of low-power wide-area network (LPWAN) designed for Internet of Things (IoT) devices. The constant value is set to 1.
MODEM_NB_IOT: Represents an NB-IoT (Narrowband IoT) modem, another type of LPWAN that is designed for IoT devices. The constant value is implicitly set to 2, as it follows MODEM_CATM.
MODEM_CATM_NBIOT: Represents a modem that supports both Cat-M and NB-IoT connectivity. The constant value is implicitly set to 3, as it follows MODEM_NB_IOT. */

void getPsmTimer();

// const char apn[] = "ibasis.iot";
const char gprsUser[] = "";
const char gprsPass[] = "";
// bool  level = false;

// Server details to test TCP/SSL
// const char server[]   = "vsh.pp.ua";
// const char resource[] = "/TinyGSM/logo.txt";

//!! Set the APN manually. Some operators need to set APN first when registering the network.
//!! Set the APN manually. Some operators need to set APN first when registering the network.
//!! Set the APN manually. Some operators need to set APN first when registering the network.
// Using 7080G with Hologram.io , https://github.com/Xinyuan-LilyGO/LilyGo-T-SIM7080G/issues/19
// const char *apn = "hologram";

const char *apn = "TM";

bool gsm_setup = false;


void Canarin_GSM_setup(){
if (gsm_setup)return;

  logger.println("start to initialize T-SIM7080G modem now ");
  SerialAT.begin(115200, SERIAL_8N1, BOARD_MODEM_RXD_PIN, BOARD_MODEM_TXD_PIN);

  pinMode(BOARD_MODEM_PWR_PIN, OUTPUT);
  pinMode(BOARD_MODEM_DTR_PIN, OUTPUT);
  pinMode(BOARD_MODEM_RI_PIN, INPUT);

  int retry = 0;
  while (!modem.testAT(1000))
  {
    logger.print(".");
    if (retry++ > 6)
    {
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
  logger.println("T-SIM7080G modem well started!");
gsm_setup = true;
}




int GSM_connect(unsigned long connectTimeout = 5000)
{
  /***********************************
   * step 3 : Check if the SIM card is inserted
   ***********************************/
  logger.print("SIM_READY: ");
  logger.println(SIM_READY);

  logger.print("REG_OK_HOME: ");
  logger.println(REG_OK_HOME);

  logger.print("REG_OK_ROAMING: ");
  logger.println(REG_OK_ROAMING);

  logger.print("Current SIM status: ");
  logger.println(modem.getSimStatus());//TODO see when sim is 0 instead 1 while is 0 

  String result;
  while (modem.getSimStatus() != SIM_READY)
  {
    logger.println("ERROR SIM Card is not insert!!!");
    
    ESP.restart();
    //return 0;
  }
  logger.println("SIM Card is insert !");

  // Disable RF
  modem.sendAT("+CFUN=0");
  if (modem.waitResponse(20000UL) != 1)
  {
    logger.println("ERROR Disable RF Failed!");
    return 0;
  }

  /*********************************
   * step 4 : Set the network mode to NB-IOT
   ***********************************/
  //logger.println("test the network mode ");
  //logger.println(modem.getNetworkModes());
  //logger.println(modem.getPreferredModes());
  delay(1000);

  logger.println("Set the network mode to NB-IOT");

  modem.setNetworkMode(2); // use automatic

  modem.setPreferredMode(3);

  uint8_t pre = modem.getPreferredMode();

  uint8_t mode = modem.getNetworkMode();

  logger.printf("getNetworkMode:%u getPreferredMode:%u\n", mode, pre);

  // modem.sendAT("+COPS=0,0,\"AT&T\",9");
  // modem.waitResponse();
  logger.print("CCID:");
  logger.println(modem.getSimCCID());


  modem.sendAT("+CBANDCFG?");
  modem.waitResponse();

  modem.sendAT("+CBANDCFG=\"NB-IoT\",12"); // 4: 700MHz for T-mobile, 12: 850MHz for AT&T
  modem.waitResponse();

  modem.sendAT("+CNBS=1");
  // Set the APN manually. Some operators need to set APN first when registering the network.
  modem.sendAT("+CGDCONT=1,\"IP\",\"", apn, "\"");
  if (modem.waitResponse() != 1)
  {
    logger.println("Set operators apn Failed!");
    return 0;
  }

  //!! Set the APN manually. Some operators need to set APN first when registering the network.
  modem.sendAT("+CNCFG=0,1,\"", apn, "\"");
  if (modem.waitResponse() != 1)
  {
    logger.println("Config apn Failed!");
    return 0;
  }

  // Enable RF
  modem.sendAT("+CFUN=1");
  if (modem.waitResponse(20000UL) != 1)
  {
    logger.println("Enable RF Failed!");
    return 0;
  }

  /*********************************
   * step 5 : Wait for the network registration to succeed
   ***********************************/
  logger.println("Wait for the network registration to succeed");
  RegStatus s;
  do
  {
    s = modem.getRegistrationStatus();
    //logger.print("=============>");
    //logger.println(s);
    if (s != REG_OK_HOME && s != REG_OK_ROAMING)
    {
      logger.print(".");
      PMU.setChargingLedMode(level ? XPOWERS_CHG_LED_ON : XPOWERS_CHG_LED_OFF);
      // TODO move the led management to the exterior function
      level ^= 1;
      delay(1000);
    }

  } while (s != REG_OK_HOME && s != REG_OK_ROAMING);

  logger.println();
  logger.println("Network register info:");
  logger.println(register_info[s]);

  // Activate network bearer, APN can not be configured by default,
  // if the SIM card is locked, please configure the correct APN and user password, use the gprsConnect() method
  modem.sendAT("+CNACT=0,1");
  if (modem.waitResponse() != 1)
  {
    logger.println("Activate network bearer Failed!");
    return 0;
  }
  modem.sendAT("+CGNAPN");
  if (modem.waitResponse(10000L) != 1)
  {
    logger.println("Get APN Failed!");
    return 0;
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

  char iccid[32] = "";
  strncpy(iccid, ccid.c_str(), sizeof(iccid));
  logger.println(iccid);

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

  return 1;
}

char *GSM_send(IPAddress server_name, const int port, char *request_data)
{
  /*********************************
   * step 6 : Send HTTP request
   ***********************************/
  logger.println("Send HTTP request");

  TinyGsmClient client(modem, 0);

  logger.printf("Connecting to", server_name);
  if (!client.connect(server_name, port))
  {
    logger.println("... failed");
  }
  else
  {
    // Make a HTTP GET request:
    client.print(String("GET ") + request_data + " HTTP/1.0\r\n");
    client.print(String("Host: ") + server_name + "\r\n");
    client.print("Connection: close\r\n\r\n");

    // Wait for data to arrive
    uint32_t start = millis();
    while (client.connected() && !client.available() && millis() - start < 30000L)
    {
      delay(100);
    };

    // Read data
    start = millis();
    char response_gsm_temp[640] = {
        '\0',
    };
    int read_chars = 0;
    while (client.connected() && millis() - start < 10000L)
    {
      while (client.available())
      {
        response_gsm_temp[read_chars] = client.read();

        read_chars++;
        start = millis();
      }
      response_gsm_temp[read_chars] = '\0';
    }
    //logger.print("#####  RECEIVED: ");
    //logger.print(strlen(response_gsm_temp));
    //logger.println("CHARACTERS");
    client.stop();
    return response_gsm_temp;
  }
  client.stop();
  return nullptr;
}



char *ExtractDataFromResponse(char *response_temp_, char sep)
{

  const char *st = response_temp_; // Start of the current substring
  //logger.print("start :");
  //logger.println(st);
  const char *end = strchr(st, sep) + 1;

  if (!end)
  {
    // Handle error; separator not found.
    return nullptr;
  }

  //logger.print("end :");
  //logger.println(end);
  size_t length = end - st - 1;
  //logger.print("length : ");
  //logger.println(length);

  // Parse the length of the next segment.
  char *length_str = new char[length + 1];
  strncpy(length_str, st, length);
  length_str[length] = '\0'; // Null-terminate the length string

  //logger.print("length_str :");
  //logger.println(length_str);

  int next_length = atoi(length_str); // Convert extracted length to an integer
  delete[] length_str;                // Deallocate the temporary buffer for length string
  //logger.print("#####  next_length: ");
  //logger.println(next_length);
  // Safeguard against potential out-of-bounds access

  // Allocate memory for the extracted data segment
  char *data_segment = new char[next_length + 1];
  strncpy(data_segment, end, next_length);
  data_segment[next_length] = '\0'; // Null-terminate the data segment

  // No need to deallocate response_temp_ here if it's being used elsewhere
  //logger.print("#####  data_segment: ");
  //logger.println(data_segment);
  return data_segment;
}

char *getData(char sep)
{
  logger.println("getData");

  uint32_t start = millis();
  while (!modem.stream.available() && millis() - start < 30000L)
  {
    delay(100);
  };

  // Read data

  char *response_GSM_temp = new char[64];
  int bufferSize = 64;
  int read_chars = 0;

  start = millis();
  while (millis() - start < 10000L)
  {
    while (modem.stream.available())
    {
      // logger.println(read_chars);
      if (read_chars >= bufferSize - 1)
      {
        bufferSize *= 2; // Double the buffer size
        char *temp = new char[bufferSize];
        strcpy(temp, response_GSM_temp); // Copy existing data to new buffer
        delete[] response_GSM_temp;      // Free old buffer
        response_GSM_temp = temp;        // Update pointer to new buffer
      }
      response_GSM_temp[read_chars++] = modem.stream.read();
      response_GSM_temp[read_chars] = '\0';

      start = millis();
    }
  }
  logger.print("#####  RECEIVED: ");
  logger.println(response_GSM_temp);
  logger.print("#####  RECEIVED METADATA: ");
  logger.println(strlen(response_GSM_temp));

  return ExtractDataFromResponse(response_GSM_temp, ',');
}

// Define a function to send UDP packets using GSM
char *GSMUDP_send(IPAddress udpAddress, const int udpport, char *request_data)
{
  // Log the function call for debugging purposes
  logger.println("GSMUDP_send");
  // Log the IP address and port being sent to
  logger.println(udpAddress);
  logger.println(udpport);

  // Check if a UDP connection is already open
  modem.sendAT("+CAOPEN?");
  String response;
  int8_t ret = modem.waitResponse(10000L, response);

  // If there is no active CAOPEN session opened for the specified UDP address and port,
  // this means we are not connected
  if (response.indexOf((String) "+CAOPEN: 0,0,UDP,\"" + udpAddress.toString() + "\"," + udpport) == -1)
  {
    // Inform that we are not connected to the UDP service
    logger.println("NOT CONNECTED to Canarin.net");

    if (response.indexOf((String) "CAOPEN") != -1)
    {
      logger.println("CAOPEN IS already connect");
      modem.sendAT("+CACLOSE=0"); // AT+CACLOSE=0
      if (modem.waitResponse(10000L) != 1)
      {
        logger.println("ERROR CACLOSE");
        return nullptr;
      }
    }
    modem.sendAT("+CAOPEN=0,0,\"UDP\",\"" + udpAddress.toString() + "\"," + udpport); // AT+CAOPEN=0,0,"UDP","18.184.137.24",60004
    // modem.sendAT("+CAOPEN=0,0,\"UDP\",\"" + String(udpAddress) + "\"," + String(udpport));
    if (modem.waitResponse(10000L) != 1)
    {
      logger.println("ERROR" + String("+CAOPEN: 0,0,UDP,\"") + udpAddress.toString() + "\"," + udpport);

      return nullptr;
    }
  }
  else
  {
    logger.println("CONNECTED already to Canarin.net");
  }

  modem.sendAT("+CASEND=0," + (String)strlen(request_data)); //?? is it the length odf data or data + 1 because of the 0x1A
  // modem.sendAT("+CASEND=0,10");//ERROR with 0

  if (modem.waitResponse(GF(">")) != 1)
  {
    return nullptr;
  }

  modem.stream.print(request_data);
  modem.stream.write(static_cast<char>(0x1A));
  modem.stream.flush();

  if (modem.waitResponse() != 1)
  {
    logger.print("ERROR CASEND ");
    return nullptr;
  }

  logger.println("CHECK IF THERE IS RESPONSE");
  if (modem.waitResponse(GF("+CADATAIND:")) != 1)
  {
    logger.println("No response to CADATAIND");
    return nullptr;
  }

  logger.println("answer: ");
  char *buffer2[100];
  modem.sendAT("+CARECV=0,265");
  if (modem.waitResponse(10000L, "+CARECV:") != 1)
  {
    logger.println("ERROR CARECV");
    return nullptr;
  }
#ifdef USB_CDC_ENABLED
  char *substring2 = getData(',');
  logger.print("output GSM_UDP_RESPONSE");
  logger.println(substring2);

  return substring2;
#endif
  return getData(',');
}

void printPercent(uint32_t readLength, uint32_t contentLength)
{
  // If we know the total length
  if (contentLength != -1)
  {
    // logger.print("\r ");
    logger.print((100.0 * readLength) / contentLength);
    logger.println('%');
  }
  else
  {
    logger.println(readLength);
  }
}

#define TINY_GSM_RX_BUFFER 4096 // 1030 d'origine

char *GSMSecure_Send(const char *server_name, const int port, const char *request_data)
{

  logger.print("Send HTTPS request to: ");
  logger.print(server_name);
  logger.print(" ");
  logger.println(port);
  logger.print(" ");
  logger.println(request_data);

  TinyGsmClientSecure Secureclient(modem);

  // if you get a connection, report back via serial:
  if (!Secureclient.connect(server_name, port))
  {
    logger.println(" fail");
    delay(10000);
    return nullptr;
  }
  else
  {
    logger.println(" OK");
    // client.connectionKeepAlive();  // Currently, this is needed for HTTPS
    //  Make a HTTP request:
    Secureclient.print(String("GET ") + request_data + " HTTP/1.0\r\n");
    Secureclient.print(String("Host: ") + server_name + "\r\n");
    Secureclient.print("Connection: close\r\n\r\n");
    logger.println(" sent request");
    long timeout = millis();
    while (Secureclient.available() == 0)
    {
      if (millis() - timeout > 5000L)
      {
        logger.println(">>> Secureclient Timeout !");
        Secureclient.stop();
        delay(10000L);
        return nullptr;
      }
    }

    logger.println(" get response");
    uint32_t start = millis();
    const int initialBufferSize = 640;
    // int bufferSize = initialBufferSize;

    logger.println(" test");

    uint32_t contentLength = 0;

    while (Secureclient.available())
    {
      // logger.print(" test2  "+(String)read_chars);
      String line = Secureclient.readStringUntil('\n');
      line.trim();
      line.toLowerCase();
      if (line.startsWith("content-length:"))
      {
        contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
      }
      else if (line.length() == 0)
      {
        break;
      }
    }

    timeout = millis();
    uint32_t readLength = 0;
    // CRC32 crc;

    logger.println("starting read");
    logger.print("contentLength:");
    logger.println(contentLength);

    printPercent(readLength, contentLength);

    logger.print("esp_get_free_heap_size: ");
    logger.println(esp_get_free_heap_size());
    logger.print("heap_caps_get_free_size(MALLOC_CAP_8BIT): ");
    logger.println(heap_caps_get_free_size(MALLOC_CAP_8BIT));
    logger.print("configMINIMAL_STACK_SIZE: ");
    logger.println(configMINIMAL_STACK_SIZE);
    logger.print("TINY_GSM_RX_BUFFER: ");
    logger.println(TINY_GSM_RX_BUFFER);
    logger.println("testing affectation char ");
    //char *response_gsm_temp = new char[contentLength + 1]; // Allocate initial buffer
    logger.println("testing affectation char end");
    unsigned long timeElapsed = millis();
    File file = SPIFFS.open("/update.bin", FILE_APPEND);
    printPercent(readLength, contentLength);

    while (readLength < contentLength && Secureclient.connected() && millis() - timeout < 60000L)
    {
      while (Secureclient.available())
      {
        if (readLength % (contentLength / 13) == 0)
        {
          printPercent(readLength, contentLength);

          logger.print("esp_get_free_heap_size: ");
          logger.println(esp_get_free_heap_size());
          logger.print("heap_caps_get_free_size(MALLOC_CAP_8BIT): ");
          logger.println(heap_caps_get_free_size(MALLOC_CAP_8BIT));
          logger.print("configMINIMAL_STACK_SIZE: ");
          logger.println(configMINIMAL_STACK_SIZE);
          logger.print("TINY_GSM_RX_BUFFER: ");
          logger.println(TINY_GSM_RX_BUFFER);
          // free(buf);
        }
        start = millis();
        /*
                // Check if we need to increase the buffer size
                if (readLength >= bufferSize - 1)
                {
                  logger.println(readLength);
                  // Double the buffer size
                  int newBufferSize = bufferSize * 2;
                  char *newBuffer = new char[newBufferSize];
                  if (newBuffer == nullptr)
                  {
                    // Memory allocation failed, handle the error
                    logger.println("Memory allocation failed!");
                    // Clean up and return or handle the error appropriately
                    delete[] response_gsm_temp;
                    return nullptr; // Or any other error handling mechanism
                  }

                  // Copy contents of old buffer to new buffer
                  for (int i = 0; i < readLength; i++)
                  {
                    newBuffer[i] = response_gsm_temp[i];
                  }

                  // Free old buffer and update pointers
                  delete[] response_gsm_temp;
                  response_gsm_temp = newBuffer;
                  bufferSize = newBufferSize;
                }*/
        //response_gsm_temp[readLength] = Secureclient.read();
        if (!file.print(char(Secureclient.read())))
            {
                logger.println("Appending file");
            }
        // logger.println(response_gsm_temp[read_chars]);

        readLength++;
      }
      //response_gsm_temp[readLength] = '\0';
    }
    file.close();
    logger.println("END OF DOWNLOAD UPDATE FILE ");

    

    Secureclient.stop();
        logger.println("stop client");



    String res;
    logger.println("get the time: ");
    modem.sendAT("+CCLK?"); // get the time TODO look for time
    modem.waitResponse(1000L, res);
    logger.println(res);

    if (readLength != contentLength)
    {
        logger.print("readLength is different than contentLength :");
        logger.print(readLength);
        logger.print(" ");
        logger.println(contentLength);
        logger.println("Restart ESP device! in 3s");
        delay(3000);
        ESP.restart();
    }
    logger.print("Taille du fichier: ");
    logger.println(contentLength);
    logger.print("Lire:  ");
    logger.println(readLength);

    return nullptr;

    /*
        uint32_t start = millis();
        char response_gsm_temp[640] = {
            '\0',
        };
        int read_chars = 0;
         logger.println(" test");

        while (Secureclient.connected() && millis() - start < 10000L)
        {
               logger.println(" test1");

          while (Secureclient.available())
          {
                 logger.print(" test2  "+(String)read_chars);

            response_gsm_temp[read_chars] = Secureclient.read();
            logger.println(response_gsm_temp[read_chars]);

            read_chars++;
            start = millis();
          }
          response_gsm_temp[read_chars] = '\0';
        }
        logger.print("#####  RECEIVED:");
        logger.print(strlen(response_gsm_temp));
        logger.println("CHARACTERS");
        Secureclient.stop();
        return response_gsm_temp;*/
  }
}
#define TINY_GSM_RX_BUFFER 1024 // 1030 d'origine
void GSM_close()
{

  if (!modem.isGprsConnected())
    return;

  modem.sendAT("AT+CAACK=?");
  modem.waitResponse(10000L);

  modem.sendAT("+CAACK=0");
  if (modem.waitResponse(10000L) != 1)
  {
    logger.println("ERROR CAACK");
    // return;
  }

  modem.sendAT("AT+CACLOSE=?");
  modem.waitResponse(10000L);

  modem.sendAT("+CACLOSE=0"); // AT+CACLOSE=0
  if (modem.waitResponse(10000L) != 1)
  {
    logger.println("ERROR CACLOSE");
    return;
  }
  modem.gprsDisconnect();
  delay(5000L);
  if (!modem.isGprsConnected())
  {
    DBG("GPRS disconnected"); // TODO DBG -> logger.print
  }
  else
  {
    DBG("GPRS disconnect: Failed.");
  }
}

#endif