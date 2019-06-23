/*
  Standalone Sketch to use with a Arduino UNO and a
  Sharp Optical Dust Sensor GP2Y1010AU0F
*/
#include <LiquidCrystal.h>
#include <dht.h>
#include <stdlib.h>

#define PULSE_MILLISECOND 1000

#define NO_DUST_VOLTAGE 0.0

#define ANALOG_PIN_DUST 5
#define ANALOG_PIN_TH 11

#define DIGITAL_PIN_LED 12

#define DUST_SAMPLING_TIME_MICRO 280
#define DUST_DELTA_TIME_MICRO 40
#define DUST_SLEEP_TIME_MICRO 9680

/* https://www.airkorea.or.kr/index
 * ~ 030 ㎍/㎥ good
 * ~ 080 ㎍/㎥ noraml
 * ~ 150 ㎍/㎥ bad
 * 150 ~ very bad

#define AIR_QUALITY_GOOD 30
#define AIR_QUALITY_NORMAL 80
#define AIR_QUALITY_BAD 150
 */

#define BUTTON_NONE   0
#define BUTTON_SELECT 1
#define BUTTON_LEFT   2
#define BUTTON_DOWN   3
#define BUTTON_UP     4
#define BUTTON_RIGHT  5

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
dht DHT;
/* Pulse-driven wave from
 *  
 *  <--10ms 1 Pulse----------------------------------------------------------..
 *  <--0.32ms LED ON------------------------------------------><--Rest LED OFF--..
 *  <--0.28ms Sampling delay--><-Sampling-><-0.04ms Off Delay->
 */

float readVoltage = 0.0;
float realVoltage = 0.0;
float caliVoltage = 0.0;
float dustdensity = 0.0;
char s[32];
int dustFlag = 0;

int read_button = 0;

int err = 0;
double temp = 0.0;
double humi = 0.0;

void button_select() {
  
}
void button_left() {
  
}
void button_down() {
  caliVoltage = caliVoltage - 0.1;
  if(caliVoltage < 0.0) {
    caliVoltage = 0;
  }
  lcd.setCursor(0, 1);
  lcd.print(dtostrf(caliVoltage, 6, 2, s));
}
void button_up() {
  caliVoltage = caliVoltage + 0.1;
  lcd.setCursor(0, 1);
  lcd.print(dtostrf(caliVoltage, 6, 2, s));
}
void button_right() {
  
}
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
    button_select();
    return BUTTON_SELECT;
  } else if(read_button > 300) {
    Serial.println("BUTTON_LEFT");
    lcd.print("LEFT ");
    button_left();
    return BUTTON_LEFT;
  } else if(read_button > 200) {
    Serial.println("BUTTON_DOWN");
    lcd.print("DOWN ");
    button_down();
    return BUTTON_DOWN;
  } else if(read_button > 50) {
    Serial.println("BUTTON_UP");
    lcd.print("UP   ");
    button_up();
    return BUTTON_UP;
  } else {
    Serial.println("BUTTON_RIGHT");
    lcd.print("RIGHT");
    button_right();
    return BUTTON_RIGHT;
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); //LCD init
  lcd.setCursor(0, 0); //LCD cursor set
  lcd.print("initialize.."); //LCD display
  pinMode(DIGITAL_PIN_LED, OUTPUT);
}

void loop() {
  Serial.println("----------------------------------");
  dustFlag = millis()%1000;
  Serial.print("millisec : ");
  Serial.print(millis());
  Serial.print(" : dustFlag : ");
  Serial.println(dustFlag);
  
  read_button  = read_LCD_buttons();

  if(dustFlag%2 != 0) {
    Serial.println("----------------------------------");
    delay(PULSE_MILLISECOND);
    return;
  }

  //TEMPERATURE, HUMIDITY START
  err = DHT.read11(ANALOG_PIN_TH);
  Serial.print("temp/humi : ");
  if(err == -1) {
    temp = DHT.temperature;
    humi = DHT.humidity;
    Serial.print(temp);
    Serial.print("/");
    Serial.println(humi);
  } else {
    Serial.print(" error : ");
    Serial.println(err);
  }
  //TEMPERATURE, HUMIDITY END
  
  //DUST START
  digitalWrite(DIGITAL_PIN_LED, LOW);          // power on the LED
  delayMicroseconds(DUST_SAMPLING_TIME_MICRO); //0280 us -> 0.28 ms
  readVoltage = analogRead(ANALOG_PIN_DUST);   // read the dust value
  delayMicroseconds(DUST_DELTA_TIME_MICRO);    //0040 us -> 0.04 ms
  digitalWrite(DIGITAL_PIN_LED, HIGH);         // turn the LED off
  delayMicroseconds(DUST_SLEEP_TIME_MICRO);    //9680 us -> 9.68 ms -sum-> 10ms

  realVoltage = (readVoltage*5.0)/1024.0; //convert analog value to voltage
//  dustdensity = (realVoltage-NO_DUST_VOLTAGE)/0.0005;
  dustdensity = (realVoltage-caliVoltage)/0.005;
  if(dustdensity < 0.0) {
    dustdensity = 0.0;
  } else if(dustdensity > 999.0) {
    dustdensity = 999.0;
  }
  //DUST END

  //LOG START
  /* dtostrf(
   * double target_value,
   * signed char total_string_length,
   * unsigned char precision_length,
   * char* charBuffer
   */
  String logString = "readVoltage[";
  logString += dtostrf(readVoltage, 7, 2, s);
  logString += "] realVoltage[";
  logString += dtostrf(realVoltage, 7, 2, s);
  logString += "] caliVoltage[";
  logString += dtostrf(caliVoltage, 7, 2, s);
  logString += "] dustdensity[";
  logString += dtostrf(dustdensity, 7, 2, s);
  logString += "]";
  Serial.println(logString);
  //LOG START

  //LCD STAR
  lcd.setCursor(0, 0); //LCD cursor line 0 pannel 0
  lcd.print(dtostrf(temp, 5, 1, s));
  lcd.print((char)223);
  lcd.print("C");
  lcd.print(dtostrf(humi, 5, 1, s));
  lcd.print("%");
  
  lcd.setCursor(0, 1); //LCD cursor line 0 pannel 1
  lcd.print(dtostrf(dustdensity, 6, 2, s));
  lcd.print("ug/m3");
  //LCD END
  
  Serial.println("----------------------------------");
  delay(PULSE_MILLISECOND-110);
}
