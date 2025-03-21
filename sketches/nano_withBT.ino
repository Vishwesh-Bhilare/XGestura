#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <SoftwareSerial.h>

Adafruit_MPU6050 mpu;

// Define RX and TX pins for SoftwareSerial
SoftwareSerial bluetooth(10, 11); // RX, TX

void setup() {
  Wire.begin();
  Serial.begin(9600);   // For Serial Monitor
  bluetooth.begin(9600); // For HC-05 Bluetooth communication
  
  // initialize MPU-6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 initialization failed!");
    while (1);
  }

  // set ranges for accelerometer and gyroscope
  Serial.println("MPU6050 initialized successfully.");
  bluetooth.println("MPU6050 initialized successfully."); // Send via Bluetooth
}

void loop() {
  // read accelerometer and gyroscope data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // create a string to hold sensor data
  String sensorData = "";

  // add accelerometer data to the string
  sensorData += "Accel X: " + String(a.acceleration.x) + 
                ", Y: " + String(a.acceleration.y) + 
                ", Z: " + String(a.acceleration.z) + "\n";

  // add gyroscope data to the string
  sensorData += "Gyro X: " + String(g.gyro.x) + 
                ", Y: " + String(g.gyro.y) + 
                ", Z: " + String(g.gyro.z) + "\n";

  // print to the serial monitor
  Serial.print(sensorData);

  // send data over Bluetooth
  bluetooth.print(sensorData);

  delay(100);
}
