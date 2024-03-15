#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
/*
   This sample code tracks satellite elevations using TinyGPSCustom objects.

   Satellite numbers and elevations are not normally tracked by TinyGPSPlus, but 
   by using TinyGPSCustom we get around this.

   It requires the use of SoftwareSerial and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(RX) and 3(TX).
 */
static const int RXPin = 44, TXPin = 45;
static const uint32_t GPSBaud = 4800;
static const int MAX_SATELLITES = 40;
static const int PAGE_LENGTH = 40;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

TinyGPSCustom totalGPGSVMessages(gps, "GPGSV", 1); // $GPGSV sentence, first element
TinyGPSCustom messageNumber(gps, "GPGSV", 2);      // $GPGSV sentence, second element
TinyGPSCustom satNumber[4]; // to be initialized later
TinyGPSCustom elevation[4];
bool anyChanges = false;
unsigned long linecount = 0;

struct {
	int elevation;
	bool active;
} sats[MAX_SATELLITES];

void setup() {
	Serial.begin(115200);
	ss.begin(GPSBaud);

	logger.println(F("SatElevTracker.ino"));
	logger.println(F("Displays GPS satellite elevations as they change"));
	logger.print(F("Testing TinyGPSPlus library v. ")); logger.println(TinyGPSPlus::libraryVersion());
	logger.println(F("by Mikal Hart"));
	logger.println();

	// Initialize all the uninitialized TinyGPSCustom objects
	for (int i=0; i<4; ++i) {
		satNumber[i].begin(gps, "GPGSV", 4 + 4 * i); // offsets 4, 8, 12, 16
		elevation[i].begin(gps, "GPGSV", 5 + 4 * i); // offsets 5, 9, 13, 17
	}
}

void loop() {
  //logger.println(F("Loop"));
	// Dispatch incoming characters
	if (ss.available() > 0)
	{
		gps.encode(ss.read());

		if (totalGPGSVMessages.isUpdated()) {
			for (int i=0; i<4; ++i) {
				int no = atoi(satNumber[i].value());
				if (no >= 1 && no <= MAX_SATELLITES) {
					int elev = atoi(elevation[i].value());
					sats[no-1].active = true;
					if (sats[no-1].elevation != elev) {
						sats[no-1].elevation = elev;
						anyChanges = true;
					}
				}
			}

			int totalMessages = atoi(totalGPGSVMessages.value());
			int currentMessage = atoi(messageNumber.value());
			if (totalMessages == currentMessage && anyChanges) {
				if (linecount++ % PAGE_LENGTH == 0) {
					printHeader();
				}
				TimePrint();
				for (int i=0; i<MAX_SATELLITES; ++i) {
					logger.print(F(" "));
					if (sats[i].active) {
						IntPrint(sats[i].elevation, 2);
					} else {
						logger.print(F("   "));
					}
					sats[i].active = false;
				}
				logger.println();
				anyChanges = false;
			}
		}
	}
}

void IntPrint(int n, int len) {
	int digs = n < 0 ? 2 : 1;
	for (int i=10; i<=abs(n); i*=10) {
		++digs;
	}
	while (digs++ < len) {
		logger.print(F(" "));
	}
	logger.print(n);
	logger.print(F(" "));
}

void TimePrint() {
	if (gps.time.isValid()) {
		if (gps.time.hour() < 10) {
			logger.print(F("0"));
		}
		logger.print(gps.time.hour());
		logger.print(F(":"));
		if (gps.time.minute() < 10) {
			logger.print(F("0"));
		}
		logger.print(gps.time.minute());
		logger.print(F(":"));
		if (gps.time.second() < 10) {
			logger.print(F("0"));
		}
		logger.print(gps.time.second());
		logger.print(F(" "));
	} else {
		logger.print(F("(unknown)"));
	}
}

void printHeader() {
	logger.println();
	logger.print(F("Time     "));
	for (int i=0; i<MAX_SATELLITES; ++i) {
		logger.print(F(" "));
		IntPrint(i+1, 2);
	}
	logger.println();
	logger.print(F("---------"));
	for (int i=0; i<MAX_SATELLITES; ++i) {
		logger.print(F("----"));
	}
	logger.println();
}