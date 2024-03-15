
#ifndef CANARIN_WIFI_H_
#define CANARIN_WIFI_H_

#define DEBUG_CANARIN_WIFI true

#include <WiFi.h>
#include <WiFiUdp.h>

#include <NTPClient.h>
// #include <WiFiUdp.h>
//  TODO What is that and where should it go
// #include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
char iccid[32] = "";

AsyncWebServer asyncwebserver(80);
bool wifiConnected = false;

const long utcOffsetInSeconds = 0; // Set your UTC offset in seconds
// const char *ntpServer = "pool.ntp.org";
// WiFiUDP ntpUDP;

// NTPClient timeClient(ntpUDP, ntpServer, utcOffsetInSeconds);
/*
void recvMsg(uint8_t* data, size_t len) {
  logger.println("Received Data...");
  String d = "";
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
}*/

struct Network_t
{
  String ssid;
  String pass;
};

std::vector<Network_t> networks;

void add(String ssid, String pass)
{ // TODO optimize memory space String const char *
  Network_t network;
  network.ssid = ssid;
  if (pass != nullptr)
    network.pass = pass;

  networks.push_back(network);
  logger.println("------->" + (String)networks.size());
};

String get_encryption_type(wifi_auth_mode_t encryptionType)
{
  switch (encryptionType)
  {
  case (WIFI_AUTH_OPEN):
    return "Open";
  case (WIFI_AUTH_WEP):
    return "WEP";
  case (WIFI_AUTH_WPA_PSK):
    return "WPA_PSK";
  case (WIFI_AUTH_WPA2_PSK):
    return "WPA2_PSK";
  case (WIFI_AUTH_WPA_WPA2_PSK):
    return "WPA_WPA2_PSK";
  case (WIFI_AUTH_WPA2_ENTERPRISE):
    return "WPA2_ENTERPRISE";
  default:
    return "UNKNOWN";
  }
}

int scan_wifis()
{
  logger.println("WiFi Scan");
  logger.println("[*] Scanning WiFi network");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  logger.println("[*] Scan done");
  if (n == 0)
  {
    logger.println("[-] No WiFi networks found");
  }
  else
  {
    logger.print("[+] WiFi networks found: ");
    logger.println(n);
    logger.println();
    logger.println("========================================================");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID, RSSI and WiFi Encryption for each network found
      logger.print(i + 1);
      logger.print(": ");
      // logger.print(WiFi.BSSID(i));
      logger.print(WiFi.SSID(i));
      logger.print(" (");
      logger.print(WiFi.RSSI(i));
      logger.print(" dB) [");
      logger.print(get_encryption_type(WiFi.encryptionType(i)));
      logger.println("]");
      delay(10);
    }
  }
  logger.println("========================================================");
  return n;
}

int wifi_connect(unsigned long connectTimeout = 5000)
{
  logger.print("WL_CONNECTED: ");
  logger.println(WL_CONNECTED);

  logger.print("WL_NO_SHIELD: ");
  logger.println(WL_NO_SHIELD);

  logger.print("WL_IDLE_STATUS: ");
  logger.println(WL_IDLE_STATUS);

  logger.print("WL_CONNECT_FAILED: ");
  logger.println(WL_CONNECT_FAILED);

  logger.print("WL_NO_SSID_AVAIL: ");
  logger.println(WL_NO_SSID_AVAIL);

  logger.print("WL_SCAN_COMPLETED: ");
  logger.println(WL_SCAN_COMPLETED);

  logger.print("WL_CONNECTION_LOST: ");
  logger.println(WL_CONNECTION_LOST);

  logger.print("WL_DISCONNECTED: ");
  logger.println(WL_DISCONNECTED);

  logger.print("STATUS: ");
  logger.println(WiFi.status());

  uint8_t status = WiFi.status();

  if (status == WL_CONNECTED)
  {
    logger.println("already Connected");
    return 1;
  }

  logger.println("NOT INITIALLY Connected");
  auto scanResult = scan_wifis();
  if (scanResult < 1)
    return 0;
  // logger.println(networks);
  for (auto network : networks)
  {
    logger.println(network.ssid);
    for (size_t i = 0; i < scanResult; i++)
    {
      // SSID() is implemented with different return types so we wrap
      // it into String()
      if (String(WiFi.SSID(i)) == network.ssid)
      {
        auto startTime = millis();
        logger.print(network.ssid);
        logger.println(" Found;");
        WiFi.begin(network.ssid.c_str(), network.pass.c_str());
        while (WiFi.status() != WL_CONNECTED && (millis() - startTime) <= connectTimeout)
        {
          delay(500);
          logger.print(".");
        }
        status = WiFi.status();

        startTime = millis();
        while (status == WL_IDLE_STATUS && (millis() - startTime) <= connectTimeout)
        {
          delay(10);
          status = WiFi.status();
        }

        if (status == WL_CONNECTED)
        {
          logger.print("IP Address: ");
          logger.println(WiFi.localIP());
          logger.println("Setting up Web server");

          WebSerial.begin(&asyncwebserver);
          WebSerial.msgCallback(recvMsg);
          asyncwebserver.begin();
          USE_WEBSERIAL = true;
          logger.println("Setting up Web server Complete");
          return 1;
        }
      }
    }
    // if (status == WL_CONNECTED)
    //   break;
  }

  return 0;
};

