/*
 *  Add new txt file and add  the string <num> -> testing 1, 2, 3. 100 times
 *  than read the file and put on serial output.
 *  library Adafruit_SPIFlash and SdFat - AdafruitFork
 *
 *  by Mischianti Renzo <https://mischianti.org>
 *  https://mischianti.org/
 *  SPIFlash connected via SPI standard check wiring on the article
 *
 */
 




//#include "SdFat.h"
//#include "Adafruit_SPIFlash.h"
#include <Adafruit_NeoPixel.h>
// for flashTransport definition
#include <Canarin_Flash.h>





char fileName[] = "canarin_log.txt";
const int chipSelect = 10;
char charRead;
char pangram_1[] = "testing file write to Canarin";
char pangram_2[] = "Using external SPI flash memory";
char pangram_3[] = "adding also LED status";
 
void setup()
{
  Serial.begin(115200);
  logger.println(F("SETUP"));
  while ( !Serial ) delay(100);   // wait for native usb
  logger.println(F("SETUP2"));

  logger.println("Enter w for write, r for read or d for delete");
  Canarin_Flash_setup();

}
 
void loop() {
  // nothing happens after setup
  if (Serial.available()) 
     {
      charRead = tolower(Serial.read());  //force ucase
      Serial.write(charRead); //write it back to Serial window
      logger.println();
     }
     
  //get command from keyboard:
   switch(charRead)
   {
    case 'r':
        logger.println("detected case r");
        readFromFile();   //read
        break;
    case 'w':
        logger.println("detected case w");
        writeToFile(); //write to file
        break;
    case 'd':
        logger.println("detected case d");
        deleteFile();  //delete
        break;
   }

}


void readFromFile()
{
  // Check to see if the file exists:
 myFile = fatfs.open(fileName);
  if (myFile) {
    pixels.setPixelColor(0, pixels.Color(0, 150, 0));
    pixels.show();  
    delay(500); 
    logger.print("Reading :");
    logger.println(fileName);
 
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
    //flash.setIndicator(LED_GPIO, false);
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();  
    delay(500); 
  } else {
    // if the file didn't open, print an error:
    logger.print("error opening ");
    logger.println(fileName);
    pixels.setPixelColor(0, pixels.Color(150, 0, 0));
    pixels.show();  
  }
 }


 void writeToFile()
{
    pixels.setPixelColor(0, pixels.Color(150, 150, 0));
    pixels.show();  
    delay(500); 
  //myFile = fatfs.open("test2.txt", FILE_WRITE);
   myFile = fatfs.open(fileName, FILE_WRITE);
  if (myFile) // it opened OK
    {
    logger.print("Writing to ");
    logger.println(fileName);
    myFile.println(pangram_1);
    myFile.println(pangram_2);
    myFile.println(pangram_3);
    myFile.close(); 
    logger.println("Done");
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();  
    delay(500);
    }
  else 
    {
    // if the file didn't open, print an error:
    logger.print("error writing to ");
    logger.println(fileName);
    pixels.setPixelColor(0, pixels.Color(150, 0, 0));
    pixels.show();
    }
}

void deleteFile()
{
 //delete a file:
  if (fatfs.exists(fileName)) // if file exist
    {
    pixels.setPixelColor(0, pixels.Color(150, 0, 0));
    pixels.show();  
    delay(500); 
    logger.print("Removing ");
    logger.println(fileName);
    //f_unlink(fileName);
    fatfs.remove(fileName);
    logger.println("Done");
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();  
    delay(500); 
    }
  else 
    {
    // if the file didn't open, print an error:
    logger.print("error deleting to ");
    logger.println(fileName);
    pixels.setPixelColor(0, pixels.Color(150, 0, 0));
    pixels.show();
    }
}
