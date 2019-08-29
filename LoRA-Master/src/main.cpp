// Pin diagram
// NodeMCU    LoRA
// 3V3        3V3
// GND        GND
// D2         DIO0
// D3         RESET
// D5         SCK
// D6         MISO
// D7         MOSI
// D8         NSS
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
  LoRa.setPins(15,0,4);

  Serial.println("LoRa Receiver");

  while(!LoRa.begin(433E6)) {
    Serial.print("*");
    delay(1000);
  }
  delay(100);
  Serial.println("");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
