#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>  // Tambahkan library untuk WiFi
#include <ThingSpeak.h>  // Tambahkan library ThingSpeak

MAX30105 particleSensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Alamat I2C LCD 16x2, sesuaikan jika diperlukan

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

float beatsPerMinute;
int beatAvg;

// Inisialisasi pin data untuk sensor DS18B20
#define oneWireBusPin D2

OneWire oneWire(oneWireBusPin);
DallasTemperature sensors(&oneWire);

// WiFi dan ThingSpeak
const char* ssid = "UGMURO-INET";  // Ganti dengan SSID WiFi Anda
const char* password = "Gepuk15000";  // Ganti dengan password WiFi Anda
const char* apiKey = "FGBFXFPNYYP83ENT";  // Ganti dengan API key ThingSpeak Anda

WiFiClient client;

unsigned long lastTimeTemp = 0;
unsigned long timerDelayTemp = 1000;
unsigned long lastTimeThingSpeak = 0;
unsigned long timerDelayThingSpeak = 20000;  // Kirim data ke ThingSpeak setiap 20 detik

void setup() {
  Serial.begin(115200);
  sensors.begin();  // DS18B20
  Serial.println("Initializing...");

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105 was not found. Please check wiring/power.");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  lcd.init();
  lcd.backlight();
  lcd.print("    WELCOME ");
  lcd.setCursor(0, 1);
  lcd.print("      TO ");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PSMURO");
  lcd.setCursor(0, 1);
  lcd.print("UNIV.GUNADARMA");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MCU HEALTCARE");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistem monitor");
  lcd.setCursor(0, 1);
  lcd.print("Suhu Badan");
  delay(5000);
  lcd.clear();
  lcd.print("Temperature:");
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Suhu   : ");
  lcd.setCursor(0, 1);
  lcd.print("BPM    : ");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = (60 / (delta / 1000.0));

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }

    lcd.setCursor(9, 1);
    lcd.print(beatAvg);
  }

  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  if (irValue < 60000) {
    Serial.print(" No finger?");
  } else {
    // No specific action here
  }

  if ((millis() - lastTimeTemp) > timerDelayTemp) {
    sensors.requestTemperatures();

    float celsius = sensors.getTempCByIndex(0);
    
    Serial.print("Suhu Celsius: ");
    Serial.print(celsius);
    Serial.print("°C ");

    lcd.setCursor(9, 0);
    lcd.print(celsius);

    lastTimeTemp = millis();
  }

  // Update ThingSpeak
  if ((millis() - lastTimeThingSpeak) > timerDelayThingSpeak) {
    lastTimeThingSpeak = millis();

    ThingSpeak.setField(1, beatsPerMinute);
    ThingSpeak.setField(2, beatAvg);
    ThingSpeak.setField(3, sensors.getTempCByIndex(0));

    // Use ThingSpeak.writeFields() with the correct signature
    int responseCode = ThingSpeak.writeFields(0, apiKey); // API Key is passed here

    if (responseCode == 200) {
      Serial.println("Data sent to ThingSpeak successfully.");
    } else {
      Serial.print("Error sending data to ThingSpeak. Response code: ");
      Serial.println(responseCode);
    }
  }

  Serial.println();
}
