// Declaració de les llibreries necessaries
#include <SoftwareSerial.h>
#include <Wire.h>
#include "Arduino.h"
#include "ADXL345.h"
#include "I2Cdev.h"
#include "MAX30105.h"
#include "spo2_algorithm.h"

// Definicions del programa
#define BT_SERIAL_TX 8      // Communicació amb el mòdul Bluetooth al pin TXD.
#define BT_SERIAL_RX 9      // Communicació amb el mòdul Bluetooth al pin RXD.
#define SERIAL_BAUD 9600    // Baudrate de la connexió serial del dispositiu
#define BLE_BAUD 9600       // Baudrate de la connexió BLE del dispositiu

// Inicialització dels pins analogics 
int FSR_1 = 0;     // Sensor FSR_1 Connectat al pin a0
int FSR_2 = 1;     // Sensor FSR_1 Connectat al pin a1
int FSR_3 = 2;     // Sensor FSR_1 Connectat al pin a2
int NTC = 3;       // Sensor NTC Connectat al pin a3

// Variables globals i objectes
SoftwareSerial BluetoothSerial(BT_SERIAL_TX, BT_SERIAL_RX); // Inicialització del port SoftwareSerial.
int16_t adxlAx, adxlAy, adxlAz;   // Variables XYZ necessaries pel sensor ADXL345.
ADXL345 adxl;                     // Inicialització de l'objecte de l'accelerometre. 
MAX30105 particleSensor;          // Inicialització de l'objecte de pulsioxometre

const unsigned long eventTime_NTC = 10000;        // Tasca 1: Lectura del sensor NTC cada 1000 ms. (10 lectures per segon).
const unsigned long eventTime_MAX30102 = 6000;  // Tasca 5: Lectura del sensor MAX30102 cada 60000 ms. (1 lectura per minut).
const unsigned long eventTime_ADXL345 = 1000;     // Tasca 6: Lectura del sensor ADXL345 cada 100ms. (10 lectures per segon).

unsigned long previousTime_NTC = 0;        // Tasca 1: Lectura del sensor NTC cada 1000 ms. (1 lectura per segon).
unsigned long previous_MAX30102 = 0;  // Tasca 5: Lectura del sensor MAX30102 cada 60000 ms. (1 lectura per minut).
unsigned long previous_ADXL345 = 0;     // Tasca 6: Lectura del sensor ADXL345 cada 100ms. (10 lectures per segon).

void setup() {
  Serial.begin(SERIAL_BAUD);
  BluetoothSerial.begin(BLE_BAUD);

  adxl.init(); // Inicialització de l'accelerometre


  /* NOMÉS EN CAS DE FER SERVIR Arduino Leonardo*/
  /* 
  // Inicialitzem el pulsioxometre
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Utilitza el port I2C per defecte, a una velocitat de 400 kHz
  {
    Serial.println("No s'ha trobat el sensor MAX30105. Comprova el cablejat / alimentació.");
    while (1);
  }
  
  byte ledBrightness = 60; //Opcions: 0=Off to 255=50mA
  byte sampleAverage = 4; //Opcions: 1, 2, 4, 8, 16, 32
  byte ledMode = 3; //Opcions: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Opcions: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Opcions: 69, 118, 215, 411
  int adcRange = 4096; //Opcions: 2048, 4096, 8192, 16384
  
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configurem el sensor amb les variables recomanades pel fabricant.
  */
  
  // Només en el cas que estiguem fent servir la placa Arduino UNO
  randomSeed(analogRead(0)); 
  
}