char *wifi_send(IPAddress server_name, const int port, const char *request_data)
{

  WiFiClient client;
  if (client.connect(server_name, port))
  {
    logger.println("connected");
    // Make a HTTP request:
    client.println("GET /search?q=arduino HTTP/1.0");
    client.println();
  }
  uint32_t start = millis();
  char response[640] = {
      '\0',
  };
  int read_chars = 0;
  while (client.connected() && millis() - start < 10000L)
  {
    while (client.available())
    {
      response[read_chars] = client.read();
      response[read_chars + 1] = '\0';
      read_chars++;
      start = millis();
    }
  }
  logger.println(response);
  logger.print("#####  RECEIVED:");
  logger.print(strlen(response));
  logger.println("CHARACTERS");
  client.stop();

  return response;
}

WiFiUDP udpclient;

char *wifiUDP_send(IPAddress udpAddress, const int udpport, char *request_data)
{

  udpclient.begin(WiFi.localIP(), 80);

  udpclient.beginPacket(udpAddress, udpport);
  udpclient.printf(request_data);
  udpclient.endPacket();

  unsigned long startTime = millis();
  bool packetReceived = false;
  while (millis() - startTime < 1000)
  { // Check for up to 1 second if this timing is necessary
    if (udpclient.parsePacket())
    {
      packetReceived = true;
      break; // Exit the loop if a packet is received
    }
  }

  char *response_wifi_temp = new char[64];
  int bufferSize = 64;
  int read_chars = 0;

  if (packetReceived)
  {
    while (udpclient.available())
    {
      if (read_chars >= bufferSize - 1)
      {
        bufferSize *= 2; // Double the buffer size
        char *temp = new char[bufferSize];
        strcpy(temp, response_wifi_temp); // Copy existing data to new buffer
        delete[] response_wifi_temp;      // Free old buffer
        response_wifi_temp = temp;        // Update pointer to new buffer
      }
      response_wifi_temp[read_chars++] = static_cast<char>(udpclient.read());
      response_wifi_temp[read_chars] = '\0';
    }

    // Null-terminate the string if response_wifi_temp is a char array

    logger.print("#####  RECEIVED: ");
    logger.println(response_wifi_temp);
    logger.print("#####  RECEIVED METADATA: ");
    logger.println(strlen(response_wifi_temp));
    // udpclient.stop();
    return response_wifi_temp;
    // return ExtractDataFromResponse(response_wifi_temp,',');
  }
  // udpclient.stop();
  logger.println(F("No UDP packet received within the timeout period."));
  return nullptr;
}

void wifi_close()
{

  udpclient.stop();
  asyncwebserver.end();
  USE_WEBSERIAL = false;
  // WebSerial.close();
  WiFi.disconnect();
}

void rebuild_network()
{

#ifdef DEBUG_CANARIN_WIFI
  logger.print("REBUILD NETWORK ");
  logger.println(SSID_vector_default.size());
#endif

  networks.clear();
  for (int i = 0; i < SSID_vector_default.size(); i++)
  {
    add(SSID_vector_default[i], PASS_vector_default[i]);
  }

#ifdef DEBUG_CANARIN_WIFI
  logger.print("NETWORKS ");
  logger.println(networks.size());
  for (int i = 0; i < networks.size(); i++)
  {
    logger.print("---> ");
    logger.print(networks[i].ssid);
    logger.print(" ");
    logger.println(networks[i].pass);
  }

  logger.println(Node_Config.wifi_connect);

#endif

  String inputString(Node_Config.wifi_connect);
  inputString.replace("\"", "");
  inputString.remove(0, 1);
  String token;
  int index = 0;
  while ((index = inputString.indexOf(';')) != -1)
  {
    token = inputString.substring(0, index);
    logger.println(token);
    inputString.remove(0, index + 1);
    logger.println(inputString);
    int pos = token.indexOf('|');
    if (pos != -1)
    {
#ifdef DEBUG_CANARIN_WIFI
      logger.print("---> ");
      logger.print(token.substring(0, pos));
      logger.print("   ");
      logger.println(token.substring(pos + 1));
#endif

      add(token.substring(0, pos), token.substring(pos + 1));
    }
  }
  int pos = inputString.indexOf('|');

#ifdef DEBUG_CANARIN_WIFI

  logger.print("---> ");
  logger.print(inputString.substring(0, pos));
  logger.print("   ");
  logger.println(inputString.substring(pos + 1, inputString.length() - 1));
#endif

  add(inputString.substring(0, pos), inputString.substring(pos + 1, inputString.length() - 1));

#ifdef DEBUG_CANARIN_WIFI

  logger.print("NETWORKS ");
  logger.println(networks.size());
  for (int i = 0; i < networks.size(); i++)
  {
    logger.print("---> ");
    logger.print(networks[i].ssid);
    logger.print(" ");
    logger.println(networks[i].pass);
  }
#endif
}

bool wifi_setup = false;

void Canarin_WIFI_setup()
{
  if (wifi_setup)
    return;
  // TODO verify network list updates if necessary only
  rebuild_network();
  wifi_setup = true;
}

#endif