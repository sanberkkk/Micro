#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define LDR_SOL 34
#define LDR_SAG 35

int ESIK_SOL;
int ESIK_SAG;

bool sag_tetik = false;
bool sol_tetik = false;
unsigned long sag_zaman = 0;
unsigned long sol_zaman = 0;
bool sag_onceki = false;
bool sol_onceki = false;
unsigned long son_kalibrasyon = 0;

void kalibrasyon() {
  long sol_toplam = 0, sag_toplam = 0;
  for (int i = 0; i < 100; i++) {
    sol_toplam += analogRead(LDR_SOL);
    sag_toplam += analogRead(LDR_SAG);
    delay(10);
  }
  ESIK_SOL = (sol_toplam / 100) * 0.80;
  ESIK_SAG = (sag_toplam / 100) * 0.80;
  Serial.print("ESIK_SOL: "); Serial.println(ESIK_SOL);
  Serial.print("ESIK_SAG: "); Serial.println(ESIK_SAG);
}

void setup() {
  Serial.begin(115200);

  if (!SerialBT.begin("MY-ESP32")) {
    Serial.println("Bluetooth baslatilamadi");
    while (1);
  }
  Serial.println("Bluetooth hazir");

  kalibrasyon();
  son_kalibrasyon = millis();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.println("HELLO");
  display.display();
}

void loop() {
  int sol = analogRead(LDR_SOL);
  int sag = analogRead(LDR_SAG);

  bool sag_kapali = sag < ESIK_SAG;
  bool sol_kapali = sol < ESIK_SOL;

  if (!sag_kapali && sag_onceki) {
    sag_tetik = true;
    sag_zaman = millis();
  }

  if (!sol_kapali && sol_onceki) {
    sol_tetik = true;
    sol_zaman = millis();
  }

  if (sol_tetik && sag_tetik) {
    unsigned long fark;
    if (sol_zaman < sag_zaman) {
      fark = sag_zaman - sol_zaman;
      if (fark > 50 && fark < 800) {
        Serial.println(">>> NEXT");
        SerialBT.println("NEXT");

        display.clearDisplay();
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(10, 20);
        display.println("NEXT");
        display.display();
      }
    } else {
      fark = sol_zaman - sag_zaman;
      if (fark > 50 && fark < 800) {
        Serial.println("<<< PREV");
        SerialBT.println("PREV");

        display.clearDisplay();
        display.setTextSize(3);
        display.setTextColor(WHITE);
        display.setCursor(10, 20);
        display.println("PREV");
        display.display();
      }
    }
    sag_tetik = false;
    sol_tetik = false;
    son_kalibrasyon = millis();
    delay(500);
  }

  // Hareket tetiklenmemişse ve 5sn geçtiyse kalibrasyon yap
  if (!sag_tetik && !sol_tetik && millis() - son_kalibrasyon > 30000) {
    kalibrasyon();
    son_kalibrasyon = millis();
  }

  if (sag_tetik && millis() - sag_zaman > 800) sag_tetik = false;
  if (sol_tetik && millis() - sol_zaman > 800) sol_tetik = false;

  sag_onceki = sag_kapali;
  sol_onceki = sol_kapali;

  delay(10);
}