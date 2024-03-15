#ifndef CANARIN_CONFIGURATION_H
#define CANARIN_CONFIGURATION_H



#include <Arduino.h>
#include <string.h>



char RESPONSE_[1024] = {'\0'};
// Define a struct to hold the configuration
struct DeviceConfig
{
    int interval;              // 0
    bool isCloudMode;          // 1
    int pmSamplingTime;        // 2
    bool gpsON;                // 3
    int cell_location;         // 4
    bool resetModem;           // 5
    bool doSleep;              // 6
    int batterySleepTime;      // 7
    bool remoteControl;        // 8
    bool ntpTime;              // 9
    bool exactMin;             // 10
    int cloud_time;            // 11
    bool erase_fs;             // 12
    bool resetGPS;             // 13
    bool updateGPSRate;        // 14
    bool restoreVars;          // 15
    int cell_location_timeout; // 16
    bool Hard_reset;           // 17 //TODO remove data in flash and reset variable
    char *Version_to_install;  // 18
    int Sample_size;           // 19
    char *wifi_connect;        // 20 (SSID|Password;SSID2|Password2)
    char *alert_SMS;           // 21 (+33630807628;+33630807629)
    char *error_log_email;     // 22 (canarinssensors@gmail.com)
    char *intended_project_id; // 23 "12"
};

DeviceConfig Node_Config = {
    .interval = 1,
    .isCloudMode = false,
    .pmSamplingTime = 5000,
    .gpsON = true,
    .cell_location = 2,
    .resetModem = false,
    .doSleep = false,
    .batterySleepTime = 2400, // 30 * 60
    .remoteControl = false,
    .ntpTime = true,
    .exactMin = true,
    .cloud_time = 120,
    .erase_fs = false,
    .resetGPS = false,
    .updateGPSRate = false,
    .restoreVars = false,
    .cell_location_timeout = 15,
    .Hard_reset = false,
    .Version_to_install = "init",
    .Sample_size = 10,
    .wifi_connect = "(SSID3|PASS3;SSID4|PASS4)", // TODO SEPARATOR FOR MULTIPLE INSTANCE -> use "\;", "\(" and "\)"
    .alert_SMS = "(+33630807628)",
    .error_log_email = "(canarinssensors@gmail.com)",
    .intended_project_id = "12"};


#include <Canarin_SPIFFS.h>

#include <Canarin_Connect.h>


void printDeviceConfig()
{ // TODO check if all variable are used and clean the print
    // Assuming 'config' has fields like 'name', 'version', and 'isActive' (as examples)
    logger.println(" Print Device Config ");
    logger.print(" 0 interval: ");
    logger.println(Node_Config.interval);
    logger.print(" 1 isCloudMode: ");
    logger.println(Node_Config.isCloudMode ? "true" : "false");
    logger.print(" 2 pmSamplingTime: ");
    logger.println(Node_Config.pmSamplingTime);
    logger.print(" 3 gpsON: ");
    logger.println(Node_Config.gpsON ? "true" : "false");
    logger.print(" 4 cell_location: ");
    logger.println(Node_Config.cell_location);
    logger.print(" 5 resetModem: ");
    logger.println(Node_Config.resetModem ? "true" : "false");
    logger.print(" 6 doSleep: ");
    logger.println(Node_Config.doSleep ? "true" : "false");
    logger.print(" 7 batterySleepTime: ");
    logger.println(Node_Config.batterySleepTime);
    logger.print(" 8 remoteControl: ");
    logger.println(Node_Config.remoteControl ? "true" : "false");
    logger.print(" 9 ntpTime: ");
    logger.println(Node_Config.ntpTime ? "true" : "false");
    logger.print("10 exactMin: ");
    logger.println(Node_Config.exactMin ? "true" : "false");
    logger.print("11 cloud_time: ");
    logger.println(Node_Config.cloud_time);
    logger.print("12 erase_fs: ");
    logger.println(Node_Config.erase_fs ? "true" : "false");
    logger.print("13 resetGPS: ");
    logger.println(Node_Config.resetGPS ? "true" : "false");
    logger.print("14 updateGPSRate: ");
    logger.println(Node_Config.updateGPSRate ? "true" : "false");
    logger.print("15 restoreVars: ");
    logger.println(Node_Config.restoreVars ? "true" : "false");
    logger.print("16 cell_location_timeout: ");
    logger.println(Node_Config.cell_location_timeout);
    logger.print("17 Hard_reset: ");
    logger.println(Node_Config.Hard_reset ? "true" : "false");
    logger.print("18 Version_to_install: ");
    logger.println(Node_Config.Version_to_install);
    logger.print("19 Sample_size: ");
    logger.println(Node_Config.Sample_size);
    logger.print("20 wifi_connect: ");
    logger.println(Node_Config.wifi_connect);
    logger.print("21 alert_SMS: ");
    logger.println(Node_Config.alert_SMS);
    logger.print("22 error_log_email: ");
    logger.println(Node_Config.error_log_email);
    logger.print("23 intended_project_id: ");
    logger.println(Node_Config.intended_project_id);

    // Add more fields if necessary
}

