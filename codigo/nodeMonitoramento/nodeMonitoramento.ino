#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal.h>

#define CE 9
#define CSN 10

const byte endereco[6] = "00001";
float dados[4];
byte grau[8] = { B00001100,
                 B00010010,
                 B00010010,
                 B00001100,
                 B00000000,
                 B00000000,
                 B00000000,
                 B00000000,
               };

RF24 radio(CE, CSN);
LiquidCrystal lcd(A0, A5, A4, A3, A2, A1);

void setup() {

  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(0, grau);

  lcd.setCursor(0, 0);
  lcd.print("Inicializando...");

  radio.begin();
  delay(1000);
  radio.setDataRate( RF24_250KBPS );
  delay(1000);
  radio.setRetries(3, 5);
  delay(1000);
  radio.openReadingPipe(1, endereco);
  delay(1000);
  radio.setPALevel(RF24_PA_MIN);
  delay(1000);
  radio.startListening();
  delay(1000);

  lcd.clear();
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
}
