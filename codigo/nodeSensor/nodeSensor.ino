#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DHT.h>

#define DHTPIN0 7
#define DHTTYPE DHT11
#define ventilador 8
#define switchChaveModo 2
#define switchChaveLiga 3
#define CE 9
#define CSN 10

const byte endereco[6] = "00001";
float dados[4];

RF24 radio(CE, CSN); // CE, CSN
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
  radio.openWritingPipe(endereco);
  delay(1000);
  radio.setPALevel(RF24_PA_MIN);
  delay(1000);
  radio.stopListening();
  delay(1000);
}

void loop() {

  dados[2] = digitalRead(switchChaveModo);
  dados[3] = digitalRead(switchChaveLiga);

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

  while (dados[2] == 1) {

    dados[0] = dht11.readTemperature();
    dados[1] = dht11.readHumidity();

    if (isnan(dados[0]) || isnan(dados[1])) {
      dados[0] = 255;
      dados[1] = 255;
    }

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