void serializeDeviceConfig(char *outStr)
{
    snprintf(outStr, 1024, // Make sure to define or check the buffer size to prevent overflows
             "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%s:%d:%s:%s:%s:%s",
             Node_Config.interval,
             Node_Config.isCloudMode ? 1 : 0,
             Node_Config.pmSamplingTime,
             Node_Config.gpsON ? 1 : 0,
             Node_Config.cell_location,
             Node_Config.resetModem ? 1 : 0,
             Node_Config.doSleep ? 1 : 0,
             Node_Config.batterySleepTime,
             Node_Config.remoteControl ? 1 : 0,
             Node_Config.ntpTime ? 1 : 0,
             Node_Config.exactMin ? 1 : 0,
             Node_Config.cloud_time,
             Node_Config.erase_fs ? 1 : 0,
             Node_Config.resetGPS ? 1 : 0,
             Node_Config.updateGPSRate ? 1 : 0,
             Node_Config.restoreVars ? 1 : 0,
             Node_Config.cell_location_timeout,
             Node_Config.Hard_reset ? 1 : 0,
             Node_Config.Version_to_install,
             Node_Config.Sample_size,
             Node_Config.wifi_connect,
             Node_Config.alert_SMS,
             Node_Config.error_log_email,
             Node_Config.intended_project_id);
}

void deserializeDeviceConfig(const char *inStr)
{
    int isCloudMode, gpsON, resetModem, doSleep, remoteControl,
        ntpTime, exactMin, erase_fs, resetGPS, updateGPSRate,
        restoreVars, Hard_reset;
    sscanf(inStr,
           "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%20s:%d:%s:%s:%s:%s",
           &Node_Config.interval,
           &isCloudMode,
           &Node_Config.pmSamplingTime,
           &gpsON,
           &Node_Config.cell_location,
           &resetModem,
           &doSleep,
           &Node_Config.batterySleepTime,
           &remoteControl,
           &ntpTime,
           &exactMin,
           &Node_Config.cloud_time,
           &erase_fs,
           &resetGPS,
           &updateGPSRate,
           &restoreVars,
           &Node_Config.cell_location_timeout,
           &Hard_reset,
           Node_Config.Version_to_install,
           &Node_Config.Sample_size,
           Node_Config.wifi_connect,
           Node_Config.alert_SMS,
           Node_Config.error_log_email,
           Node_Config.intended_project_id

    );

    // Convert integer values back to bool where necessary
    Node_Config.isCloudMode = isCloudMode != 0;
    Node_Config.gpsON = gpsON != 0;
    Node_Config.resetModem = resetModem != 0;
    Node_Config.doSleep = doSleep != 0;
    Node_Config.remoteControl = remoteControl != 0;
    Node_Config.ntpTime = ntpTime != 0;
    Node_Config.exactMin = exactMin != 0;
    Node_Config.erase_fs = erase_fs != 0;
    Node_Config.resetGPS = resetGPS != 0;
    Node_Config.updateGPSRate = updateGPSRate != 0;
    Node_Config.restoreVars = restoreVars != 0;
    Node_Config.Hard_reset = Hard_reset != 0;
}

// #####  RECEIVED: {2,860016040662772,12,1708702002,0:1,1:0,2:5000,3:1,4:2,5:0,6:0,7:1800,8:0,9:1,10:1,11:120,12:0,13:0,14:0,15:0,16:15}

