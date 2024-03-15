#include "esp_task_wdt.h"

bool first_charge = true;

SemaphoreHandle_t spiffsSemaphore;

void TaskCommunication(void *pvParameters)
{

  using namespace std::chrono;

  // Start the timer

  logger.print("TaskCommunication running on core ");
  logger.println(xPortGetCoreID());
  int counter = 0;
  for (;;)
  {
    logger.print("BEGIN TASK CONNECT ");
      auto start = steady_clock::now();

    connect();
    Connectic_status();
    if (first_loop)
    {
      node_request(0); // TODO if wrong project then pass
      first_loop = false;
    }
    if (PMU.isCharging() && first_charge)
    {
      node_request(2);
      if (!parseAndUpdateConfig(RESPONSE_))
      {
        logger.println("Failed to parse and update config.");
      }
      else
      {
        logger.println("Config updated successfully.");
        // Connectic_status();
        printDeviceConfig();
        // Connectic_status();
      }
      // TODO if version different Config.Version_to_install
      first_charge = false;
    }

    if (!PMU.isCharging())
      first_charge = true;

    if (strcmp(version, Node_Config.Version_to_install))
    {
      logger.println("Actual version: " + (String)version);
      logger.println("Install new version: " + (String)Node_Config.Version_to_install);
    }

    
    esp_task_wdt_reset();
    if (xSemaphoreTake(spiffsSemaphore, portMAX_DELAY) == pdTRUE)
    {
      // Append the constructed string to the file

      logger.println("SENDING SAMPLE DATA TO Canarin.net");
      if (SPIFFS.exists("/Sample.data"))
      {
        char *file_content = readFileContent(SPIFFS, "/Sample.data", 1024);

        if (file_content)
        {
          printFile(SPIFFS, "/Sample.data");
          String RESPONSE_ = SendUDP(REMOTE_UDP_ADDR, 60004, file_content);
          logger.println("=========================================================================");
          logger.println(RESPONSE_);
          logger.println("=========================================================================");
          // TODO if success in sending file_content
          //  if (){

          deleteFile(SPIFFS, "/Sample.data");
          //  }

          delete[] file_content;
        }
      }

      xSemaphoreGive(spiffsSemaphore);
    }
    esp_task_wdt_reset();
    if (xSemaphoreTake(spiffsSemaphore, portMAX_DELAY) == pdTRUE)
    {
      logger.println("SENDING SAMPLE DATA TO canarin.sensors@gmail.com");

      if (SPIFFS.exists("/error.log"))
      {
        printFile(SPIFFS, "/error.log");
      }

      // Print a message indicating the string has been written to the file

      xSemaphoreGive(spiffsSemaphore);
    }
    esp_task_wdt_reset();
    logger.println("SENDING PHASE FINISHED");

    counter++; // Increment the counter after each write
    Close();
    logger.println("TASK FINISHED");
    auto end = steady_clock::now();
    // logger.println(end );
    auto elapsed = duration_cast<seconds>(end - start).count();
    logger.println(elapsed);
    int duration = (Node_Config.interval * 60);
    // Sleep for the remaining time if necessary
    logger.println(duration);

    logger.println(duration - elapsed);

    if (elapsed < duration)
    {
      unsigned int remaining_time = duration - static_cast<unsigned int>(elapsed);
      std::this_thread::sleep_for(seconds(remaining_time));
    }

    logger.println("LOOP FINISHED");
  }
}

// Task2code: blinks an LED every 700 ms
void TaskSampling(void *pvParameters)
{
  logger.print("Task2 running on core ");
  logger.println(xPortGetCoreID());

  for (;;)
  {

    logger.println("SAMPLING--------------------------------------");

    Sampling();
    // Geolocating();
    logger.println("AGGREGATING-----------------------------------");

    char MESSAGE[1024] = {'\0'};

    Aggregating(MESSAGE);
    logger.print(MESSAGE);

    logger.println("PRINT-----------------------------------");
    if (xSemaphoreTake(spiffsSemaphore, portMAX_DELAY) == pdTRUE)
    {

      // Append the constructed string to the file

      listDir(SPIFFS, "/", 0);
      listDir(SPIFFS, "/", 1);
      listDir(SPIFFS, "/", 2);
      if (SPIFFS.exists("/Sample.data"))
      {
        printFile(SPIFFS, "/Sample.data");
        appendFile(SPIFFS, "/Sample.data", MESSAGE);
      }
      else
      {
        writeFile(SPIFFS, "/Sample.data", MESSAGE);
      }

      // Print a message indicating the string has been written to the file

      // printFile(SPIFFS, "/Sample.data");

      xSemaphoreGive(spiffsSemaphore);
    }
    // delete[] MESSAGE;
  }
}