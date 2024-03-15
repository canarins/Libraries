/*
   This file is part of Arduino_ESP32_OTA.

   Copyright 2020 ARDUINO SA (http://www.arduino.cc/)

   This software is released under the GNU General Public License version 3,
   which covers the main part of arduino-cli.
   The terms of this license can be found at:
   https://www.gnu.org/licenses/gpl-3.0.en.html

   You can be released from the requirements of the above licenses by purchasing
   a commercial license. Buying such a license is mandatory if you want to modify or
   otherwise use the software for commercial activities involving the Arduino
   software without disclosing the source code of your own applications. To purchase
   a commercial license, send an email to license@arduino.cc.
*/

#ifndef CANARIN_GSM_OTA_H_
#define CANARIN_GSM_OTA_H_

/******************************************************************************
 * INCLUDE
 ******************************************************************************/



#include <Arduino_DebugUtils.h>

#define TINY_GSM_MODEM_SIM7080
#include <TinyGsmClient.h>
#include "decompress/utility.h"

/******************************************************************************
   DEFINES
 ******************************************************************************/
#if defined (ARDUINO_NANO_ESP32)
  #define CANARIN_GSM_OTA_MAGIC 0x23410070
#else
  #define CANARIN_GSM_OTA_MAGIC 0x45535033
#endif
/******************************************************************************
   CONSTANTS
 ******************************************************************************/

static uint32_t const CANARIN_GSM_OTA_HTTP_HEADER_RECEIVE_TIMEOUT_ms = 10000;
static uint32_t const CANARIN_GSM_OTA_BINARY_BYTE_RECEIVE_TIMEOUT_ms = 2000;

/******************************************************************************
 * TYPEDEF
 ******************************************************************************/

typedef uint8_t(*ArduinoEsp32OtaReadByteFuncPointer)(void);
typedef void(*ArduinoEsp32OtaWriteByteFuncPointer)(uint8_t);

/******************************************************************************
 * CLASS DECLARATION
 ******************************************************************************/

class Canarin_GSM_OTA
{

public:

  enum class Error : int
  {
    None                 =  0,
    NoOtaStorage         = -2,
    OtaStorageInit       = -3,
    OtaStorageEnd        = -4,
    UrlParseError        = -5,
    ServerConnectError   = -6,
    HttpHeaderError      = -7,
    ParseHttpHeader      = -8,
    OtaHeaderLength      = -9,
    OtaHeaderCrc         = -10,
    OtaHeaterMagicNumber = -11,
    OtaDownload          = -12,
  };

           Canarin_GSM_OTA();
  virtual ~Canarin_GSM_OTA() { }

  Canarin_GSM_OTA::Error begin();
  void setCACert (const char *rootCA);
  int download(const char * ota_url, TinyGsmClient * client);
  uint8_t read_byte_from_network();
  void write_byte_to_flash(uint8_t data);
  Canarin_GSM_OTA::Error update();
  void reset();

private:

  TinyGsmClient * _client;
  OtaHeader _ota_header;
  size_t _ota_size;
  uint32_t _crc32;
  const char * _ca_cert;
};

#endif /* CANARIN_GSM_OTA_H_ */