void updateConfigByIndex(int index, const String &valueStr)
{
    /*
    logger.print("updateConfigByIndex ");
    logger.print(index);
    logger.print(" ");
    logger.println(valueStr);
*/
    switch (index)
    {
    case 0:
        Node_Config.interval = valueStr.toInt();
        break;
    case 1:
        Node_Config.isCloudMode = valueStr != "0";
        break;
    case 2:
        Node_Config.pmSamplingTime = valueStr.toInt();
        break;
    case 3:
        Node_Config.gpsON = valueStr != "0";
        break;
    case 4:
        Node_Config.cell_location = valueStr != "0";
        break;
    case 5:
        Node_Config.resetModem = valueStr != "0";
        break;
    case 6:
        Node_Config.doSleep = valueStr != "0";
        break;
    case 7:
        Node_Config.batterySleepTime = valueStr.toInt();
        break;
    case 8:
        Node_Config.remoteControl = valueStr != "0";
        break;
    case 9:
        Node_Config.ntpTime = valueStr != "0";
        break;
    case 10:
        Node_Config.exactMin = valueStr != "0";
        break;
    case 11:
        Node_Config.cloud_time = valueStr.toInt();
        break;
    case 12:
        Node_Config.erase_fs = valueStr != "0";
        break;
    case 13:
        Node_Config.resetGPS = valueStr != "0";
        break;
    case 14:
        Node_Config.updateGPSRate = valueStr != "0";
        break;
    case 15:
        Node_Config.restoreVars = valueStr != "0";
        break;
    case 16:
        Node_Config.cell_location_timeout = valueStr.toInt();
        break;
    case 17:
        Node_Config.Hard_reset = valueStr != "0";
        break;
    case 18:
        //logger.println(sizeof(Node_Config.Version_to_install) - 1);
        //logger.println(valueStr.length());
        strncpy(Node_Config.Version_to_install, valueStr.c_str(), valueStr.length());
        Node_Config.Version_to_install[valueStr.length()] = '\0'; // Ensure null-termination
        break;
    case 19:
        Node_Config.Sample_size = valueStr.toInt();
        break;
    case 20:
        logger.println(sizeof(Node_Config.wifi_connect) - 1);
        logger.println(valueStr.length());
                 Connectic_status();

        strncpy(Node_Config.wifi_connect, valueStr.c_str(), valueStr.length());//TODO Why is GSM connectng
                         Connectic_status();

        Node_Config.wifi_connect[valueStr.length()] = '\0';
                 Connectic_status();

                 rebuild_network();
        break;
    case 21:
        //logger.println(sizeof(Node_Config.alert_SMS) - 1);
        //logger.println(valueStr.length());
        strncpy(Node_Config.alert_SMS, valueStr.c_str(), valueStr.length());
        Node_Config.alert_SMS[valueStr.length()] = '\0';
        break;
    case 22:
        //logger.println(sizeof(Node_Config.error_log_email) - 1);
        //logger.println(valueStr.length());
        strncpy(Node_Config.error_log_email, valueStr.c_str(),valueStr.length());
        Node_Config.error_log_email[valueStr.length()] = '\0';
        break;
    case 23:
        //logger.println(sizeof(Node_Config.intended_project_id) - 1);
        //logger.println(valueStr.length());
        strncpy(Node_Config.intended_project_id, valueStr.c_str(), valueStr.length());
        Node_Config.intended_project_id[valueStr.length()] = '\0';
        break;

    // Continue for other indexes...
    // Note: This example simplifies boolean assignments; adjust parsing as needed.
    default:
        logger.print("Unknown index ");
        logger.println(index);
        break;
    }
}

#include <vector>
void splitString(String data, char separator, std::vector<String> &dest)
{
/*#ifdef USB_CDC_ENABLED
    logger.print("splitString ");
    logger.print(data);
    logger.print(" ");
    logger.println(separator);
#endif*/
    int iPos = 0;
    int newPos = -1;
    int sizeS2 = data.length();

    while (iPos < sizeS2)
    {
/*#ifdef USB_CDC_ENABLED
// logger.print(iPos);
// logger.print(" < ");
// logger.println(sizeS2);
#endif*/
        newPos = data.indexOf(separator, iPos);
/*#ifdef USB_CDC_ENABLED

        logger.print(iPos);
        logger.print(" ->  ");
        logger.println(newPos);
#endif*/

        if (newPos == -1)
        {
            newPos = sizeS2;
        }
        //logger.println(data.substring(iPos, newPos));
        dest.push_back(data.substring(iPos, newPos));

        iPos = newPos + 1;
        // logger.println(iPos);
    }
}

