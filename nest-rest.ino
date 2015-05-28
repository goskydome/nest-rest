
#include <RF22ReliableDatagram.h>
#include <RF22.h>
#include <SPI.h>
#include <avr/pgmspace.h>

#include <EtherCard.h>


#define SERVER_ADDRESS 0

RF22ReliableDatagram manager(SERVER_ADDRESS, 8, 0);

static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

byte Ethernet::buffer[700];

const char skydome[] PROGMEM = "api.skydome.io";

void setup() {
  Serial.begin(9600);
  if (manager.init())
    Serial.println(F("init success"));
  else
    Serial.println(F("init failed"));

  if (ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));

  if (!ether.dnsLookup(skydome))
    Serial.println(F("DNS failed"));

  ether.hisport = 8080;

  Serial.println(F("setup() done"));
}

static void my_callback (byte statusCode, word off, word len) {
  Ethernet::buffer[off + 300] = 0;
  Serial.println((const char*) Ethernet::buffer + off);
}

const uint8_t charOffset PROGMEM = 48;
void loop() {
  ether.packetLoop(ether.packetReceive());

  uint8_t buf[2] PROGMEM;
  if (manager.available()) {
    uint8_t len = 2;
    uint8_t from;

    if (manager.recvfromAck(buf, &len, &from)) {

      const char message[] PROGMEM = {'{', '"', 'i', 'd', '"', ':',   '"', 'a', 'b', 'c', 'd', 'e', 'f', 'g',   '"', ',', /*{"id":"abcdefg",*/
                                      '"', 'f', 'r', 'o', 'm',   '"', ':',  '"', (from + charOffset),   '"', ',', /*"from":"1",*/
                                      '"', 't', 'y', 'p', 'e',   '"', ':',   '"', (buf[0] + charOffset),   '"', ',', /*"type":"7",*/
                                      '"', 'v', 'a', 'l', 'u', 'e',   '"', ':',   '"', (buf[1] + charOffset),   '"', '}' /*"value":"0"*/
                                     };

      ether.httpPost(PSTR("/data"), skydome, PSTR("Authorization: Token asdaasd"), message, my_callback);
    }
  }
}
