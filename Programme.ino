/**
 * Cuve
 * 
 * gestion du niveau et de la température de la cuve d'eau (récupérateur)
 * 
 * @date  Juillet 2017
 * @see   https://docs.google.com/document/d/1D_nuetByEitFJuT3LoqX1b15HQhd-w6IJazHjmXhHUk/
 */

/**
 * Librairies
 */
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <Wire.h>

/**
 * Constantes
 */
#define I2C_ADRESS 0x20   // Adresse I2C du module
#define TEMP_ALARM 2.00F  // Alarme de température
#define Cniv3 2           // Niveau 2%
#define Cniv10 3          // Niveau 10%
#define Cniv20 4          // Niveau 20%
#define Cniv30 5          // Niveau 30%
#define Cniv40 6          // Niveau 40%
#define Cniv50 7          // Niveau 50%
#define Cniv60 8          // Niveau 60%
#define Cniv70 9          // Niveau 70%
#define Cniv80 10         // Niveau 80%
#define Cniv90 11         // Niveau 90%
#define Cniv100 12        // Niveau 100%
#define Vtemp A0          // Sonde de température de l'eau
#define Vtemp2 A1         // Sonde de température du circuit
#define Alarm 13          // LED d'alarme de température

/**
 * Variables
 */
byte levelWater;          // Niveau d'eau dans la cuve
float tempWater;          // Température de l'eau
float tempCircuit;        // Température du circuit
byte i2cCommand;          // Commande I2C reçue

/**
 * Initialisation
 */
void setup() {
  
  // Entrées
  pinMode(Cniv3, INPUT_PULLUP);
  pinMode(Cniv10, INPUT_PULLUP);
  pinMode(Cniv20, INPUT_PULLUP);
  pinMode(Cniv30, INPUT_PULLUP);
  pinMode(Cniv40, INPUT_PULLUP);
  pinMode(Cniv50, INPUT_PULLUP);
  pinMode(Cniv60, INPUT_PULLUP);
  pinMode(Cniv70, INPUT_PULLUP);
  pinMode(Cniv80, INPUT_PULLUP);
  pinMode(Cniv90, INPUT_PULLUP);
  pinMode(Cniv100, INPUT_PULLUP);
  
  // Sorties
  pinMode(Alarm, OUTPUT);

  // I2C
  Wire.begin(I2C_ADRESS);
  Wire.onReceive(i2cReceive);
  Wire.onRequest(i2cRequest);
  i2cCommand = 0;
}

/**
 * Programme principal
 */
void loop() {
  
  readWaterLevel();
  readTemperatures();
  setAlarme();
  setSleep();
}

/**
 * Indique le niveau d'eau dans la variable levelWater
 */
void readWaterLevel() {
  
  if (!digitalRead(Cniv100)) levelWater = 100;
  else if (!digitalRead(Cniv90)) levelWater = 90;
  else if (!digitalRead(Cniv80)) levelWater = 80;
  else if (!digitalRead(Cniv70)) levelWater = 70;
  else if (!digitalRead(Cniv60)) levelWater = 60;
  else if (!digitalRead(Cniv50)) levelWater = 50;
  else if (!digitalRead(Cniv40)) levelWater = 40;
  else if (!digitalRead(Cniv30)) levelWater = 30;
  else if (!digitalRead(Cniv20)) levelWater = 20;
  else if (!digitalRead(Cniv10)) levelWater = 10;
  else if (!digitalRead(Cniv3)) levelWater = 3;
  else levelWater = 0;
}

/**
 * Indique la température de l'eau dans la variable tempWater
 * et celle du circuit dans tempCircuit
 */
void readTemperatures() {
  
  int value = analogRead(Vtemp);
  tempWater = ((value * 5.0 / 1024) - 1.375) * (1000 / 22.5);
  value = analogRead(Vtemp2);
  tempCircuit = ((value * 5.0 / 1024) - 1.375) * (1000 / 22.5);
}

/**
 * Allume la led si la température de l'eau est inférieure ou égale à la consigne
 * Attention : la led est branchée en logique inversée
 */
void setAlarme() {
  
  if (tempCircuit <= TEMP_ALARM)
    digitalWrite(Alarm, LOW);
  else
    digitalWrite(Alarm, HIGH);
}

/**
 * Met en sommeil le processeur
 */
void setSleep() {
  
  // TODO
}

/**
 * Reçoit des données I2C (Ecriture)
 * 
 * Commandes :
 * - 0xFF renvoi toutes les informations à la suite (niveau d'eau, température eau, température boîtier)
 * - 0xE0 renvoi le niveau d'eau dans la cuve (1 octet)
 * - 0xD0 renvoi la température de l'eau (4 octets)
 * - 0xD1 renvoi la température du boîtier (4 octets)
 */
void i2cReceive(int datasLength) {
  
  i2cCommand = Wire.read();
}

/**
 * Envoi des données I2C suite à une demande (Lecture)
 */
void i2cRequest() {

  if (i2cCommand == 0xFF) { // Toutes les données
    Wire.write(levelWater);
    Wire_SendDouble(tempWater);
    Wire_SendDouble(tempCircuit);
  }
  else if (i2cCommand == 0xE0) // Niveau d'eau
    Wire.write(levelWater);
  else if (i2cCommand == 0xD0) // Température de l'eau
    Wire_SendDouble(tempWater);
  else if (i2cCommand == 0xD1) // Température du boîtier
    Wire_SendDouble(tempCircuit);
  else  // Commande inconnue
    Wire.write(0xFF);
    
}

/**
 * Envoi un double par la liaison I2C
 */
void Wire_SendDouble(float f){
  
    union Sharedblock
    {
        byte octet[4];
        float data;
    } datas;
    datas.data = f;
    Wire.write(datas.octet, 4);
}
 
