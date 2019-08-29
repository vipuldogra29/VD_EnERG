// Pin diagram
// Nano LoRA
// 
// D2   DIO0
// D9   RST
// D10  NSS
// D11  MOSI
// D12  MISO
// D13  SCK
//      
//      PMS
// 5V   VIN
// GND  GND
// TX   RX
// RX   TX
//      
//      HTU
// A4   SDA
// A5   SCL
//     
//      RTC
// A4   SDA
// A5   SCL

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h> //I2C library for RTC
#include <DS3231M.h>   

DateTime dt(__DATE__,__TIME__);
DS3231M_Class DS3231M;
  
boolean readPMSdata(Stream *s);
struct pms5003data
{
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
struct pms5003data data;

DateTime now;

void setup() {
  Serial.begin(9600);

  DS3231M.begin();
  DS3231M.adjust(dt);  

  while(!LoRa.begin(433E6)) {
    Serial.print("*");
    delay(500);
  }
  LoRa.setTxPower(20);

  Serial.println("Initialization done.");
  delay(100);
}

void loop() {
  
  DateTime now = DS3231M.now();  // get the current time

  if (readPMSdata(&Serial)){
    //reading data was successful!

    LoRa.beginPacket(); //Start sending
    
    digitalWrite(LED_BUILTIN, HIGH);
    LoRa.println("---------------------------------------");
    LoRa.println(data.particles_03um);
    LoRa.println(data.particles_05um);
    LoRa.println(data.particles_10um);
    LoRa.println(data.particles_25um);
    LoRa.println(data.particles_50um);
    LoRa.println(data.particles_100um);
    LoRa.print(now.second());LoRa.print('-');
    LoRa.print(now.minute());LoRa.print('-');
    LoRa.print(now.hour());LoRa.print('-');
    LoRa.print(now.day());LoRa.print('-');
    LoRa.print(now.month());LoRa.print('-');
    LoRa.print(now.year());LoRa.println('|');
    LoRa.println("---------------------------------------");
    LoRa.endPacket();
    
  }
  else{
    LoRa.beginPacket(); //Start sending
    LoRa.print("No data from slave");
    LoRa.endPacket();
  }
  delay(500);
}

boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }

/*   debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println(); */
  
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}
