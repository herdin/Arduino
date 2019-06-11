/*
  Standalone Sketch to use with a Arduino UNO and a
  Sharp Optical Dust Sensor GP2Y1010AU0F
*/
#include <LiquidCrystal.h>
#include <stdlib.h>

#define PURE_VOLTAGE 145;

#define BUTTON_NONE   0
#define BUTTON_SELECT 1
#define BUTTON_LEFT   2
#define BUTTON_DOWN   3
#define BUTTON_UP     4
#define BUTTON_RIGHT  5

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
float calcVoltage = 0;
float dustdensity = 0;
float ppmpercf = 0;
char s[32];

int read_button  = 0;

int read_LCD_buttons()
{
  read_button = analogRead(0); //analog read from AO pin
  Serial.print("read_button : ");
  Serial.print(read_button);
  Serial.print(" : ");
  /* 1023 none
   * 638 select
   * 409 left
   * 256 down
   * 99 up
   * 0 right
   */
  
  if (read_button > 1000) return BUTTON_NONE;
  else if(read_button > 600) return BUTTON_SELECT;
  else if(read_button > 300) return BUTTON_LEFT;
  else if(read_button > 200) return BUTTON_DOWN;
  else if(read_button > 50) return BUTTON_UP;
  else return BUTTON_RIGHT;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); //LCD init
  lcd.setCursor(0, 0); //LCD cursor set
  lcd.print("Air Condition"); //LCD display
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
  String logString = "realVoltage[";
  logString += dtostrf(realVoltage, 10, 4, s); //9, 4
  logString += "] calcVoltage[";
  logString += dtostrf(calcVoltage, 10, 4, s); //9, 4
  logString += "] dustdensity[";
  logString += dtostrf(dustdensity, 10, 4, s); //5, 2
  logString += "] ppmpercf[";
  logString += dtostrf(ppmpercf, 10, 4, s); //8, 0
  logString += "] calibrationVoltage[";
  logString += dtostrf((calibrationVoltage/calibrationIndex), 10, 4, s);
  logString += "]\n";

  String lcdString = "";
  lcdString += dtostrf(dustdensity, 10, 4, s);
  lcdString += "";
  lcd.setCursor(0, 1); //LCD cursor line 0 pannel 0
  lcd.print(lcdString);
  Serial.print(logString);

  delay(1000);
}
