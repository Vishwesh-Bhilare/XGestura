#include <Wire.h>
#include <Adafruit_MPU6050.h>

Adafruit_MPU6050 mpu;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  // Initialize MPU-6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 initialization failed!");
    while (1);
  }

  // Set accelerometer and gyroscope ranges if necessary
  // Adafruit's library automatically sets to default values
  
  Serial.println("MPU6050 initialized successfully.");
}

void loop() {
  // Read accelerometer and gyroscope data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Print data
  Serial.print("Accel X: "); Serial.print(a.acceleration.x);
  Serial.print(", Y: "); Serial.print(a.acceleration.y);
  Serial.print(", Z: "); Serial.println(a.acceleration.z);
  
  Serial.print("Gyro X: "); Serial.print(g.gyro.x);
  Serial.print(", Y: "); Serial.print(g.gyro.y);
  Serial.print(", Z: "); Serial.println(g.gyro.z);
  
  delay(100);
}
