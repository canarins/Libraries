#include <Canarin_Setup.h>
#include <Canarin_SPIFFS.h>

void setup(){



    Serial.println("TOTO");logger.println("TOTO 2log");Serial.println("TOTO");
        
//    if(!SPIFFS.begin(true)){
//       Serial.println("SPIFFS Mount Failed");
//        logger.println("SPIFFS Mount Failed");
//        return;
//    }
//    else {
//     Serial.println("SPIFFS Mount Success");
//        logger.println("SPIFFS Mount Success");
//    }
    Canarin_SPIFFS_setup();
    

    listDir(SPIFFS, "/", 0);
/*    writeFile(SPIFFS, "/hello.txt", "Hello ");
    appendFile(SPIFFS, "/hello.txt", "World!\r\n");
    readFile(SPIFFS, "/hello.txt");
    renameFile(SPIFFS, "/hello.txt", "/foo.txt");
    readFile(SPIFFS, "/foo.txt");
    deleteFile(SPIFFS, "/foo.txt");
    testFileIO(SPIFFS, "/test.txt");
    deleteFile(SPIFFS, "/test.txt");
    logger.println( "Test complete" );*/
logger.println("END OF SETUP");
}

void loop(){

while(true){
  delay(10000);
}

}
