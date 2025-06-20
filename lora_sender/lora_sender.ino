#include <LoRa.h>
#include <SPI.h>
 
#define ss 5
#define rst 14
#define dio0 2
 
// Pines para los 7 botones (sin conflicto con LoRa)
#define BTN1  4
#define BTN2  12
#define BTN3  13
#define BTN4  15
#define BTN5  25
#define BTN6  26
#define BTN7  27

int counter = 0;

// Estados previos para detección de flanco
bool lastBtnState[7] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

void setup() 
{
  Serial.begin(115200); 
  while (!Serial);
  Serial.println("LoRa Sender");
 
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module
  
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");

  // Configura los pines de los botones como entrada con pull-up
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);
  pinMode(BTN5, INPUT_PULLUP);
  pinMode(BTN6, INPUT_PULLUP);
  pinMode(BTN7, INPUT_PULLUP);
}
 
void loop() 
{
  // Revisa cada botón y envía un mensaje si se presiona
  int btnPins[7] = {BTN1, BTN2, BTN3, BTN4, BTN5, BTN6, BTN7};
  for (int i = 0; i < 7; i++) {
    bool btnState = digitalRead(btnPins[i]);
    if (lastBtnState[i] == HIGH && btnState == LOW) { // flanco descendente
      // Envía mensaje LoRa
      LoRa.beginPacket();
      LoRa.print("Boton ");
      LoRa.print(i+1);
      LoRa.print(" presionado");
      LoRa.endPacket();
      Serial.print("Mensaje enviado: Boton ");
      Serial.println(i+1);
      delay(10); // anti-rebote reducido a 10ms
    }
    lastBtnState[i] = btnState;
  }
  delay(10); // pequeño delay para evitar rebotes
}