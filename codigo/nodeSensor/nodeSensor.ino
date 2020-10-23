/* Projeto SoftCool
 * Desenvolvido durante o Desafio IoT 2020 da @Tech
 * Autor: João Tartaglia
 * github.com/jvtartaglia
 * 
 * Bibliotecas utilizadas
 * SPI.h - https://www.arduino.cc/en/reference/SPI
 * DHT- https://github.com/adafruit/DHT-sensor-library
 * RF24 - https://tmrh20.github.io/RF24/index.html
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DHT.h>

#define DHTPIN0 2
#define DHTTYPE DHT11
#define ventilador 8
#define switchChaveModo 4
#define switchChaveLiga 3
#define pinoChipEnable 9
#define pinoChipSelect 10

const byte enderecoPipe[6] = "00001";

/*  array para armazenar as variaveis controladas e transmitidas
    dados[0] = temperatura
    dados[1] = umidade
    dados[2] = modo de operacao (0 = automatico / 1 = manual)
    dados[3] = controle do ventilador (0 = desligado / 1 = ligado)
*/
float dados[4];

RF24 radio(pinoChipEnable, pinoChipSelect);
DHT dht11(DHTPIN0, DHTTYPE);

void setup() {

  pinMode(ventilador, OUTPUT);
  pinMode(switchChaveModo, INPUT_PULLUP);
  pinMode(switchChaveLiga, INPUT_PULLUP);

  dht11.begin();

  radio.begin();
  delay(1000);
  radio.setDataRate( RF24_250KBPS );
  delay(1000);
  radio.setRetries(3, 5);
  delay(1000);
  radio.openWritingPipe(enderecoPipe);
  delay(1000);
  radio.setPALevel(RF24_PA_MIN);
  delay(1000);
  radio.stopListening();
  delay(1000);
}

void loop() {

  dados[2] = digitalRead(switchChaveModo);
  dados[3] = digitalRead(switchChaveLiga);
  
  // modo automatico
  while (dados[2] == 0) {

    dados[0] = dht11.readTemperature();
    dados[1] = dht11.readHumidity();

    if (isnan(dados[0]) || isnan(dados[1])) {
      dados[0] = 255;
      dados[1] = 255;
    }

    if (dados[0] >= 20) {
      digitalWrite(ventilador, HIGH);
    } else {
      digitalWrite(ventilador, LOW);
    }

    radio.write(&dados, sizeof(dados));

    delay(2000);

    dados[2] = digitalRead(switchChaveModo);
    dados[3] = digitalRead(switchChaveLiga);
  }

  // modo manual
  while (dados[2] == 1) {

    dados[0] = dht11.readTemperature();
    dados[1] = dht11.readHumidity();
    
    // checa se o retorno do DHT e valido
    if (isnan(dados[0]) || isnan(dados[1])) {
      dados[0] = 255;
      dados[1] = 255;
    }
    
    // checa a chave switch liga/desliga
    if (dados[3] == 1) {
      digitalWrite(ventilador, HIGH);
    } else {
      digitalWrite(ventilador, LOW);
    }

    radio.write(&dados, sizeof(dados));

    delay(2000);

    dados[2] = digitalRead(switchChaveModo);
    dados[3] = digitalRead(switchChaveLiga);
  }

}
