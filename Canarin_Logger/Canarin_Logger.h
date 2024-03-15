#ifndef CANARIN_LOGGER_H_
#define CANARIN_LOGGER_H_

#include <WebSerial.h>

#define USB_CDC_ENABLED (Serial)

#include <iostream>
#include <cstdio>
#include <cstdarg>

// TODO see what is the difference between print, println, printf, write ....
bool USE_WEBSERIAL = false;

/// @brief
class Log
{
public:
  Log()
  {

    Serial.begin(9600);

    // Start while waiting for Serial monitoring
    while (!Serial)
      delay(10);

    Serial.println("LOGGER");
    // delay(3000);
    println();
  }
  // PRINT

  template <typename T>
  void error(const T &value)
  {

    Serial.println(value);

    if (USE_WEBSERIAL)
      WebSerial.print(value);
  }

  template <typename T>
  void print(const T &value)
  {

    Serial.print(value);

    if (USE_WEBSERIAL)
      WebSerial.print(value);
  }

  void print()
  {
    print("");
  }

  void print(int i, int j)
  {
    Serial.print(i, j);

    if (USE_WEBSERIAL)
      WebSerial.print(i, j);
  }

  // PRINTLN
  template <typename T>
  void println(const T &value)
  {
    print(value);
    print("\n");
  }

  void println(const char *value)
  {

    Serial.println(value);
  }

  void println()
  {
    println("");
  }

  void println(int i, int j)
  {
    print(i, j);
    print("\n");
  }
  // WRITE
  template <typename T>
  void write(const T &value)
  {

    Serial.write(value);

    if (USE_WEBSERIAL)
      WebSerial.print(value);
  }

  // PRINTF
  template <typename... Args>
  void printf(const char *format, Args... args)
  {

    Serial.printf(format, args...);

    if (USE_WEBSERIAL)
    {
      const int BUFFER_SIZE = 256; // Adjust the size as necessary
      char buffer[BUFFER_SIZE];

      // Use snprintf to format the string safely within the buffer size
      snprintf(buffer, BUFFER_SIZE, format, args...);

      // Send the buffer content to WebSerial
      WebSerial.print(buffer);
    }
  }
  bool available()
  {
    return Serial.available();
  }

  bool availableForWrite()
  {
    return Serial.availableForWrite();
  }

  int read()
  {
    return Serial.read();
  }

  void Sep()
  {
    println("-------------------------------------");
  }
};

Log logger;

void recvMsg(uint8_t *data, size_t len)
{
  logger.println("Received WebSerial Data...");
  String d = "";
  for (int i = 0; i < len; i++)
  {
    d += char(data[i]);
  }
  logger.println(d);
}

#endif /* CANARIN_LOGGER_H_ */