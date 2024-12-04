// esp32 wroom dev kit
// oled display module (4 pin)
// 2 ttp223 touch modules
// mpu 6050

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Define pins for TTP223 sensors
const int touchPin1 = 12;
const int touchPin2 = 13;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial port to connect.

  // Initialize OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to initialize MPU6050!");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Initialize TTP223 pins as input with pull-up resistors
  pinMode(touchPin1, INPUT_PULLUP);
  pinMode(touchPin2, INPUT_PULLUP);
}

void loop() {
  // Read MPU6050 data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Read TTP223 sensor states
  int touchState1 = digitalRead(touchPin1);
  int touchState2 = digitalRead(touchPin2);

  // Display sensor data on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Accel: X=");
  display.print(a.acceleration.x);
  display.print(" Y=");
  display.print(a.acceleration.y);
  display.print(" Z=");
  display.println(a.acceleration.z);

  display.print("Gyro: X=");
  display.print(g.gyro.x);
  display.print(" Y=");
  display.print(g.gyro.y);
  display.print(" Z=");
  display.println(g.gyro.z);

  display.print("Touch 1: ");
  display.println(touchState1 == LOW ? "Pressed" : "Released");
  display.print("Touch 2: ");
  display.println(touchState2 == LOW ? "Pressed" : "Released");

  display.display();

  delay(100);
}
