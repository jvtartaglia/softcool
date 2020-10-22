/* Projeto SoftCool
   Desenvolvido durante o Desafio IoT 2020 da @Tech
   Autor: João Tartaglia
   github.com/jvtartaglia

   Bibliotecas utilizadas
   SPI.h - https://www.arduino.cc/en/reference/SPI
   LiquidCrystal- https://www.arduino.cc/en/Reference/LiquidCrystal
   RF24 - https://tmrh20.github.io/RF24/index.html
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal.h>

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

byte caractereGrau[8] = { B00001100,
                          B00010010,
                          B00010010,
                          B00001100,
                          B00000000,
                          B00000000,
                          B00000000,
                          B00000000,
                        };

RF24 radio(pinoChipEnable, pinoChipSelect);

/*  atencao aos pinos utilizados durante a conexao do LCD
    parametros = (RS, E, D4, D5, D6, D7)
*/
LiquidCrystal lcd(A0, A5, A4, A3, A2, A1);

void setup() {

  // configuracoes iniciais do LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(0, caractereGrau);
  lcd.setCursor(0, 0);
  lcd.print("Inicializando...");

  // configuracoes iniciais do nRF24l01
  radio.begin();
  delay(1000);
  radio.setDataRate( RF24_250KBPS );
  delay(1000);
  radio.setRetries(3, 5);
  delay(1000);
  radio.openReadingPipe(1, enderecoPipe);
  delay(1000);
  radio.setPALevel(RF24_PA_MIN);
  delay(1000);
  radio.startListening();
  delay(1000);
  lcd.clear();

  // checa a comunicacao entre o modulo e arduino
  bool check = radio.isChipConnected();
  if (check == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Receptor ligado");
  } else {
    lcd.setCursor(4, 0);
    lcd.print("Falha no");
    lcd.setCursor(4, 1);
    lcd.print("receptor");
  }
  delay(2000);
  lcd.clear();

  // checa a comunicacao entre os nRF24l01
  if (radio.available()) {
    lcd.setCursor(4, 0);
    lcd.print("Conexao");
    lcd.setCursor(2, 1);
    lcd.print("estabelecida");
  } else {
    lcd.setCursor(4, 0);
    lcd.print("Falha ao");
    lcd.setCursor(4, 1);
    lcd.print("conectar");
  }
  delay(2000);
  lcd.clear();
}


void loop() {

  while (radio.available()) {

    radio.read(&dados, sizeof(dados));

    if (dados[0] == 255 && dados[1] == 255) {

      lcd.setCursor(0, 0);
      lcd.print("Falha na leitura");
      lcd.setCursor(0, 1);
      lcd.print("Verificar sensor");

    } else {

      lcd.setCursor(1, 0);
      lcd.print(dados[0], 1);
      lcd.write((byte)0);
      lcd.print("C");
      lcd.setCursor(10, 0);
      lcd.print(dados[1], 1);
      lcd.print("%");
      lcd.setCursor(0, 1);

      if (dados[2] == 0) {
        lcd.print("Autom.");
        lcd.print(" | ");

        if (dados[0] >= 20) {
          lcd.print("Ligado ");
        } else {
          lcd.print("Deslig.");
        }

      } else {
        lcd.print("Manual");
        lcd.print(" | ");

        if (dados[3] == 1) {
          lcd.print("Ligado ");
        } else {
          lcd.print("Deslig.");
        }
      }
    }
  }

  while (!radio.available()) {

    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Falha na");
    lcd.setCursor(3, 1);
    lcd.print("comunicacao");
    delay(2000);
  }
}
