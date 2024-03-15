// #include <Canarin_Utilities.h>





TaskHandle_t Task2;

//#include "parameter.h"
//bool first_loop = true;
#include <Canarin_Setup.h>


byte mac[6];


void setup() {

    //SPIFFS.format();

disableCore1WDT();
  spiffsSemaphore = xSemaphoreCreateMutex();

  if (spiffsSemaphore == NULL) {
    logger.println("Failed to create spiffsSemaphore!");
    // Handle failure to create a semaphore, e.g., by rebooting or halting setup
  }

  setup_Primary();

  Canarin_Sampling_setup();

  listDir(SPIFFS, "/", 0);

//  if (!SPIFFS.exists("/Sample.data"))
//    writeFile(SPIFFS, "/Sample.data", "{}");

  // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    TaskSampling,    /* Task function. */
    "Task Sampling", /* name of task. */
    10000,           /* Stack size of task */
    NULL,            /* parameter of the task */
    0,               /* priority of the task */
    &Task2,          /* Task handle to keep track of created task */
    1);              /* pin task to core 1 */
  delay(500);
}



// Task1code: blinks an LED every 1000 ms


void loop() {

  logger.println("SETUP is finished");
  while (true) {
    delay(10000);
  }
}