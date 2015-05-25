
#include <RF22ReliableDatagram.h>
#include <RF22.h>
#include <SPI.h>
#include <UIPEthernet.h>
#include "RestClient.h"

#define SERVER_ADDRESS 0

// Singleton instance of the radio
RF22ReliableDatagram manager(SERVER_ADDRESS, 8, 0);
RestClient client = RestClient("192.168.2.120");


void setup() {
  Serial.begin(9600);
  if (manager.init())
    Serial.println(F("init success"));
  else
    Serial.println(F("init failed"));
  client.dhcp();
  Serial.println(F("setup() done"));
}

String response;
void loop() {

  uint8_t buf[2] PROGMEM;
  if (manager.available()) {
    uint8_t len = 2;
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from)) {
      Serial.print(F("Received :"));

      response = "";
      int statusCode = client.post("/data", "POSTDATA", &response);
      Serial.print(F("Status code from server: "));
      Serial.println(statusCode);
      Serial.print(F("Response body from server: "));
      Serial.println(response);
      if (!manager.sendtoWait(buf, 2, from)) {
        //// alarm to a red led !!!
      }
    }
  }
}
