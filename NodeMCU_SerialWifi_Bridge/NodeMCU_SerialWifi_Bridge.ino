// https://arduino-esp8266.readthedocs.io/en/latest/exception_causes.html

#define ESP8266_LED 5
#include "data.h"
#include <ESP8266WiFi.h>
#include <espnow.h>

#define DEBUGPRINT
#define DEV 3

//#undef DEBUGPRINT

#define BUF_LEN 32
uint8_t __attribute__((aligned(4))) rxDataBuf[BUF_LEN];
uint8_t __attribute__((aligned(4))) txDataBuf[BUF_LEN];

uint8_t* destMAC = 0;
uint8_t txcount = 0;
uint8_t rxcount = 0;

void OnDataSent(uint8_t* mac, uint8_t sendStatus)
{
  txcount++;

#ifdef DEBUGPRINT
  Serial.print("Tx sent: ");
  Serial.println(txcount);
#endif
}

void OnDataRecv(uint8_t* mac, uint8_t* dataIn, uint8_t len)
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
  if (DEV==1)
  {
    destMAC = macAddr2;
  }
  else if (DEV==2)
  {
    destMAC = macAddr1;
  }
  else
  {
    destMAC = macAddr2;
  }

  pinMode(ESP8266_LED, OUTPUT);
  digitalWrite(ESP8266_LED, HIGH);
  delay(1000);
  digitalWrite(ESP8266_LED, LOW);
  
#ifdef DEBUGPRINT
  Serial.println();
  Serial.println();
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());
#endif

  WiFi.mode(WIFI_STA); // Set device as a WiFi station
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0)
  {
    Serial.println("ERROR initializing ESP-NOW");
    return;
  }

//  for (size_t n=0; n<BUF_LEN; n++)
//  {
//    rxDataBuf[n] = 0;
//    txDataBuf[n] = 0;
//  }
  
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(destMAC, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(OnDataRecv);
  Serial.setTimeout(100);
}


void loop() {
  // put your main code here, to run repeatedly:
  char rc;
  int nBytes = 0;
  while (nBytes = Serial.available() > 0) 
  {
    size_t n = Serial.readBytes(rxDataBuf, BUF_LEN);
    esp_now_send(destMAC, rxDataBuf, n);
  }
}
