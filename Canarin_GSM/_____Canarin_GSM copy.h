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

#define TINY_GSM_MODEM_SIM7080

char buffer[1024] = {0};

// XPowersPMU PMU;

// See all AT commands, if wanted

#define TINY_GSM_RX_BUFFER 1024
#include <TinyGsmClient.h>

#define SerialAT SerialAT

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
// StreamDebugger debugger(SerialAT, Serial);
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

int GSM_connect()
{
  /***********************************
   * step 3 : Check if the SIM card is inserted
   ***********************************/
  logger.println("SIM_READY: " + (String)SIM_READY);
  logger.println("REG_OK_HOME: " + (String)REG_OK_HOME);
  logger.println("REG_OK_ROAMING: " + (String)REG_OK_ROAMING);

  logger.println("Current SIM status: " + (String)modem.getSimStatus());
  // logger.println("---------->"+(String)modem.simUnlock("0000"));
  // modem.simUnlock("0000");
  // logger.println("---------->"+(String)modem.getSimStatus());
  String result;
  if (modem.getSimStatus() != SIM_READY)
  {
    logger.println("SIM Card is not insert!!!");

    return 0;
  }
  logger.println("SIM Card is insert !");

  // Disable RF
  modem.sendAT("+CFUN=0");
  if (modem.waitResponse(20000UL) != 1)
  {
    logger.println("Disable RF Failed!");
  }

  /*********************************
   * step 4 : Set the network mode to NB-IOT
   ***********************************/
  logger.println("test the network mode ");
  logger.println(modem.getNetworkModes());
  logger.println(modem.getPreferredModes());
  delay(10000);

  logger.println("Set the network mode to NB-IOT");

  modem.setNetworkMode(2); // use automatic

  modem.setPreferredMode(3);

  uint8_t pre = modem.getPreferredMode();

  uint8_t mode = modem.getNetworkMode();

  logger.printf("getNetworkMode:%u getPreferredMode:%u\n", mode, pre);

  // modem.sendAT("+COPS=0,0,\"AT&T\",9");
  // modem.waitResponse();

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
  }

  /*********************************
   * step 5 : Wait for the network registration to succeed
   ***********************************/
  logger.println("Wait for the network registration to succeed");
  RegStatus s;
  do
  {
    s = modem.getRegistrationStatus();
    logger.print("=============>");
    logger.println(s);
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
    logger.print("#####  RECEIVED:");
    logger.print(strlen(response_gsm_temp));
    logger.println("CHARACTERS");
    client.stop();
    return response_gsm_temp;
  }
  client.stop();
  return "";
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

  char *response_temp_ = new char[64];
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
        strcpy(temp, response_temp_); // Copy existing data to new buffer
        delete[] response_temp_;      // Free old buffer
        response_temp_ = temp;        // Update pointer to new buffer
      }
      response_temp_[read_chars++] = modem.stream.read();
      response_temp_[read_chars] = '\0';

      start = millis();
    }
  }
  // logger.println("#####  RECEIVED");
  // logger.println(response_temp_);
  // logger.print("#####  RECEIVED METADATA: ");
  // logger.println(strlen(response_temp_));

  // splitAtComma(response_temp_);

  const char *st = response_temp_; // Start of the current substring
  // logger.println("start :" + (String)st);
  const char *end = strchr(st, sep);
  if (!end)
  {
    // Handle error; separator not found.
    return nullptr;
  }
  // logger.println("end :" + (String)end);
  size_t length = end - st;
  // logger.println("length :" + (String)length);
  // Parse the length of the next segment.
  char *length_str = new char[length + 1];
  strncpy(length_str, st, length);
  length_str[length] = '\0'; // Null-terminate the length string

  int next_length = atoi(length_str); // Convert extracted length to an integer
  delete[] length_str;                // Deallocate the temporary buffer for length string

  // Safeguard against potential out-of-bounds access
  const char *resp_end = response_temp_ + strlen(response_temp_);
  if (st + next_length > resp_end)
  {
    // Handle error; length extends beyond response_temp_'s bounds.
    return nullptr;
  }

  // Allocate memory for the extracted data segment
  char *data_segment = new char[next_length + 1];
  strncpy(data_segment, st, next_length);
  data_segment[next_length] = '\0'; // Null-terminate the data segment

  // No need to deallocate response_temp_ here if it's being used elsewhere

  return data_segment; // Note that the caller must delete[] this buffer later
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

  logger.println(+"@@@@@@@@@@@@@@@@@@@@@@@@" + (String)ret + " --  " + response + " -- " + response.indexOf((String) "+CAOPEN: 0,0,UDP,\"" + udpAddress.toString() + "\"," + udpport + ""));
  // logger.println(+"@@@@@@@@@@@@@@@@@@@@@@@@" + (String)ret + " --  " + response + " -- " + response.indexOf((String) "+CAOPEN: 0,0,UDP,\"18.184.137.24\",60004"));

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
        return "error";
      }
      // return "error";
    }
    modem.sendAT("+CAOPEN=0,0,\"UDP\",\"" + udpAddress.toString() + "\"," + udpport); // AT+CAOPEN=0,0,"UDP","18.184.137.24",60004
    // modem.sendAT("+CAOPEN=0,0,\"UDP\",\"" + String(udpAddress) + "\"," + String(udpport));
    if (modem.waitResponse(10000L) != 1)
    {
      logger.println("ERROR" + String("+CAOPEN: 0,0,UDP,\"") + udpAddress.toString() + "\"," + udpport);

      return "error";
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
    return "error";
  }

  modem.stream.print(request_data);
  modem.stream.write(static_cast<char>(0x1A));
  modem.stream.flush();

  if (modem.waitResponse() != 1)
  {
    logger.print("ERROR CASEND ");
    return "error";
  }

  logger.println("CHECK IF THERE IS RESPONSE");
  if (modem.waitResponse(GF("+CADATAIND:")) != 1)
  {
    return "error";
  }

  logger.println("answer: ");
  char *buffer2[100];
  modem.sendAT("+CARECV=0,265");
  if (modem.waitResponse(10000L, "+CARECV:") != 1)
  {
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

  return substring2;
}

