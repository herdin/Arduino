/*
   Displays text sent over the serial port (e.g. from the Serial Monitor) on
   an attached LCD.
  Compatible with the Arduino IDE 1.0
  Library version:1.1
*/
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int read_lcd_key  = 0;
int read_button  = 0;

#define BUTTON_NONE   0
#define BUTTON_SELECT 1
#define BUTTON_LEFT   2
#define BUTTON_DOWN   3
#define BUTTON_UP     4
#define BUTTON_RIGHT  5

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

void setup()
{
  lcd.begin(16, 2); //LCD init
  lcd.setCursor(0, 0); //LCD cursor set
  lcd.print("Push the buttons"); //LCD display
  Serial.begin(9600);
}

void loop()
{
  delay(100);
  lcd.setCursor(9, 1); //LCD cursor line 1 pannel 9
  lcd.print(millis() / 1000); //LCD display

  lcd.setCursor(0, 1); //LCD cursor line 1 pannel 0
  read_lcd_key = read_LCD_buttons(); //button read
  switch (read_lcd_key)
  {
    case BUTTON_RIGHT:
        lcd.print("RIGHT");//1
        Serial.print("RIGHT\n");
        break;
    case BUTTON_LEFT:
        lcd.print("LEFT");//3
        Serial.print("LEFT\n");
        break;
    case BUTTON_UP:
        lcd.print("UP");//4
        Serial.print("UP\n");
        break;
    case BUTTON_DOWN:
        lcd.print("DOWN");//2
        Serial.print("DOWN\n");
        break;
    case BUTTON_SELECT:
        lcd.print("SELECT");
        Serial.print("SELECT\n");
        break;
    case BUTTON_NONE:
        lcd.print("NONE");//2
        Serial.print("NONE\n");
        break;
  }

}
