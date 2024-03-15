
#ifndef CANARIN_FILE_H_
#define CANARIN_FILE_H_


#include <Canarin_Utilities.h>
#include "FS.h"
#include "SPIFFS.h"


void appendFile(fs::FS &fs, const char *path, const char *message);
char * readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void deleteFile(fs::FS &fs, const char *path);


//---------------------------------------------------------------------------------




//---------------------------------------------------------------------------------




void appendFile(fs::FS &fs, const char *path, const char *message) {
    logger.println("Appending to file:" + String(path));

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        logger.println("Failed to open file for appending");
        return;
    }
    if (file.print(message)) {
        logger.println("Appending OK");
    } else {
        logger.println("Appending KO");
    }
    file.close();
}

//char * buffer_data = 0;

char* readFile(fs::FS &fs, const char *path) {
    //int bufferSize = sizeof(buffer) / sizeof(buffer[0]); // Determine the size of the buffer
    
    File file = fs.open(path);
    if (!file || file.isDirectory()) {
        logger.println("Failed to open file for reading");
        return nullptr;
    }
    size_t fileSize = file.size();
    //logger.println("File Size : "+ String(fileSize));
    char * buffer_data = new char[fileSize + 1]; // Allocate buffer to hold file content plus null terminator

    //logger.println(String(buffer_data));
    int i = 0;
    //logger.println(" " + String(i) + "; file.available(): " + (file.available()?"T" :"F" ) + "; i < fileSize  : " + (i < fileSize  ? "T" : "F"));
    while (file.available() && i < fileSize ) { // Ensure there's room for '\0'
        
        buffer_data[i++] = (char)file.read();
        delayMicroseconds(100); // You might want to remove this if not needed
        //logger.println(String(buffer_data));
        //logger.println(" " + String(i) + "; file.available(): " + (file.available()?"T" :"F" ) + "; i < fileSize : " + (i < fileSize  ? "T" : "F"));
    }

    buffer_data[i] = '\0'; // Null-terminate the string
    //logger.println(String(buffer_data));
    // Do note that using static buffer is not thread-safe and not reentrant.
    // Use with caution or consider dynamically allocating memory.

    file.close(); // Close the file as we're done with it
    return buffer_data; 
}

size_t Filesize(fs::FS &fs,const char* filePath){
File file = fs.open(filePath, "r");
    if (!file) {
        logger.println("Failed to open file for reading");
        return 0;
    }
    size_t fileSize = file.size();
    file.close();
    return fileSize;

}

char* readFileContent(fs::FS &fs,const char* filePath , int minimumsize) {
    // Open the file in read mode
    File file = fs.open(filePath, "r");
    if (!file) {
        logger.println("Failed to open file for reading");
        return nullptr;
    }

    // Check the size of the file
    size_t fileSize = file.size();
    if (fileSize < minimumsize) {
        logger.println("File size is less than 10 KB");
        file.close();
        return nullptr;
    }

    // Allocate memory for the buffer to store file content
    char* buffer = new char[fileSize + 1]; // +1 for null terminator
    if (!buffer) {
        logger.println("Failed to allocate memory for buffer");
        file.close();
        return nullptr;
    }

    // Read file content into the buffer
    size_t bytesRead = file.readBytes(buffer, fileSize);
    buffer[bytesRead] = '\0'; // Null terminate the string
    file.close();
return buffer;
    // Do something with the file content stored in the buffer
    // For example, print it to the Serial monitor
    //logger.println("File content:");
    //logger.println(buffer);

    // Free memory allocated for the buffer
    //delete[] buffer;
}

void printFile(fs::FS &fs, const char *path) {
    logger.println("Reading file:"+ String(path));

    File file = fs.open(path);
    if (!file || file.isDirectory()) {
        logger.println("Failed to open file for reading");
        return;
    }

    logger.print("Read from file: ");
    while (file.available()) {
        logger.write(file.read());
        delayMicroseconds(100);
    }
    logger.println();
    logger.println("--------------------------------");
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
    logger.printf("Writing file: %s\n", path);
logger.print(message);
    logger.println("------------------------------------------");

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        logger.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        logger.println("File written");
    } else {
        logger.println("Write failed");
    }
    file.close();
}





void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    logger.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        logger.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        logger.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            logger.print("  DIR : ");
            logger.println(file.name());
            if (levels) {
                listDir(fs, file.name(), levels - 1);
            }
        } else {
            logger.print("  FILE: ");
            logger.print(file.name());
            logger.print("  SIZE: ");
            logger.println(file.size());
        }
        file = root.openNextFile();
    }
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    logger.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        logger.println("- file renamed");
    } else {
        logger.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char *path) {
    logger.printf("Deleting file: %s\n", path);
    if (fs.remove(path)) {
        logger.println("File deleted");
    } else {
        logger.println("Delete failed");
    }
}


void testFileIO(fs::FS &fs, const char * path){
    logger.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        logger.println("- failed to open file for writing");
        return;
    }

    size_t i;
    logger.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          logger.print(".");
        }
        file.write(buf, 512);
    }
    logger.println("");
    uint32_t end = millis() - start;
    logger.printf(" - %u bytes written in %lu ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        logger.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              logger.print(".");
            }
            len -= toRead;
        }
        logger.println("");
        end = millis() - start;
        logger.printf("- %u bytes read in %lu ms\r\n", flen, end);
        file.close();
    } else {
        logger.println("- failed to open file for reading");
    }
}


#endif