void GSM_close()
{

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
    DBG("GPRS disconnected");
  }
  else
  {
    DBG("GPRS disconnect: Failed.");
  }
}

void GSM_exec()
{

  /*********************************
   * step 6 : Send HTTP request
   ***********************************/
  logger.println("Send HTTP request");

  TinyGsmClient client(modem, 0);
  const int port = 80;
  logger.printf("Connecting to", serverexample);
  if (!client.connect(serverexample, port))
  {
    logger.println("... failed");
  }
  else
  {
    // Make a HTTP GET request:
    client.print(String("GET ") + resourceexample + " HTTP/1.0\r\n");
    client.print(String("Host: ") + serverexample + "\r\n");
    client.print("Connection: close\r\n\r\n");

    // Wait for data to arrive
    uint32_t start = millis();
    while (client.connected() && !client.available() && millis() - start < 30000L)
    {
      delay(100);
    };

    // Read data
    start = millis();
    char response_temp_[640] = {
        '\0',
    };
    int read_chars = 0;
    while (client.connected() && millis() - start < 10000L)
    {
      while (client.available())
      {
        response_temp_[read_chars] = client.read();
        response_temp_[read_chars + 1] = '\0';
        read_chars++;
        start = millis();
      }
    }
    logger.println(response_temp_);
    logger.print("#####  RECEIVED:");
    logger.print(strlen(response_temp_));
    logger.println("CHARACTERS");
    client.stop();
  }

  /*********************************
   * step 6 : Send HTTPS request
   ***********************************/
  logger.println("Send HTTPS request");
  TinyGsmClientSecure secureClient(modem, 1);
  const int securePort = 443;
  logger.printf("Connecting securely to", serverexample);
  if (!secureClient.connect(serverexample, securePort))
  {
    logger.println("... failed");
  }
  else
  {
    // Make a HTTP GET request:
    secureClient.print(String("GET ") + resourceexample + " HTTP/1.0\r\n");
    secureClient.print(String("Host: ") + serverexample + "\r\n");
    secureClient.print("Connection: close\r\n\r\n");

    // Wait for data to arrive
    uint32_t startS = millis();
    while (secureClient.connected() && !secureClient.available() && millis() - startS < 30000L)
    {
      delay(100);
    };

    // Read data
    startS = millis();
    char response_temp_S[640] = {
        '\0',
    };
    int read_charsS = 0;
    while (secureClient.connected() && millis() - startS < 10000L)
    {
      while (secureClient.available())
      {
        response_temp_S[read_charsS] = secureClient.read();
        response_temp_S[read_charsS + 1] = '\0';
        read_charsS++;
        startS = millis();
      }
    }
    logger.println(response_temp_S);
    logger.print("#####  RECEIVED:");
    logger.print(strlen(response_temp_S));
    logger.println("CHARACTERS");
    secureClient.stop();
  }
}

#endif