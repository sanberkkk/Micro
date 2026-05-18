#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Pin Tanımlamaları ---
#define ONE_WIRE_BUS 2      // DS18B20 Data pini D2'ye bağlı
#define BT_RX 10            // HC-05 TX pini D10'a bağlı
#define BT_TX 11            // HC-05 RX pini D11'e bağlı 

// --- Ekran Ayarları ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Sensör ve BT Ayarları ---
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
SoftwareSerial btSerial(BT_RX, BT_TX);

float hedefSicaklik = 40.0; // İçeceğin ulaşmasını istediğimiz sıcaklık
bool sarkiCalindi = false;  // Şarkının sürekli baştan başlamaması için bayrak

void setup() {
  Serial.begin(9600);       // Bilgisayardan hata ayıklama için
  btSerial.begin(9600);     // HC-05 varsayılan baud rate
  sensors.begin();
  
  // OLED Ekranı Başlat
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED baslatilamadi"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  sensors.requestTemperatures(); 
  float anlikSicaklik = sensors.getTempCByIndex(0);
  
  // Ekrana Yazdırma
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Akilli Sogutucu");
  display.drawLine(0, 10, 128, 10, WHITE);
  
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("Temp: ");
  display.print(anlikSicaklik, 1);
  display.println("C");

  // Karar Mekanizması
  if (anlikSicaklik <= hedefSicaklik && !sarkiCalindi && anlikSicaklik > -50.0) {
    display.setTextSize(1);
    display.setCursor(0, 50);
    display.println("Soguk! Muzik Caliyor");
    
    btSerial.println("PLAY_MUSIC"); // Python'a komut gönder
    Serial.println("Komut gönderildi: PLAY_MUSIC");
    sarkiCalindi = true; 
  } 
  else if (anlikSicaklik > hedefSicaklik + 2.0) {
    // Sıcaklık tekrar artarsa sistemi sıfırla
    sarkiCalindi = false;
  }
  
  display.display();
  delay(2000); // 2 saniyede bir okuma yap
}