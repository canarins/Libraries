

#include <vector>
#include <string>

const IPAddress REMOTE_UDP_ADDR = IPAddress(18, 184, 137, 24);

std::vector<String> SSID_vector_default;
std::vector<String> PASS_vector_default;



const int REMOTE_UDP_PORT = 60004;
const int NTP_PORT = 123;


const int DEFAULT_COMMAND = 1;  // INSERT NEW NUM VALUE
const int NEW_ID_COMMAND = 0;

char NODE_ID[18] = "TODEFINE#";
char iccid[32] = "";


String nodeID = "";
int PROJECT_ID = 11;  // Pico


#define PACKET_STRING_LENGTH 256

void setup_parameters()
{

  SSID_vector.push_back("Ariele_hotspot_android");
  PASS_vector.push_back("pros_hotspot_ti");

  SSID_vector.push_back("Freebox-32242D");
  PASS_vector.push_back("hbk4nqmt26q3xnftqzcvwh");

  SSID_vector.push_back("Letmeknow");
  PASS_vector.push_back("letmeknow75002");
}

//TODO make functions to modify Variables