void loop() {
  // Variables de funció
  unsigned long currentTime = millis(); // Temps des que s'ha inicialitzat l'Arduino
  int fsr_1_read = analogRead(FSR_1); // Tasca 2: Lectura del sensor FSR_1 quan sigui superior a 20g (valor 200).
  int fsr_2_read = analogRead(FSR_2); // Tasca 3: Lectura del sensor FSR_2 quan sigui superior a 20g (valor 200).
  int fsr_3_read = analogRead(FSR_3); // Tasca 4: Lectura del sensor FSR_3 quan sigui superior a 20g (valor 200).
  int ntc_read = 0;
 
  // Tasca 1 (1 Lectura per segon) 
  if ( currentTime - previousTime_NTC >= eventTime_NTC ){
    // Tot el que es faci aqui dins, s'executarà cada 1 segon.
    
    ntc_read = analogRead(NTC); 
    double temp_Kelvin = log(10000.0 * ((1024.0 / ntc_read - 1)));
    temp_Kelvin = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * temp_Kelvin * temp_Kelvin )) * temp_Kelvin );
    float temp_Celcius = temp_Kelvin - 273.15;            // Convertim els Kelvin a Celcius
    float temp_Fahrenheit = (temp_Celcius * 9.0)/ 5.0 + 32.0;      // Convertim els Celcius a Fahrenheit
    Serial.println(String("NTC: ") + temp_Celcius);
    
    BluetoothSerial.println(String("NTC: ") + temp_Celcius);
    previousTime_NTC = currentTime;
  }
  
  // Tasca 5: Lectura del sensor MAX30102 cada 60000 ms. (1 lectura per minut).
  if ( currentTime - previous_MAX30102 >= eventTime_MAX30102 ){
    // Tot el que es faci aqui dins, s'executarà cada 1 minut. 

    // Aquest codi comentat només funciona a la placa Arduino Leonardo degut a superar el 80% d'utilització de la memòria SRAM (2KB) de l'ATmega328 que porta la UNO
    // En cas d'utilitzar la ATmega32u4 que porta 2,5KB de memoria SRAM es podria fer lectura correcte de SPO2 i HR
    /*
    // Variables per al pulsioximetre
    // Arduino Uno no té prou SRAM per emmagatzemar 100 mostres de dades de leds IR i dades de leds vermells en format de 32 bits
    // Per resoldre aquest problema, es truncarà en 16bit MSB les dades mostrejades. Les mostres es converteixen en dades de 16 bits.
    uint16_t irBuffer[100]; // Dades del sensor LED d’infrarojos
    uint16_t redBuffer[100];  // Dades del sensor LED vermell
    
    int32_t bufferLength; // Longitud de les dades
    int32_t spo2; // Valor SPO2 
    int8_t validSPO2; // Indicador per mostrar si el càlcul SPO2 és vàlid
    int32_t heartRate; // Valor de la freqüència cardíaca
    int8_t validHeartRate; // Indicador per mostrar si el càlcul de la freqüència cardíaca és vàlid
  
    bufferLength = 100; // Mida del buffer de 100 lectures mesurades en 4 segons de mostres a 25sps

    // Llegeix les primeres 100 mostres i determia l'abast del senyal
    for (byte i = 0 ; i < bufferLength ; i++){
      while (particleSensor.available() == false) // Hi ha noves dades?
        particleSensor.check(); // Mira al sensor si hi ha noves dades
      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); // Hem acabat amb aquesta mostra, així que mirem la mostra següent 
    }
    
    // Calcula la freqüència cardíaca i SpO2 després de les primeres 100 mostres (primers 4 segons de mostres)
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

    // Continuem prenent mostres del sensor MAX30102. La freqüència cardíaca i SpO2 es calculen cada 1 segon
    while(1){
      //Descartem els primers 25 conjunts de mostres de la memòria i desplaçem els darrers 75 conjunts de mostres a la part superior  
      for (byte i = 25; i < 100; i++){
        redBuffer[i - 25] = redBuffer[i];
        irBuffer[i - 25] = irBuffer[i];
      }
      
      // Agafem les 25 mostres abans de calcular la freqüència cardíaca.
      for (byte i = 75; i < 100; i++) {
        while (particleSensor.available() == false) // Hi ha noves dades?
          particleSensor.check(); // Mira al sensor si hi ha noves dades
  
        redBuffer[i] = particleSensor.getRed();
        irBuffer[i] = particleSensor.getIR();
        particleSensor.nextSample(); // Hem acabat amb aquesta mostra, així que mirem la mostra següent 

        // Mostrem les dades per pantalla
        Serial.print(String("BPM: ") + validHeartRate); Serial.print(F("\t"));
        Serial.println(String("SPO2: ") + validSPO2);
        BluetoothSerial.print(String("BPM: ") + validHeartRate); BluetoothSerial.print(F("\t"));
        BluetoothSerial.println(String("SPO2: ") + validSPO2);
      }
  
      // Després de recollir 25 noves mostres, recalculem HR i SP02
      maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    }
    */

    /*Només en cas de fer servir Arduino UNO*/
    int validHeartRate = random(55, 120);
    int validSPO2 = random(92, 99);
    // Mostrem les dades per pantalla
    Serial.print(String("BPM: ") + validHeartRate); Serial.print(F("\t"));
    Serial.println(String("SPO2: ") + validSPO2);
    BluetoothSerial.print(String("BPM: ") + validHeartRate); BluetoothSerial.print(F("\t"));
    BluetoothSerial.println(String("SPO2: ") + validSPO2);
    
    previous_MAX30102 = currentTime;
  }
  
  // Tasca 6: Lectura del sensor ADXL345 cada 100ms. (10 lectures per segon).
  if ( currentTime - previous_ADXL345 >= eventTime_ADXL345 ){
    // Tot el que es faci aqui dins, s'executarà cada 1 100ms. 
    
    adxl.getAcceleration(&adxlAx, &adxlAy, &adxlAz);
    Serial.print(String("X: ") +adxlAx); Serial.print(F("\t"));
    Serial.print(String("Y: ") +adxlAy); Serial.print(F("\t"));  
    Serial.println(String("Z: ") +adxlAz);
    
    BluetoothSerial.print(String("X: ") + adxlAx); BluetoothSerial.print(F("\t"));
    BluetoothSerial.print(String("Y: ") + adxlAy); BluetoothSerial.print(F("\t"));  
    BluetoothSerial.println(String("Z: ") + adxlAz);
    previous_ADXL345 = currentTime;
  }
  
  
  if (fsr_1_read > 200) {
    Serial.println(String("FSR_1: ") + fsr_1_read);
    BluetoothSerial.println(String("FSR_1: ") + fsr_1_read);
  }
  delay(500);

}
