/*
  Standalone Sketch to use with a Arduino UNO and a
  Sharp Optical Dust Sensor GP2Y1010AU0F
*/
#include <stdlib.h>

int dustPin = 0; //Connect dust sensor to Arduino A0 pin
int ledPower = 2;   //Connect 3 led driver pins of dust sensor to Arduino D2

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voltage = 0;
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
  voltage = analogRead(dustPin); // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH); // turn the LED off
  delayMicroseconds(sleepTime);


  voltage = voltage * 0.0049;
  dustdensity = 0.17 * voltage - 0.1;
  ppmpercf = (voltage - 0.0256) * 120000;
  if (ppmpercf < 0)
    ppmpercf = 0;
  if (dustdensity < 0 )
    dustdensity = 0;
  if (dustdensity > 0.5)
    dustdensity = 0.5;

  String dataString = "";
  dataString += dtostrf(voltage, 9, 4, s);
  dataString += ",";
  dataString += dtostrf(dustdensity, 5, 2, s);
  dataString += ",";
  dataString += dtostrf(ppmpercf, 8, 0, s);
  dataString += "\n";
  Serial.print(dataString);

  delay(1000);
}