bool parseAndUpdateConfig(String configStr)
{
    logger.print("parseAndUpdateConfig : ");
    logger.println(configStr);
    //Connectic_status();
    //logger.print("parseAndUpdateConfig  ");
    if (!configStr.startsWith("{") || !configStr.endsWith("}"))
        return false;
    std::vector<String> parts; // Adjust size based on expected maximum number of parts
    logger.println(configStr.substring(1, configStr.length() - 1));
    if (configStr.length() > 2)
    {
        splitString(configStr.substring(1, configStr.length() - 1), ',', parts);
    }
    else
    {
        // The string is too short to remove both first and last characters
        return false;
    }

/*
#ifdef USB_CDC_ENABLED
    logger.println("PARTS");
    for (int i = 0; i < parts.size(); i++)
        logger.println(parts[i]);

#endif
*/

    if (parts[0].toInt() != 2)
    {
        logger.println("REQUEST NOT ACCEPTED");
        return false;
    }
    // Convert the part to a C-string for comparison
    if (strcmp(parts[1].c_str(), NODE_ID) != 0)
    {
        logger.print(parts[1].c_str());
        logger.print(" != ");
        logger.print(NODE_ID);
        logger.println("NODE_ID not good");
        return false;
    }
    //Connectic_status();

    // Assuming PROJECT_ID is a char array; adjust if it's a different type
    if (strcmp(parts[2].c_str(), PROJECT_ID) != 0)
    {
        logger.print(parts[2].c_str());
        logger.print(" != ");
        logger.print(PROJECT_ID);
        logger.println("PROJECT_ID not good");
        return false;
    }
    //Connectic_status();

    unsigned long timestamp = (unsigned long)parts[3].toInt();
    if (timestamp <= LAST_VAR_UPDATE)
    {
        logger.print(timestamp);//TODO replace print by error
        logger.print(" <= ");
        logger.print(LAST_VAR_UPDATE);
        logger.println("LAST UPDATE not good");
        return false;
    }
    //logger.println("TEST");
    //Connectic_status();
    //logger.println("TEST");
    for (int i = 4; i < parts.size(); i++)
    { // Adjust loop to match parts array size
        int index = parts[i].substring(0, parts[i].indexOf(':')).toInt();
        String value = parts[i].substring(parts[i].indexOf(':') + 1);

        // if (!
        logger.print(index);
        logger.print("  ");
        logger.println(value);
        // Connectic_status();
        updateConfigByIndex(index, value);

        // Connectic_status(); //) {
        //    return false;
        //}
    }
    LAST_VAR_UPDATE = timestamp;
    //Connectic_status();
    logger.println("EXIT PARSE");
    return true;
}


int node_request(int TYPE_REQUEST)
{
  logger.println("-----------------------------------------------------------------------------------------------------------------");

  char dataS[680];

  //---------------------------------------------------------------------------------------------------------------------------------------

  char str0[60] = {'\0'};
  snprintf(str0, sizeof(str0), "{%d,%s,%s}", TYPE_REQUEST, NODE_ID, PROJECT_ID);

  //logger.printf("----------->%s\n", str0); // Outputs: {0,42,12}

  snprintf(dataS, sizeof(dataS), "%s", str0);
  logger.print("------------>");
  logger.println(dataS);
  //logger.println(sizeof(dataS));

  // execute_with_minimum_duration(Send, 1, REMOTE_UDP_ADDR, 60004, dataS);
  //Connectic_status();
  String test = SendUDP(REMOTE_UDP_ADDR, 60004, dataS);
  //Connectic_status();
  //logger.println(test);
  //logger.println(test.length());
  strncpy(RESPONSE_, test.c_str(), test.length());
  RESPONSE_[test.length()] = '\0';

  //logger.println("---->");
  logger.println("RESPONSE Node REQUEST : " + (String)RESPONSE_);
  //logger.println("-----------------------------------------------------------------------------------------------------------------");

  return 0;
}


void get_configuration(){
   logger.println("BEGIN SETUP PRETHREADS");
  Connectic_status();
  connect();
  Connectic_status();
  logger.println("GET NODE REGISTRATION");
  Connectic_status();
  node_request(0);
  Connectic_status();
  logger.println("CONFIGURATION PAR DEFAULT MACHINE");
  
  printDeviceConfig();

  logger.println("CHECK FLASH FOR CONFIGURATION");

  if (!SPIFFS.exists("/Configuration.txt"))
  {
    logger.println("CONFIGURATION FILE NOT EXIST");
    // If the file is empty, serialize and write the device configuration
    char config_char[1024] = {'\0'};
    serializeDeviceConfig(config_char);
    logger.println(config_char);
    logger.println(sizeof(config_char));
    writeFile(SPIFFS, "/Configuration.txt", config_char);
  }
  else
  {
    printFile(SPIFFS, "/Configuration.txt");

    File configFile = SPIFFS.open("/Configuration.txt", "r");
    if (configFile && configFile.size() == 0)
    {
      logger.println("CONFIGURATION FILE EMPTY");
      // If the file is empty, serialize and write the device configuration
      char config_char[1024] = {'\0'};
      serializeDeviceConfig(config_char);
      logger.println(config_char);
      logger.println(sizeof(config_char));
      writeFile(SPIFFS, "/Configuration.txt", config_char);
    }
    else
    {
      // If the file isn't empty, deserialize the device configuration
      logger.println("CONFIGURATION FILE EXISTS");
      char config_char2[1024];
      char *readfile = readFile(SPIFFS, "/Configuration.txt");

      strncpy(config_char2, readfile, strlen(readfile));
      config_char2[strlen(readfile)] = '\0';
      deserializeDeviceConfig(config_char2);
      logger.println(config_char2);
    }
    configFile.close();
  }

  printDeviceConfig();
  logger.println("GET REMOTE CONFIGURATION");

  // Request data from node with id=2
  Connectic_status();
  node_request(2); // TODO No UDP packet received within the timeout period.Guru Meditation Error: Core  1 panic'ed (LoadProhibited). Exception was unhandled.
  Connectic_status();
  //logger.println(RESPONSE_);
  // Assuming RESPONSE_ is updated by node_request and contains a valid response

  // loggerize the response and overwrite the Configuration.txt file
  if (! parseAndUpdateConfig(RESPONSE_))
  {
    logger.println("Failed to parse and update config.");
  }
  else
  {
    logger.println("Config updated successfully.");
    //Connectic_status();
    printDeviceConfig();
    //Connectic_status();
  }
  

  //Connectic_status();

  logger.println("CLOSING CONNECTION");
  Connectic_status();
  Close(); // TODO GSM shutting down when WIFI activate
  Connectic_status();
  logger.println("END OF SETUP PRETHREADS");
}


