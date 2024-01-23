// https://arduino-esp8266.readthedocs.io/en/latest/exception_causes.html

#define ESP8266_LED 5
#include "data.h"
#include <WiFi.h>
#include <esp_now.h>

#include <Adafruit_NeoPixel.h>
// How many internal neopixels do we have? some boards have more than one!
#define NUMPIXELS        1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

uint32_t colors[] = {0xFF0000, 0x00FF00, 0x0000FF};
uint32_t color_ind = 0;

#define DEV 1
//#define DEBUGPRINT

#define BUF_LEN 32 // max is 250 bytes
uint8_t __attribute__((aligned(4))) rxDataBuf[BUF_LEN];
uint8_t __attribute__((aligned(4))) txDataBuf[BUF_LEN];

uint8_t* destMAC = 0;
uint8_t txcount = 0;
uint8_t rxcount = 0;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  txcount++;

#ifdef DEBUGPRINT
  Serial.print("Tx sent: ");
  Serial.println(txcount);
#endif
}

void OnDataRecv(const uint8_t * mac, const uint8_t *dataIn, int len)
{
  rxcount++;
#ifdef DEBUGPRINT
    Serial.println("rx: ");
    Serial.println(len);
#endif
    Serial.write(dataIn, len);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(3000);

#if defined(NEOPIXEL_POWER)
  // If this board has a power control pin, we must set it to output and high
  // in order to enable the NeoPixels. We put this in an #if defined so it can
  // be reused for other boards without compilation errors
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
#endif

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20); // not so bright
  // set color to red
  pixels.fill(0xFF0000);
  pixels.show();
  delay(500); // wait half a second
  
  if (DEV==1)
  {
    destMAC = macAddr2;
  }
  else if (DEV==2)
  {
    destMAC = macAddr1;
  }

  WiFi.mode(WIFI_STA); // Set device as a WiFi station
  delay(1000); // wait a second
  
//#ifdef DEBUGPRINT
  Serial.println();
  Serial.println();
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());
//#endif

// set color to green
  pixels.fill(0x00FF00);
  pixels.show();
  delay(500); // wait half a second

//  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ERROR initializing ESP-NOW");
    return;
  }

//  for (size_t n=0; n<BUF_LEN; n++)
//  {
//    rxDataBuf[n] = 0;
//    txDataBuf[n] = 0;
//  }

// Register peer
  memcpy(peerInfo.peer_addr, destMAC, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
//  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  
  Serial.setTimeout(100);

  // set color to blue
  pixels.fill(0x0000FF);
  pixels.show();
  delay(500); // wait half a second
}


void loop() {
  // put your main code here, to run repeatedly:
  char rc;
  int nBytes = 0;
  while (nBytes = Serial.available() > 0) 
  {
    size_t n = Serial.readBytes(rxDataBuf, BUF_LEN);
    esp_now_send(destMAC, rxDataBuf, n);
    
    pixels.fill(colors[color_ind]);
    pixels.show();
    ++color_ind;
    color_ind %= 3;
  }
}
