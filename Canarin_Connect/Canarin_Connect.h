
#ifndef CANARIN_CONNECT_H_
#define CANARIN_CONNECT_H_

#include <Canarin_PMU.h>

#include <Canarin_Wifi.h>

// TODO include the Debug mode for the AT Command in the logger block
/*
#define DUMP_AT_COMMANDS

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
*/

// bool level = false;

#include <Canarin_GSM.h>

#include <thread>

#include <iomanip>
#include <iostream>
#include <string>

template <typename Func, typename... Args>
void execute_with_minimum_duration(Func func, unsigned int duration, Args &&...args)
{
  using namespace std::chrono;

  // Start the timer
  auto start = steady_clock::now();

  // Execute the function with provided arguments
  func(std::forward<Args>(args)...);

  // Calculate the elapsed time in seconds
  auto end = steady_clock::now();
  auto elapsed = duration_cast<seconds>(end - start).count();

  // Sleep for the remaining time if necessary
  if (elapsed < duration)
  {
    unsigned int remaining_time = duration - static_cast<unsigned int>(elapsed);
    std::this_thread::sleep_for(seconds(remaining_time));
  }
}

void Connectic_status()
{
logger.println("====================================================================================");
  logger.print("CONNECTIVITY STATUS : WIFI : ");
  logger.print((WiFi.status() == WL_CONNECTED) ? "CONNECTED" : "NOT CONNECTED");
  logger.print("  GSM :");
  logger.println(modem.isGprsConnected() ? "CONNECTED" : "NOT CONNECTED");
  logger.print("ALIMENTATION : ");
  logger.println((PMU.isCharging()) ? "CHARGING" : "NOT CHARGING");
  logger.print("IP : ");
  logger.println(WiFi.localIP());
  logger.println(modem.localIP());
  delay(50);
  // WebSerial.println(F("--------voltage---------------"));
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
  logger.println("====================================================================================");
  //delay(10000);
}

bool result__1 = false;
bool result__2 = false;

// Wrapper function
int Connectic_wrapper(int (*f1)(long unsigned int), int (*f2)(), bool (*condition1)(), bool (*condition2)())
{

  result__1 = (bool)condition1();
  //logger.print("-> ");
  //logger.println(result__1);

  if (condition1())
  {
    logger.print("INITIALIZING WiFi CONNECTION = ");
  //  logger.println(result__1);
    f1(5000); // Execute function 1
  }
  else
    logger.println("NOT INITIALIZING WiFi CONNECTION ");

  //logger.print("Testing GSM CONNECTION");
  //result__2 = (bool)condition2();
  //logger.print("-> ");
  //logger.println(result__2);
  if (condition2())
  {
    logger.print("INITIALIZING GSM CONNECTION = ");
    //logger.println(result__2);
    f2(); // Execute function 2
  }
  else
    logger.println("NOT INITIALIZING GSM CONNECTION ");

  if (WiFi.status() == WL_CONNECTED && modem.isGprsConnected())
  {


    logger.println("CLOSING REDUNDANT GSM CONNECTION ");

    GSM_close();
  }
  return condition1() && condition2();
}

bool connect_setup = false;

void Canarin_Connectic_setup()
{
  if (connect_setup)
  return;

  Canarin_PMU_setup();

  Canarin_WIFI_setup();
  
  Canarin_GSM_setup();

  connect_setup = true;
}

int connect()
{
  logger.print("CONNECTION    ");
  if (WiFi.status() == WL_CONNECTED)
    return 1;

  if (WiFi.status() != WL_CONNECTED)
  {
    if (USE_WEBSERIAL)
      wifi_close();
    if (wifi_connect(5000))
    {
      logger.print();
      GSM_close();
      return 1;
    }
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    return GSM_connect();
  }
  return 0;
}

char *SendUDP(IPAddress server_name, int port, char *request)
{


  // char* temp_response;
  // RESPONSE[0] = '\0';

#ifdef USB_CDC_ENABLED_CONN
  logger.Sep();
  logger.Sep();
  logger.println("Testing WiFi CONNECTION");
  result__1 = WiFi.status() == WL_CONNECTED;
  logger.print("-> ");
  logger.println(result__1);
#endif

  if (WiFi.status() == WL_CONNECTED)
  {
    logger.print("INITIALIZING WiFi SEND  ");
    //logger.Sep();
    //logger.Sep();
    return wifiUDP_send(server_name, port, request); // Execute function 1
  }
#ifdef USB_CDC_ENABLED_CONN
  else
    logger.println("NOT INITIALIZING WiFi SEND ");

  logger.println("Testing GSM SEND===============================");
  result__2 = modem.isGprsConnected();
  logger.print("-> ");
  logger.println(result__2);
  logger.println("===============================");
#endif

  if (modem.isGprsConnected())
  {
    logger.print("INITIALIZING GSM SEND ");
    logger.Sep();
    logger.Sep();
    return GSMUDP_send(server_name, port, request);
  }
  
#ifdef USB_CDC_ENABLED_CONN
  else
    logger.println("NOT INITIALIZING GSM SEND ");
#endif

  return 0;
}

void Close()
{
  if (PMU.isCharging())
  return;

  if (WiFi.status() == WL_CONNECTED) // TODO if charging no need to disconnect intentionnally but need to account if it does indipendently
  {
    logger.print("INITIALIZING WiFi SHUTDOWN  ");

    wifi_close();
  }
#ifdef USB_CDC_ENABLED_CONN

  else
    logger.println("NOT INITIALIZING WiFi SHUTDOWN ");

      logger.println("Testing GSM===============================");

  logger.println(modem.isGprsConnected());
#endif


  if (modem.isGprsConnected())//TODO GPRS Connected ne devrais pasetre vrai
  {
    logger.print("INITIALIZING GSM SHUTDOWN = ");
    GSM_close();
  }
#ifdef USB_CDC_ENABLED_CONN

  else
    logger.println("NOT INITIALIZING GSM SHUTDOWN ");
#endif
  
}


#endif