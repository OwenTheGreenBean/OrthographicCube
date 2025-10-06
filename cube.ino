#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found");
    for (;;);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  display.clearDisplay();
  display.display();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float roll = atan2(a.acceleration.y, a.acceleration.z);
  float pitch = atan2(-a.acceleration.x, sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z));

  const float size = 10;
  float verts[8][3] = {
    {-size, -size, -size}, {size, -size, -size}, {size, size, -size}, {-size, size, -size},
    {-size, -size, size},  {size, -size, size},  {size, size, size},  {-size, size, size}
  };

  int edges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
  };

  display.clearDisplay();
  for (int i = 0; i < 12; i++) {
    int v1 = edges[i][0];
    int v2 = edges[i][1];
    int16_t x1, y1, x2, y2;
    rotateProject(verts[v1][0], verts[v1][1], verts[v1][2], roll, pitch, x1, y1);
    rotateProject(verts[v2][0], verts[v2][1], verts[v2][2], roll, pitch, x2, y2);
    display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
  }

  display.display();
  delay(30);
}

void rotateProject(float x, float y, float z, float roll, float pitch, int16_t &px, int16_t &py) {
  float y1 = y * cos(roll) - z * sin(roll);
  float z1 = y * sin(roll) + z * cos(roll);
  float x2 = x * cos(pitch) + z1 * sin(pitch);
  float z2 = -x * sin(pitch) + z1 * cos(pitch);

  px = (int16_t)(x2 + SCREEN_WIDTH / 2);
  py = (int16_t)(y1 + SCREEN_HEIGHT / 2);
}
