// // UNFINISHED CODE!!

//   #include <Wire.h> // framework library
//   #include <Adafruit_MCP9601.h> // amplifier library
//   #include <Adafruit_MPRLS.h> // pressure sensor library

//   #define PCA9548_ADDR 0x70 // define multiplexer
//   #define SerialPort Serial // serial connection


//   Adafruit_MCP9601 tc[8]; // thermocouple object
//   Adafruit_MPRLS press; // pressure sensor object

//   unsigned long lastSample = 0; // placeholder time value
//   const unsigned long sampleInterval = 100; // 10 Hz

//   // bool control = false; // set to 1 to run code with temp control
//   // const int pelt_pin = 1; // define which pin the peltier is connected to
//   // const int heat_pin = 2; // define which pin the heater is connected to

//   // const int setpoint_low = 0; // low stable temperature (deg C)
//   // const int setpoint_hi = 35; // high stable temperature (deg C)
//   // const int hist = 2; // hysterisis value (deg C)

//   // function to cycle and initialize temp readings
//   void tempSelect(uint8_t channel) {
//     if (channel > 7) return;
//     Wire.beginTransmission(PCA9548_ADDR);
//     Wire.write(1 << channel);
//     Wire.endTransmission();
//   }

// void setup() {

//   Wire.begin();
//   Serial.begin(115200); // baud rate

//   while (!Serial) {
//     delay(10);
//   }

//   // pinMode(pelt_pin, OUTPUT); 
//   // pinMode(heat_pin, OUTPUT);

//   for (int i = 0; i < 8; i++) {
    
//     if (!tc[i].begin(0x67)) {
//       Serial.print("MCP9601 not found on Channel ");
//       Serial.println(i); }
//     tc[i].setADCresolution(MCP9600_ADCRESOLUTION_18);
//     tc[i].setThermocoupleType(MCP9600_TYPE_K);
//     tc[i].setFilterCoefficient(3);
//     tc[i].enable(true); }


//   if (!press.begin()) {
//     Serial.println("Pressure Sensor not found!");}

//   Serial.println("timestamp_ms,Pressure,T1,T2,T3,T4,T5,T6,T7,T8,Peltier");
//   Serial.println("Ready.");
// }

// void loop() {

//   Serial.println("Program Initialized!");

//   unsigned long timestamp = millis();

//   if (timestamp - lastSample >= sampleInterval) {
//     lastSample = timestamp;
//   }

//     float temps[8]; // to hold temp values

//     // get temp data from thermocouples
//     for (uint8_t i = 0; i < 8; i++) {
//       tempSelect(i);
//       delay(10);  // allow mux to settle
//       temps[i] = tc[i].readThermocouple();
//       // delay(1000); // remove when ready to test fully
//     }

//     // if (control == true) {

//     //   if (temps[7] > setpoint_hi + hist) {
//     //       digitalWrite(pelt_pin, HIGH); // turns on peltier cell
//     //   }

//     //   if (temps[7] < setpoint_hi - hist) {
//     //       digitalWrite(pelt_pin, LOW); // turns off peltier cell
//     //   }

//     //   if (temps[7] < setpoint_low + hist) {
//     //       digitalWrite(heat_pin, HIGH); // turns on heater
//     //   }

//     //   if (temps[7] > setpoint_low - hist) {
//     //     digitalWrite(heat_pin, LOW); // turns off heater
//     //   }
//     // }

//     // read pressure data
//     float pressure = press.readPressure();

//     // output to matlab (t, pressure, panel temp 1-6, int ambient, ext ambient, active cooling on / off)
//     // output issued
//     Serial.print(timestamp); // time (s)
//     Serial.print(",");
//     Serial.print(pressure, 3); // pressure (Pa)
//     Serial.print(",");

//     for (int i = 0; i < 7; i++) {
//       Serial.print(temps[i], 3);
//       Serial.print(","); }

//     // for last temp, a new line starts after
//     for (int i = 7; i < 8; i++) {
//       Serial.println(temps[i], 3);}

//     }


// UNFINISHED CODE!!



#include <Wire.h>
#include <Adafruit_MCP9601.h>
#include <Adafruit_MPRLS.h>

#define PCA9548_ADDR 0x70
#define SerialPort Serial

Adafruit_MCP9601 tc[8];
Adafruit_MPRLS press = Adafruit_MPRLS(-1,-1);
unsigned long lastSample = 0;
const unsigned long sampleInterval = 500; // 10 Hz

void tempSelect(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(PCA9548_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void muxOff() {
  Wire.beginTransmission(PCA9548_ADDR);
  Wire.write(0); // disable all channels
  Wire.endTransmission();
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Serial.println("Program Initialized!");

  muxOff();

  if (!press.begin()) {
    Serial.println("Pressure Sensor not found!");
  }

  for (int i = 0; i < 8; i++) {
    tempSelect(i); // ← ADDED: select MUX channel before init
    delay(10);
    if (!tc[i].begin(0x67)) {
      Serial.print("MCP9601 not found on Channel ");
      Serial.println(i);
    }
    tc[i].setADCresolution(MCP9600_ADCRESOLUTION_18);
    tc[i].setThermocoupleType(MCP9600_TYPE_T); // ← CHANGED: K → T
    tc[i].setFilterCoefficient(3);
    tc[i].enable(true);
  }

  Serial.println("timestamp_ms,Pressure,T1,T2,T3,T4,T5,T6,T7,T8,Peltier");
  Serial.println("Ready.");
}

void loop() {
  unsigned long timestamp = millis();

  if (timestamp - lastSample >= sampleInterval) { // ← brace now wraps everything
    lastSample = timestamp;

    float temps[8];

  for (uint8_t i = 0; i < 8; i++) {
      tempSelect(i);
      delay(10);  // allow mux to settle
      temps[i] = tc[i].readThermocouple();}

    muxOff();
    delay(10);
    float pressure = press.readPressure();

    Serial.print(timestamp);
    Serial.print(",");
    Serial.print(pressure, 3);
    Serial.print(",");
    for (int i = 0; i < 7; i++) {
      Serial.print(temps[i], 3);
      Serial.print(",");
    }
    Serial.println(temps[7], 3); // last channel + newline

  } // ← end of sample interval block
}