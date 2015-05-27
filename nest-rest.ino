
#include <RF22ReliableDatagram.h>
#include <RF22.h>
#include <SPI.h>
#include <UIPEthernet.h>
#include "RestClient.h"
#include <avr/pgmspace.h>

#define SERVER_ADDRESS 0

// Singleton instance of the radio
RF22ReliableDatagram manager(SERVER_ADDRESS, 8, 0);
RestClient client = RestClient("192.168.2.120", 8080);

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
      response = "";

      char message[] = {'{', '"', 'i', 'd', '"', ':',   '"', 'a', 'b', 'c', 'd', 'e', 'f', 'g',   '"', ',', /*{"id":"abcdefg",*/
                        '"', 'f', 'r', 'o', 'm',   '"', ':',  '"', (from + 48),   '"', ',', /*"from":"1",*/
                        '"', 't', 'y', 'p', 'e',   '"', ':',   '"', (buf[0] + 48),   '"', ',', /*"type":"7",*/
                        '"', 'v', 'a', 'l', 'u', 'e',   '"', ':',   '"', (buf[1] + 48),   '"', '}' /*"value":"0"*/
                       };

      client.setHeader("Authorization: Token xk2e2k245h5lsa");
      int statusCode = client.post("/data", message, &response);
    }
  }
}
