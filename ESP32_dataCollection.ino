// UNFINISHED CODE!!

#include <Wire.h> // framework library
#include <Adafruit_MCP9601.h> // amplifier library
#include <Adafruit_MPRLS.h> // pressure sensor library

#define PCA9548_ADDR 0x70 // define multiplexer

// function to cycle and initialize temp readings
void tempSelect(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(PCA9548_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

Adafruit_MCP9601 tc; // temperature amplifier object
Adafruit_MPRLS press(1500.0, -1500.0); // pressure sensor object

unsigned long lastSample = 0;
const unsigned long sampleInterval = 100; // 10 Hz

void setup() {

  Serial.begin(115200);
  Wire.begin(21, 22);   // ESP32 SDA, SCL

  pcaSelect(0);
  delay(10);

  if (!tc.begin(0x60)) {
    Serial.println("MCP9601 not found!");
    while (1);
  }
  // characteristics
  tc.setADCresolution(MCP9601_ADCRESOLUTION_18);
  tc.setThermocoupleType(MCP9601_TYPE_K);
  tc.setFilterCoefficient(3);
  tc.enable(true);

  if (!press.begin()) {
    Serial.println("Pressure Sensor not found!");
    while (1);
  }

  Serial.println("timestamp_ms,Pressure, T1,T2,T3,T4,T5,T6,T7,T8,Peltier");
  Serial.println("Ready.");
}

void loop() {

  unsigned long timestamp = millis();

  if (timestamp - lastSample >= sampleInterval) {
    lastSample = timestamp;

    float temps[8];

    // get temp data from thermocouples
    for (uint8_t i = 0; i < 8; i++) {
      pcaSelect(i);
      delay(3);  // allow mux to settle
      temps[i] = tc.readThermocouple();
    }

    // read pressure data
    float pressure = press.readPressure();

    // output to matlab (t, pressure, panel temp 1-6, int ambient, ext ambient, active cooling on / off)
    // output issued
    Serial.print(timestamp); // time (s)
    Serial.print(",");
    Serial.print(pressure, 3); // pressure (Pa)

    for (int i = 0; i < 7; i++) {
      Serial.print(temps[i], 3);
      Serial.print(","); }

    // for last temp, a new line starts after
    for int i = 7; i < 8; i++) {
      Serial.println(temps[i], 3);}

    }

  }
}
