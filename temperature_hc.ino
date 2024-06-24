#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin D2 on the WeMos D1 Mini
#define ONE_WIRE_BUS D2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass the oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

void setup(void) {
  // Start serial communication
  Serial.begin(115200);
  Serial.println("Dallas Temperature Control Library Demo");

  // Start up the library
  sensors.begin();
}

void loop(void) {
  // Request temperature readings
  sensors.requestTemperatures(); 
  Serial.print("Temperature is: ");
  Serial.print(sensors.getTempCByIndex(0)); // Get temperature in Celsius
  Serial.println("°C");

  // Wait for 1 second before the next reading
  delay(1000);
}
