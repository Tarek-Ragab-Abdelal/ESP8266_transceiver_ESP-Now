/*
 IMPORTANT NOTE: This is ESP8266 one to one two way communication
*/

#include <ESP8266WiFi.h>
#include <espnow.h>

//%################################################################################################//
// Structure example to receive data
// It must match the struct sent initially
typedef struct struct_message
{
  char header[64];
  char body[128];
} struct_message;
//%################################################################################################//

//^################################################################################################//
struct_message rec_packet;
// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
  memcpy(&rec_packet, incomingData, sizeof(rec_packet));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println("Header: ");
  Serial.println(rec_packet.header);
  Serial.println("Body: ");
  Serial.println(rec_packet.body);
}
//^###############################################################################################//

//$################################################################################################//
// The mac address that data will be sent to
uint8_t target_mac_add[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// The mac address of the current board
uint8_t my_mac_add[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

struct_message sen_packet;
// Period of data transmitting
unsigned long lastTime = 0;
unsigned long timerDelay = 2000; // send readings timer

// Callback function that will be executed when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
  Serial.print("Last rec_packet Send Status: ");
  if (sendStatus == 0)
  {
    Serial.println("Delivery success");
  }
  else
  {
    Serial.println("Delivery fail");
  }
}
// Transmitting data
void send_packet()
{
  // Set values to be sent
  strcpy(sen_packet.header, "This is a Header");
  strcpy(sen_packet.body, "This is a test body data - to be replaced with any  data");

  // Send a packet via ESP-NOW
  esp_now_send(target_mac_add, (uint8_t *)&sen_packet, sizeof(sen_packet));
  lastTime = millis();
}
//$################################################################################################//

//*################################################################################################//
void initiallize()
{
  if (esp_now_init() != 0)
  {
    Serial.println("ESP-Now Init Success");
  }
  else
  {
    Serial.println("ESP-Now Init Failed");
    ESP.restart();
  }
}
//*################################################################################################//

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // Init ESP-NOW
  initiallize();
  // Set the role of the ESP to be a receiver
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  // Set the callback functions
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(target_mac_add, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    send_packet();
  }
}