

#include <vector>
#include <string>

const IPAddress REMOTE_UDP_ADDR = IPAddress(18, 184, 137, 24);

std::vector<String> SSID_vector_default;
std::vector<String> PASS_vector_default;



const int REMOTE_UDP_PORT = 60004;
const int NTP_PORT = 123;


const int DEFAULT_COMMAND = 1;  // INSERT NEW NUM VALUE
const int NEW_ID_COMMAND = 0;
const int CONFIGURATION_COMMAND = 2;

// Define a macro to convert __DATE__ and __TIME__ macros into a string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Create the VERSION macro by concatenating "Canarin_FW" with __DATE__ and optionally __TIME__

String removeSpaces(const char* str) {
  String result = ""; // Create an empty string to store the result
  while (*str != '\0') { // Loop until the end of the string
    if (*str != ' ') { // If the current character is not a space
      result += *str; // Append it to the result string
    }
    str++; // Move to the next character
  }
  return result;
}

#define VERSION "Canarin_FW_"

String versionString = String(VERSION) + removeSpaces(__DATE__);

char NODE_ID[16] = "TODEFINE#";
char PROJECT_ID[4] = "12";  // Pico

int LAST_VAR_UPDATE = 0;//TODO Check it is updated with each VAR UPDATE

/*

ID	CONFIG VAR	Units DEFAULT VALUES
0	int interval	ms	1
1	int isCloudMode	0/1	0
2	int pmSamplingTime	ms	5000
3	int gpsON	0/1	1
4	int cell_location	0/1	2
5	int resetModem	0/1	0
6	int doSleep	0/1	0
7	int batterySleepTime	ms	2400 = 30 *60
8	int remoteControl	0/1	0
9	int ntpTime	0/1	1
10	int exactMin	0/1	1
11	int cloud_time	s	120
12	int erase_fs	0/1	0
13	int resetGPS	0/1	0
14	int updateGPSRate	0/1	0
15	int restoreVars	0/1	0
16	int cell_location_timeout	s	15
			
17	int hard_reset	0/1	0
18	Char version_to_install	-	“Init”
19	Sample size	unit	10
20	Char wifi_connect	-	{iccid:pass}
21	Char alert_SMS	-	{“+33630807628”}
22	Char error_log_email	-	{canarinssensors@gmail.com}

			



int INTERVAL = 1;
bool ISCLOUDMODE = false;
int PMSAMPLINGTIME = 5000;
bool GPSON = true;
bool CELL_LOCATION = false; //TODO better
bool RESETMODEM = false; 
bool DOSLEEP = false;
int BATTERYSLEEPTIME = 2400;
bool REMOTECONTROL = false;
bool NTPTIME = true;
bool EXACTMIN = true;
int CLOUD_TIME = 120;
bool ERASE_FS = false;
bool RESETGPS = false;
bool UPDATEGPSRATE = false;
bool RESTOREVARS = false;
int CELL_LOCATION_TIMEOUT = 15;
bool HARD_RESET = false;
char VERSION[32] = {'\0'};

char WIFI_CONECT[512] = {'\0'};
char ALERT_SMS[512] = {'\0'};
char ERROR_LOG_EMAIL[512] = {'\0'};
*/




#define PACKET_STRING_LENGTH 256

void setup_parameters()
{



  SSID_vector_default.push_back("Ariele_hotspot_android");
  PASS_vector_default.push_back("pros_hotspot_ti");

  //SSID_vector.push_back("Freebox-32242D");
  //PASS_vector.push_back("hbk4nqmt26q3xnftqzcvwh");

  //SSID_vector.push_back("Letmeknow");
  //PASS_vector.push_back("letmeknow75002");


}

//TODO make functions to modify Variables


