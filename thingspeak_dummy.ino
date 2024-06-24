#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <ThingSpeak.h>

// Data wire is connected to GPIO4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

// WiFi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// ThingSpeak credentials
unsigned long myChannelNumber = YOUR_CHANNEL_NUMBER;
const char* myWriteAPIKey = "YOUR_WRITE_API_KEY";

WiFiClient client;

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);

  // Start up the library
  sensors.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Request temperature from sensor
  sensors.requestTemperatures();

  // Read temperature in Celsius
  float cakti_fasya = sensors.getTempCByIndex(0);
  float dicky_fahri = sensors.getTempCByIndex(1);
  float justin_alfi = sensors.getTempCByIndex(2);
  float arya_dimas = sensors.getTempCByIndex(3);
  float alifian_anwar = sensors.getTempCByIndex(4);
  float rino_mycella = sensors.getTempCByIndex(5);

  // Write temperature to ThingSpeak
  ThingSpeak.setField(1, cakti_fasya);
  ThingSpeak.setField(2, dicky_fahri);
  ThingSpeak.setField(3, justin_alfi);
  ThingSpeak.setField(4, arya_dimas);
  ThingSpeak.setField(5, alifian_anwar);
  ThingSpeak.setField(6, rino_mycella);

  // Write to ThingSpeak
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  
  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Wait 20 seconds to update ThingSpeak
  delay(20000);
}
