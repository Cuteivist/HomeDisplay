#include "screen.h"
#include "http.h"
#include "timeUpdate.h"
#include "sleep.h"

#include <SPI.h>
#include <WiFi.h>                     // Built-in
#include "time.h"                     // Built-in

//################ PROGRAM VARIABLES and OBJECTS ################
#define max_readings 24
#define BUILTIN_LED 13

#define autoscale_on  true
#define autoscale_off false
#define barchart_on   true
#define barchart_off  false

float pressure_readings[max_readings]    = {0};
float temperature_readings[max_readings] = {0};
float humidity_readings[max_readings]    = {0};
float rain_readings[max_readings]        = {0};
float snow_readings[max_readings]        = {0};

int32_t mCounter = 0;

//#########################################################################################
void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  initialiseDisplay();

  int16_t x0 = 100;
  int16_t y0 = 100;
  int16_t x1 = 350;
  int16_t y1 = 300;

  int16_t height = 120;
  drawWeather(x0, y0, height, LowCloudy);

  display.drawLine(0, y0, SCREEN_WIDTH, y0, GxEPD_BLACK); // test line (horizontal)
  display.drawLine(0, y1, SCREEN_WIDTH, y1, GxEPD_BLACK); // test line (horizontal)
  display.drawLine(x0, 0, x0, SCREEN_HEIGHT, GxEPD_BLACK); // test line (vertical)
  display.drawLine(x1, 0, x1, SCREEN_HEIGHT, GxEPD_BLACK); // test line (vertical)
  display.display(true);
}

//#########################################################################################
void loop() { // this will never run!
}