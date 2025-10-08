#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <math.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BUFFER_SIZE 20

MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

struct SensorData {
  float ax, ay, az;
  float gx, gy, gz;
};
SensorData buffer[BUFFER_SIZE];
int indexBuffer = 0;
bool bufferFilled = false;

float restAx = 0, restAy = 0, restAz = 1;
float rollOffset = 0, pitchOffset = 0;
float filteredRoll = 0, filteredPitch = 0;
String currentGesture = "Calibrating...";
int stableFrames = 0;

// ---------------- Utility ----------------
float computeAverage(float arr[], int n) {
  float sum = 0;
  for (int i = 0; i < n; i++) sum += arr[i];
  return sum / n;
}
float computeStdDev(float arr[], int n, float mean) {
  float s = 0;
  for (int i = 0; i < n; i++) s += pow(arr[i] - mean, 2);
  return sqrt(s / n);
}

// ---------------- Calibration ----------------
void calibrateSensor() {
  const int samples = 200;
  float sumAx = 0, sumAy = 0, sumAz = 0;
  for (int i = 0; i < samples; i++) {
    sumAx += mpu.getAccelerationX() / 16384.0;
    sumAy += mpu.getAccelerationY() / 16384.0;
    sumAz += mpu.getAccelerationZ() / 16384.0;
    delay(5);
  }
  restAx = sumAx / samples;
  restAy = sumAy / samples;
  restAz = sumAz / samples;

  rollOffset = atan2(restAy, restAz);
  pitchOffset = atan2(-restAx, sqrt(restAy * restAy + restAz * restAz));
}

// ---------------- Data collection ----------------
void collectData() {
  SensorData d;
  d.ax = mpu.getAccelerationX() / 16384.0;
  d.ay = mpu.getAccelerationY() / 16384.0;
  d.az = mpu.getAccelerationZ() / 16384.0;
  d.gx = mpu.getRotationX() / 131.0;
  d.gy = mpu.getRotationY() / 131.0;
  d.gz = mpu.getRotationZ() / 131.0;
  buffer[indexBuffer] = d;
  indexBuffer = (indexBuffer + 1) % BUFFER_SIZE;
  if (indexBuffer == 0) bufferFilled = true;
}

// ---------------- Shake Detection ----------------
bool detectOscillation(float arr[], int size, float threshold, int requiredFlips) {
  int signFlips = 0;
  for (int i = 1; i < size; i++) {
    if ((arr[i - 1] > threshold && arr[i] < -threshold) ||
        (arr[i - 1] < -threshold && arr[i] > threshold))
      signFlips++;
  }
  return (signFlips >= requiredFlips);
}

// ---------------- Gesture Detection ----------------
String detectGesture(float avgAx, float avgAy, float avgAz,
                     float avgGx, float avgGy, float avgGz,
                     float stdGx, float stdGy, float stdGz,
                     float axArr[], float ayArr[], float azArr[]) {

  float dAx = avgAx - restAx;
  float dAy = avgAy - restAy;
  float dAz = avgAz - restAz;

  // Stability detection
  if (stdGx < 10 && stdGy < 10 && stdGz < 10)
    stableFrames++;
  else
    stableFrames = 0;

  // --- Shake detection (oscillation-based) ---
  if (detectOscillation(axArr, BUFFER_SIZE, 0.4, 3) ||
      detectOscillation(ayArr, BUFFER_SIZE, 0.4, 3))
    return "Shake Horizontal";
  else if (detectOscillation(azArr, BUFFER_SIZE, 0.4, 3))
    return "Shake Vertical";

  // --- Other gestures ---
  if (fabs(avgGz) > 40)
    return (avgGz > 0) ? "Twist CW" : "Twist CCW";
  else if (dAy > 0.25)
    return "Flip Forward";
  else if (dAy < -0.25)
    return "Flip Backward";
  else if (fabs(avgAz - restAz) > 0.5)
    return "Tap";
  else if (dAx > 0.18)
    return "Tilt Right";
  else if (dAx < -0.18)
    return "Tilt Left";
  else if (dAz > 0.15)
    return "Lift Up";
  else if (dAz < -0.15)
    return "Lower Down";
  else if (stableFrames > 1)
    return "Stable";
  else
    return currentGesture;
}

// ---------------- Display ----------------
void drawTiltDot(float roll, float pitch) {
  int cx = SCREEN_WIDTH / 2;
  int cy = 30;
  int radius = 12;

  display.drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 4, SSD1306_WHITE);
  display.drawFastHLine(cx - radius - 6, cy, 2 * (radius + 6), SSD1306_WHITE);
  display.drawFastVLine(cx, cy - radius - 6, 2 * (radius + 6), SSD1306_WHITE);
  display.drawCircle(cx, cy, radius, SSD1306_WHITE);

  int dx = (int)(roll * 57.3 * 0.6);
  int dy = (int)(pitch * 57.3 * 0.6);
  dx = constrain(dx, -radius, radius);
  dy = constrain(dy, -radius, radius);
  display.fillCircle(cx + dx, cy + dy, 3, SSD1306_WHITE);
}

void updateDisplay(float avgAx, float avgAy, float avgAz, float roll, float pitch) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 0);
  display.printf("Ax:%+.2f Ay:%+.2f Az:%+.2f", avgAx, avgAy, avgAz);

  drawTiltDot(roll, pitch);

  display.setCursor(5, 52);
  display.printf("Gesture: %s", currentGesture.c_str());
  display.display();
}

// ---------------- Setup ----------------
void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);
  mpu.initialize();
  if (!mpu.testConnection()) while (1);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while (1);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 25);
  display.println("Calibrating...");
  display.display();

  calibrateSensor();
  currentGesture = "Stable";
}

// ---------------- Loop ----------------
void loop() {
  collectData();
  if (!bufferFilled) { delay(10); return; }

  float ax[BUFFER_SIZE], ay[BUFFER_SIZE], az[BUFFER_SIZE];
  float gx[BUFFER_SIZE], gy[BUFFER_SIZE], gz[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) {
    ax[i] = buffer[i].ax; ay[i] = buffer[i].ay; az[i] = buffer[i].az;
    gx[i] = buffer[i].gx; gy[i] = buffer[i].gy; gz[i] = buffer[i].gz;
  }

  float avgAx = computeAverage(ax, BUFFER_SIZE);
  float avgAy = computeAverage(ay, BUFFER_SIZE);
  float avgAz = computeAverage(az, BUFFER_SIZE);
  float avgGx = computeAverage(gx, BUFFER_SIZE);
  float avgGy = computeAverage(gy, BUFFER_SIZE);
  float avgGz = computeAverage(gz, BUFFER_SIZE);
  float stdGx = computeStdDev(gx, BUFFER_SIZE, avgGx);
  float stdGy = computeStdDev(gy, BUFFER_SIZE, avgGy);
  float stdGz = computeStdDev(gz, BUFFER_SIZE, avgGz);

  currentGesture = detectGesture(avgAx, avgAy, avgAz, avgGx, avgGy, avgGz,
                                 stdGx, stdGy, stdGz, ax, ay, az);

  float newRoll = atan2(avgAy, avgAz) - rollOffset;
  float newPitch = atan2(-avgAx, sqrt(avgAy * avgAy + avgAz * avgAz)) - pitchOffset;

  filteredRoll = filteredRoll * 0.7 + newRoll * 0.3;
  filteredPitch = filteredPitch * 0.7 + newPitch * 0.3;

  updateDisplay(avgAx, avgAy, avgAz, filteredRoll, filteredPitch);

  delay(60);
}
