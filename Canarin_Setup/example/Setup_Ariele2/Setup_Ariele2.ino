// #include <Canarin_Utilities.h>




TaskHandle_t Task1;
TaskHandle_t Task2;

#include "parameter.h"
#include <Canarin_Setup.h>
#include <Canarin_Sampling.h>


byte mac[6];


void setup() {
  disableCore0WDT();
disableCore1WDT();
  spiffsSemaphore = xSemaphoreCreateMutex();

  if (spiffsSemaphore == NULL) {
    logger.println("Failed to create spiffsSemaphore!");
    // Handle failure to create a semaphore, e.g., by rebooting or halting setup
  }

  setup_Primary();

  Canarin_Sampling_setup();


  listDir(SPIFFS, "/", 0);

  if (!SPIFFS.exists("/Sample.data"))
    writeFile(SPIFFS, "/Sample.data", "\0");






  // create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    TaskCommunication, /* Task function. */
    "Connectic Task",  /* name of task. */
    10000,             /* Stack size of task */
    NULL,              /* parameter of the task */
    1,                /* priority of the task */
    &Task1,            /* Task handle to keep track of created task */
    0);                /* pin task to core 0 */
  delay(500);

  // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    TaskSampling,    /* Task function. */
    "Task Sampling", /* name of task. */
    10000,           /* Stack size of task */
    NULL,            /* parameter of the task */
    1,               /* priority of the task */
    &Task2,          /* Task handle to keep track of created task */
    1);              /* pin task to core 1 */
  delay(500);
}


// Task1code: blinks an LED every 1000 ms
void TaskCommunication(void *pvParameters) {
  logger.print("TaskCommunication running on core ");
  logger.println(xPortGetCoreID());
  int counter = 0;
  for (;;) {
    logger.print("BEGIN TASK CONNECT ");
    connect();
    Connectic_status();
    if (first_loop) {
      node_request(0);  //TODO if wrong project then pass
      first_loop = false;
    }
    if (PMU.isCharging()) {
      node_request(2);
      if (!parseAndUpdateConfig(RESPONSE_)) {
        logger.println("Failed to parse and update config.");
      } else {
        logger.println("Config updated successfully.");
        //Connectic_status();
        printDeviceConfig();
        //Connectic_status();
      }
      //TODO if version different Config.Version_to_install
    }
    if (strcmp(version, Node_Config.Version_to_install)) {
      logger.println("Install new version");
    }

    String dataToAppend = "toto  ======= " + String(counter);
    logger.print("WRITING FLASH: ");
    logger.println(dataToAppend);

    if (xSemaphoreTake(spiffsSemaphore, portMAX_DELAY) == pdTRUE) {
      // Append the constructed string to the file

      logger.println("SENDING SAMPLE DATA TO Canarin.net");
      if (SPIFFS.exists("/Sample.data")) {
        printFile(SPIFFS, "/Sample.data");
      }

      xSemaphoreGive(spiffsSemaphore);
    }
    if (xSemaphoreTake(spiffsSemaphore, portMAX_DELAY) == pdTRUE) {
      logger.println("SENDING SAMPLE DATA TO sanarin.sensors@gmail.com");

      if (!SPIFFS.exists("/error.log")) {
        printFile(SPIFFS, "/error.log");
      }

      // Print a message indicating the string has been written to the file



      xSemaphoreGive(spiffsSemaphore);
    }

    delay(1000);  // Wait for 1000 ms between writes
    counter++;    // Increment the counter after each write
    Close();
    while (true)
      delay(1000);
  }
}

// Task2code: blinks an LED every 700 ms
void TaskSampling(void *pvParameters) {
  logger.print("Task2 running on core ");
  logger.println(xPortGetCoreID());

  for (;;) {
    logger.println("SAMPLING--------------------------------------");

    Sampling();
    //Geolocating();
    logger.println("AGGREGATING-----------------------------------");
    char *MESSAGE = Aggregating();
    logger.println(MESSAGE);


    logger.println("PRINT-----------------------------------");
    if (xSemaphoreTake(spiffsSemaphore, portMAX_DELAY) == pdTRUE) {

      // Append the constructed string to the file

      if (SPIFFS.exists("/Sample.data")) {
        printFile(SPIFFS, "/Sample.data");
        appendFile(SPIFFS, "/Sample.data", MESSAGE);

      } else {
        writeFile(SPIFFS, "/Sample.data", MESSAGE);
      }

      // Print a message indicating the string has been written to the file

      //printFile(SPIFFS, "/Sample.data");

      xSemaphoreGive(spiffsSemaphore);
    }
    delete[] MESSAGE;
    while (true)
      delay(1000);
  }
}

void loop() {

  logger.println("SETUP is finished");
  while (true) {
    delay(10000);
  }
}