/*
  Standalone Sketch to use with a Arduino UNO and a
  Sharp Optical Dust Sensor GP2Y1010AU0F
*/
#include <LiquidCrystal.h>
#include <stdlib.h>

#define PURE_VOLTAGE 145;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int dustPin = 5; //a pin
int ledPower = 12; //d pin

/* Pulse-driven wave from
 *  
 *  <--10ms 1 Pulse----------------------------------------------------------..
 *  <--0.32ms LED ON------------------------------------------><--Rest LED OFF--..
 *  <--0.28ms Sampling delay--><-Sampling-><-0.04ms Off Delay->
 */
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

long calibrationIndex = 0;
float calibrationVoltage = 0;
float realVoltage = 0;
float caclVoltage = 0;
float dustdensity = 0;
float ppmpercf = 0;
char s[32];

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPower, OUTPUT);
}

void loop() {
  digitalWrite(ledPower, LOW); // power on the LED
  delayMicroseconds(samplingTime);
  realVoltage = analogRead(dustPin); // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  if(calibrationIndex > 10000) {
    calibrationIndex = 0;
    calibrationVoltage = 0;
  }
  calibrationIndex = calibrationIndex+1;
  calibrationVoltage = calibrationVoltage + realVoltage;
  
  calcVoltage = realVoltage - PURE_VOLTAGE;
  dustdensity = 0.17 * (calcVoltage * 0.0049) - 0.1;
  ppmpercf = ((calcVoltage * 0.0049) - 0.0256) * 120000;
  if (ppmpercf < 0)
    ppmpercf = 0;
  if (dustdensity < 0 )
    dustdensity = 0;
  if (dustdensity > 0.5)
    dustdensity = 0.5;

  /* dtostrf(
   * double target_value,
   * signed char total_string_length,
   * unsigned char precision_length,
   * char* charBuffer
   */
  String dataString = "realVoltage[";
  dataString += dtostrf(realVoltage, 10, 4, s); //9, 4
  dataString += "] calcVoltage[";
  dataString += dtostrf(calcVoltage, 10, 4, s); //9, 4
  dataString += "] dustdensity[";
  dataString += dtostrf(dustdensity, 10, 4, s); //5, 2
  dataString += "] ppmpercf[";
  dataString += dtostrf(ppmpercf, 10, 4, s); //8, 0
  dataString += "] calibrationVoltage[";
  dataString += dtostrf((calibrationVoltage/calibrationIndex), 10, 4, s);
  dataString += "]\n";
  
  Serial.print(dataString);

  delay(1000);
}
