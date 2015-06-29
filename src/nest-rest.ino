
#include <RF22ReliableDatagram.h>
#include <RF22.h>
#include <SPI.h>
#include <avr/pgmspace.h>
#include <EtherCard.h>

#define RF_SERVER_ADDRESS 0

RF22ReliableDatagram manager(RF_SERVER_ADDRESS, 8, 0);

static byte mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };

byte Ethernet::buffer[700];

const char skydome[] PROGMEM = "api.skydome.io";
//const char skydome[] PROGMEM = "192.168.2.20";'

void setup() {
  Serial.begin(9600);
  if (manager.init()){
  //  Serial.println(F("init success"));
  }
  else {
   // Serial.println(F("init failed"));
  }
  
  if (ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0) {
 //   Serial.println(F("Failed to access Ethernet controller"));
}
  if (!ether.dhcpSetup()){
 //   Serial.println(F("DHCP failed"));
}

  if (!ether.dnsLookup(skydome)){
 //   Serial.println(F("DNS failed"));
}
  ether.hisport = 8080;

//  Serial.println(F("setup() done"));
}

static void responseCallback (byte statusCode, word off, word len) {
  Ethernet::buffer[off + 300] = 0;
 // Serial.println((const char*) Ethernet::buffer + off);
}

const uint8_t charOffset PROGMEM = 48;
void loop() {
  ether.packetLoop(ether.packetReceive());

  uint8_t data[2] PROGMEM;
  if (manager.available()) {
    uint8_t len = 2;
    uint8_t from;

    if (manager.recvfromAck(data, &len, &from)) {
         
      Serial.print("got data[0]:");
      Serial.println(data[0]);
        
      Serial.print("got data[1]:");
      Serial.println(data[1]);
    char fromChar = from;
    char data0 = data[0];
    char data1 = data[1];
       const char *message = '1323';
    //  char message[] PROGMEM = {'{','"','a','"',':','"',from+charOffset,'"',',','"','t','"',':','"',data[0]+charOffset,'"',',',
    //                                '"', 'v', '"',':','"',data[1]+charOffset,'"', '}' 
    //                           };
    
      ether.httpPost(PSTR("/telemetry/plant/id/yaman"), skydome, PSTR("Authorization: Token asdaasd"), message, NULL);
    }
  }
}
