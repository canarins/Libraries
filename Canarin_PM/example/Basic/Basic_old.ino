#include "PMS.h"

PMS pms(Serial);
PMS::DATA data;

void setup()
{

  Serial.begin(115200);   // GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board)
  Serial1.begin(9600);  // GPIO2 (D4 pin on ESP-12E Development Board)

}

void loop()
{
  Serial1.println("Test of PMS library");
  if (pms.read(data))
  {
    Serial1.print("PM 1.0 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_1_0);

    Serial1.print("PM 2.5 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_2_5);

    Serial1.print("PM 10.0 (ug/m3): ");
    Serial1.println(data.PM_AE_UG_10_0);

    Serial1.println();
  }
  delay(3000);
  // Do other stuff...
}