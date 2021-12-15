#include "screen.h"
#include "http.h"
#include "timeUpdate.h"
#include "sleep.h"

#include <SPI.h>
#include <WiFi.h>                     // Built-in
#include "time.h"                     // Built-

//################ VARIABLES ###########################################
boolean LargeIcon = true, SmallIcon = false;
#define Large  17           // For icon drawing, needs to be odd number for best effect
#define Small  6            // For icon drawing, needs to be odd number for best effect
String  Time_str, Date_str; // strings to hold time and received weather data


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

  drawString(100, 100, "TEST STRING", CENTER);
  display.display(true);
}

//#########################################################################################
void loop() { // this will never run!
}