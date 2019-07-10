#define RXD2 12
#define TXD2 13
#include "heltec.h"
//#include "images.h"
#define BAND    433E6  //you can set band here directly,e.g. 868E6,915E6
unsigned int counter = 0;
String rssi = "RSSI --";
String packSize = "--";
String packet ;
String Data = "";

void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
 
  Heltec.display->init();
  Heltec.display->flipScreenVertically();  
  Heltec.display->setFont(ArialMT_Plain_10);
//  logo();
  delay(300);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Heltec.LoRa Initial success!");
  Heltec.display->display();
//  delay(1000);
}
 
void loop() { //Choose Serial1 or Serial2 as required
  while (Serial2.available()) {
    char character=char(Serial2.read());
    Data.concat(character);
    if(character == '\n'){
      Serial.print(Data);
      Heltec.display->clear();
      Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
      Heltec.display->setFont(ArialMT_Plain_10);
      
      Heltec.display->drawString(0, 0, "Packet: ");
      Heltec.display->drawString(0, 10, Data  );
      Heltec.display->display();
      // send packet
      LoRa.beginPacket();
      LoRa.print(Data);
      LoRa.endPacket();

      Data ="";
      
    }
    
  }
}