/*
bool parseAndUpdateConfig(const String& data) {
    // Existing parsing code here...
    // Assume we've already parsed and validated the initial part of the data
        if (!data.startsWith("{") || !data.endsWith("}")) return false;

    int sections[5]; // To store positions of commas separating the sections
    int sectionCount = 0;
    for (int i = 1; i < data.length() - 1 && sectionCount < 5; ++i) {
        if (data[i] == ',' || data[i] == '[') {
            sections[sectionCount++] = i;
            if (data[i] == '[') break; // Stop at the array start
        }
    }

    // Parse and validate the first term
    int interval = data.substring(2, sections[0]).toInt();
    if (interval == 0) {
        logger.println("Error: First term is 0");
        return false;
    } else if (interval != 2) {
        logger.println("Error: First term is not 2");
        return false;
    }

    // Parse and validate node_ID (isCloudMode used as placeholder)
    logger.print(data.substring(sections[0] + 2, sections[1]).c_str());
    logger.print(" =? ");
    logger.println(NODE_ID);
        logger.println(strcmp(data.substring(sections[0] + 2, sections[1]).c_str(),NODE_ID));
    if (strcmp(data.substring(sections[0] + 2, sections[1]).c_str(), NODE_ID) != 0) { // Assuming 123456789 is the expected node_ID
        logger.println("Error: node_ID does not match");
        return false;
    }

    // Parse and validate project_id (pmSamplingTime used as placeholder)
        logger.print(sections[1] + 2, sections[2]);
    logger.print(" =? ");
    logger.println(PROJECT_ID);
    logger.println(strcmp(data.substring(sections[1] + 2, sections[2]).c_str(),PROJECT_ID));
    if (strcmp(data.substring(sections[1] + 2, sections[2]).c_str(),PROJECT_ID) != 0 ) { // Assuming 42 is the expected project_id
        logger.println("Error: project_id does not match");
        return false;
    }

    // Parse and validate the timestamp
    unsigned long timestamp = (unsigned long)data.substring(sections[2] + 2, sections[3]).toInt();
    if (timestamp <= LAST_VAR_UPDATE) {
        logger.println("Error: Timestamp is not after the last update");
        return false;
    }


    // Find the key-value pair section within the brackets
    int keyValueStart = data.indexOf('[', sections[3]) + 1; // Start after '['
    int keyValueEnd = data.indexOf(']', keyValueStart); // Find closing ']'
    String keyValueSection = data.substring(keyValueStart, keyValueEnd);

    // Split the key-value pairs
    int start = 0;
    while (start < keyValueSection.length()) {
        int colonPos = keyValueSection.indexOf(':', start);
        int commaPos = keyValueSection.indexOf(',', start);
        if (commaPos == -1) commaPos = keyValueSection.length(); // Last pair

        if (colonPos != -1) {
            int index = keyValueSection.substring(start, colonPos).toInt();
            String val = keyValueSection.substring(colonPos + 1, commaPos);
            val.trim();

            // Update the Config based on index and value
            updateConfigByIndex(index, val);

            start = commaPos + 1; // Move past the comma for the next iteration
        } else {
            // No colon found, break out of the loop
            break;
        }
    }

    return true; // Return true if parsing and updating were successful
}

*/
#endif