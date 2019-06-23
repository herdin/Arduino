/*
  Standalone Sketch to use with a Arduino UNO and a
  Sharp Optical Dust Sensor GP2Y1010AU0F
*/
#include <LiquidCrystal.h>
#include <stdlib.h>

#define NO_DUST_VOLTAGE 0.0
#define DUST_PIN 5
#define LED_PIN 12
/* https://www.airkorea.or.kr/index
 * ~ 030 ㎍/㎥ good
 * ~ 080 ㎍/㎥ noraml
 * ~ 150 ㎍/㎥ bad
 * 150 ~ very bad
 */
#define AIR_QUALITY_GOOD 30
#define AIR_QUALITY_NORMAL 80
#define AIR_QUALITY_BAD 150

#define BUTTON_NONE   0
#define BUTTON_SELECT 1
#define BUTTON_LEFT   2
#define BUTTON_DOWN   3
#define BUTTON_UP     4
#define BUTTON_RIGHT  5

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
/* Pulse-driven wave from
 *  
 *  <--10ms 1 Pulse----------------------------------------------------------..
 *  <--0.32ms LED ON------------------------------------------><--Rest LED OFF--..
 *  <--0.28ms Sampling delay--><-Sampling-><-0.04ms Off Delay->
 */
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float readVoltage = 0.0;
float realVoltage = 0.0;
float caliVoltage = 0.0;
float dustdensity = 0.0;
char s[32];
int dustFlag = 0;

int read_button = 0;

int read_LCD_buttons()
{
  read_button = analogRead(0); //analog read from AO pin
  Serial.print("read_button : ");
  /* 1023 none
   * 638 select
   * 409 left
   * 256 down
   * 99 up
   * 0 right
   */
  lcd.setCursor(11, 1); //LCD cursor line 0 pannel 0
  if (read_button > 1000) {
    Serial.println("BUTTON_NONE");
    lcd.print("NONE ");
    return BUTTON_NONE;
  } else if(read_button > 600) {
    Serial.println("BUTTON_SELECT");
    lcd.print("SELECT");
    return BUTTON_SELECT;
  } else if(read_button > 300) {
    Serial.println("BUTTON_LEFT");
    lcd.print("LEFT ");
    return BUTTON_LEFT;
  } else if(read_button > 200) {
    Serial.println("BUTTON_DOWN");
    lcd.print("DOWN ");
    return BUTTON_DOWN;
  } else if(read_button > 50) {
    Serial.println("BUTTON_UP");
    lcd.print("UP   ");
    return BUTTON_UP;
  } else {
    Serial.println("BUTTON_RIGHT");
    lcd.print("RIGHT");
    return BUTTON_RIGHT;
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); //LCD init
  lcd.setCursor(0, 0); //LCD cursor set
  lcd.print("Air Condition"); //LCD display
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  dustFlag = millis()/1000;
  Serial.print("dustFlag : ");
  Serial.println(dustFlag);
  
  //BUTTON START
  read_button  = read_LCD_buttons();
  //BUTTON END
  if(dustFlag%2 != 0) {
    delay(1000);
    return;
  }
  //DUST START
  digitalWrite(LED_PIN, LOW);         // power on the LED
  delayMicroseconds(samplingTime);    //0280 -> 0.28
  readVoltage = analogRead(DUST_PIN); // read the dust value
  delayMicroseconds(deltaTime);       //0040 -> 0.04
  digitalWrite(LED_PIN, HIGH);        // turn the LED off
  delayMicroseconds(sleepTime);       //9680 -> 9.68 -+-> 10ms

  realVoltage = (readVoltage*5.0)/1024.0; //convert analog value to voltage
  dustdensity = (realVoltage-NO_DUST_VOLTAGE)/0.0005;
  //DUST END

  //LOG START
  /* dtostrf(
   * double target_value,
   * signed char total_string_length,
   * unsigned char precision_length,
   * char* charBuffer
   */
  String logString = "readVoltage[";
  logString += dtostrf(readVoltage, 10, 4, s); //9, 4
  logString += "] realVoltage[";
  logString += dtostrf(realVoltage, 10, 4, s); //9, 4
  logString += "] dustdensity[";
  logString += dtostrf(dustdensity, 10, 4, s); //5, 2
  logString += "]\n";
  Serial.print(logString);
  //LOG START

  //LCD STAR
  lcd.setCursor(0, 0); //LCD cursor line 0 pannel 0
  lcd.print(dtostrf(readVoltage, 6, 2, s));
  lcd.print("->");
  lcd.print(dtostrf(realVoltage, 6, 2, s));
  
  lcd.setCursor(0, 1); //LCD cursor line 0 pannel 0
  lcd.print(dtostrf(dustdensity, 6, 2, s));
  //LCD END

  delay(1000);
}