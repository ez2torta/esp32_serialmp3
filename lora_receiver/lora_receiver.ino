#include <LoRa.h>
#include <SPI.h>
#include <YX5300_ESP32.h>

/*

  --- Basic playback of track ---
  
  Establishes connection with the YX5300 MP3 Module and plays the first 
  track on the device, looping through every song on the SD card.
  
  The circuit:
  - MP3 module TX connected to pin 16 (RX1)
  - MP3 module RX connected to pin 17 (TX1)
  
  Other notes:
  1. The SD card must have at least one song for playback. 
  Consider using the example music provided on GitHub:
  https://github.com/bluejunimo/YX5300_ESP32/music
  
  
  created 26 Apr 2024
  modified 26 Apr 2024
  by bluejunimo

*/
// *make sure the RX on the YX5300 goes to the TX on the ESP32, and vice-versa
#define RX 26
#define TX 27
// parece que estos pines están al reves
// al menos así los tuve que conectar en el módulo serial de mp3 que compré

// Lora Pins
#define ss 5
#define rst 14
#define dio0 2
// hay que conectar todos los lora pins, ver ejemplo en
// https://randomnerdtutorials.com/esp32-lora-rfm95-transceiver-arduino-ide/


YX5300_ESP32 mp3;  // the mp3 object
int counter;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  mp3 = YX5300_ESP32(Serial2, RX, TX);
  Serial.println("Iniciando MP3");
  mp3.enableDebugging();
  Serial.println("LoRa Receiver");

  LoRa.setPins(ss, rst, dio0);  //setup LoRa transceiver module

  while (!LoRa.begin(433E6))  //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  int packetSize = LoRa.parsePacket();  // try to parse packet
  if (packetSize) {
    String LoRaData = "";
    while (LoRa.available())  // read packet
    {
      LoRaData += LoRa.readString();
    }
    Serial.print("Received packet: ");
    Serial.println(LoRaData);
    // Buscar el número de botón en el mensaje
    int boton = -1;
    int idx = LoRaData.indexOf("Boton ");
    if (idx != -1) {
      int start = idx + 6; // después de "Boton "
      int end = LoRaData.indexOf(' ', start);
      if (end == -1) end = LoRaData.length();
      String numStr = LoRaData.substring(start, end);
      boton = numStr.toInt();
    }
    if (boton >= 1 && boton <= 8) {
      Serial.print("Ejecutando acción para botón: ");
      Serial.println(boton);
      mp3.playTrack(boton); // Reproduce la pista correspondiente al botón
    } else {
      Serial.println("Mensaje no reconocido o fuera de rango");
    }
  }
  delay(2); // Pequeño delay para evitar sobrecalentamiento
}