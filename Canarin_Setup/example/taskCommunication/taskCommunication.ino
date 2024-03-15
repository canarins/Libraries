

TaskHandle_t Task1;
TaskHandle_t Task2;

//bool first_loop = true;

//#include "parameter.h"
//#include <Canarin_Configuration.h>
#include <Canarin_Setup.h>
#include <Canarin_Sampling.h>



byte mac[6];

String messagetype = "{1,860016043183479,1070149932,37142,7:23,8:24,9:19,4:0.199,5:0.199,6:0.199,40:86.280200,41:0,42:40,43:1422,44:1913,10:0,50:21,45:Pico_Mar_14_2024,46:0.00,47:-1,48:0}";



void setup() {

  //SPIFFS.format();
  disableCore0WDT();
  spiffsSemaphore = xSemaphoreCreateMutex();

  if (spiffsSemaphore == NULL) {
    logger.println("Failed to create spiffsSemaphore!");
    // Handle failure to create a semaphore, e.g., by rebooting or halting setup
  }

  setup_Primary();

  //Canarin_Sampling_setup();


  listDir(SPIFFS, "/", 0);



  // create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    TaskCommunication, /* Task function. */
    "Connectic Task",  /* name of task. */
    10000,             /* Stack size of task */
    NULL,              /* parameter of the task */
    0,                 /* priority of the task */
    &Task1,            /* Task handle to keep track of created task */
    0);                /* pin task to core 0 */
  delay(500);

  // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1


  xTaskCreatePinnedToCore(
    TaskSample_sim, /* Task function. */
    "Simulation of Sampling Task",      /* name of task. */
    10000,                 /* Stack size of task */
    NULL,                  /* parameter of the task */
    0,                     /* priority of the task */
    &Task2,                /* Task handle to keep track of created task */
    1);                    /* pin task to core 0 */
  delay(500);

  // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
}




void TaskSample_sim(void *pvParameters) {
  using namespace std::chrono;
  for (;;) {
    auto start = steady_clock::now();
    if (xSemaphoreTake(spiffsSemaphore, portMAX_DELAY) == pdTRUE) {

      if (!SPIFFS.exists("/Sample.data"))
        writeFile(SPIFFS, "/Sample.data", messagetype.c_str());
      else
        appendFile(SPIFFS, "/Sample.data", messagetype.c_str());
      xSemaphoreGive(spiffsSemaphore);
    }

    logger.println("===================================================================");
    logger.println("SAMPLE FILE");
    logger.println(Filesize(SPIFFS, "/Sample.data"));
    logger.println("===================================================================");


    logger.println("TASK FINISHED");
    auto end = steady_clock::now();
    // logger.println(end );
    auto elapsed = duration_cast<seconds>(end - start).count();
    logger.println(elapsed);
    int duration = (Node_Config.interval * 60);
    // Sleep for the remaining time if necessary
    logger.println(duration);

    logger.println(duration - elapsed);

    if (elapsed < duration) {
      unsigned int remaining_time = duration - static_cast<unsigned int>(elapsed);
      std::this_thread::sleep_for(seconds(remaining_time));
    }

    logger.println("LOOP FINISHED");
  }
}

void loop() {

  logger.println("SETUP is finished");
  while (true) {
    delay(10000);
  }
}