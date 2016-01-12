#include <EEPROM.h>
#include <Keypad.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

LiquidCrystal_I2C lcd(0x27,16,2); 

HX711 scale(A1, A0);  // A0-->DOUT  A1-->SCK

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

#define Buzzer 10

float weight;
int function;
int num, selec = 1, sum_all1, sum_all2;
int sum1_1,  sum1_2, sum1_3, sum1_4;
int sum2_1,  sum2_2, sum2_3, sum2_4;
int i = 5, o = 14, hot = 0;
int Hot_cof = 8, Ice_cof = 9;

void setup() {
  Serial.begin(9600);
  lcd.init(); 
  lcd.backlight();
  delay(100);
  
  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, 1);
  
  lcd.home();
  lcd.print("  Calibrating ! ");
  scale.set_scale(2132.10f);
  scale.tare();
  delay(1000);
}

void loop() { 
  lcd.setCursor(0, 0);
  lcd.print(" Barista scales ");

/************************* Selection mode ******************&********/  
  char customKey = customKeypad.getKey();
  if(customKey == 'A')  {
    beep();
    function = 1;   
  }
  if(customKey == 'B')  {
    beep();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set Weight      ");
    lcd.setCursor(0, 1);
    lcd.print("Now = ");
    lcd.print(EEPROM.read(Hot_cof));
    lcd.print(" g & ");
    lcd.print(EEPROM.read(Ice_cof));
    lcd.print(" g   ");
    delay(2000);
    i = 5;
    o = 14;
    hot = 0;
    lcd.clear();
    function = 2;
  }
  if(customKey == 'C')  {
    beep();
    lcd.setCursor(0, 0);
    lcd.print("Selection mode  ");
    lcd.setCursor(0, 1);
    lcd.print("  Kg or Coffee ");
    delay(1000);
    lcd.clear();
    function = 3;
  }

/************************* Show weight ******************************/
  if(function == 0) {
    if(selec == 1)  {
      lcd.setCursor(0, 1);
      lcd.print("Weight = ");
      weight = scale.get_units(10);
      if(weight <= 0.0f) weight = 0.0f;
      lcd.print(weight);
      lcd.print("g  ");
    }
    if(selec == 2)  {
      lcd.setCursor(0, 1);
      lcd.print("Coffee = ");
      weight = int(scale.get_units(10));
      if(weight == EEPROM.read(Hot_cof)) 
        lcd.print(" HOT  ");
      if(weight == EEPROM.read(Ice_cof)) 
        lcd.print(" ICE  ");
      else
        lcd.print("      ");
      Serial.print(weight);
      Serial.print(" ");
      Serial.print(EEPROM.read(Hot_cof));
      Serial.print(" ");
      Serial.println(EEPROM.read(Ice_cof));
    }
  } // end function 0 

/********************** Calibration mode **************************/  
  while(function == 1)  {
    lcd.setCursor(0, 0);
    lcd.print("Calibration :   ");
    lcd.setCursor(0, 1);
    lcd.print("*:Cal or #:Exit");

    char customKey = customKeypad.getKey();
    if(customKey == '*')  {
      beep();
      scale.set_scale(2133.00f);
      scale.tare();
      function = 0;
    } 
    if(customKey == '#')  {
      beep();
      function = 0;
    }
  } // end function 1

/********************* Set weight hot and ice ********************/   
  while(function == 2)  {
    lcd.setCursor(0, 0);
    lcd.print("   Hot     Ice  ");  
    lcd.setCursor(0, 1);
    lcd.print("  ");   
    lcd.setCursor(6, 1);
    lcd.print("g ");
    lcd.setCursor(8, 1);
    lcd.print("|");
    lcd.setCursor(9, 1);
    lcd.print("  "); 
    lcd.setCursor(15, 1);
    lcd.print("g "); 
  
    char customKey = customKeypad.getKey();
    if(customKey == '1') num = 1;
    if(customKey == '2') num = 2;
    if(customKey == '3') num = 3;
    if(customKey == '4') num = 4;
    if(customKey == '5') num = 5;
    if(customKey == '6') num = 6;
    if(customKey == '7') num = 7;
    if(customKey == '8') num = 8;
    if(customKey == '9') num = 9;
    if(customKey == '0') num = 0;
    if(customKey == 'D')  {
      beep();
      if(hot >= 1 && hot <= 4)
        i -= 1;
      if(hot >= 5 && hot <= 8)
        o -= 1;
      hot += 1;
    }

    if(hot >= 1 && hot <= 4)  {
      lcd.setCursor(i, 1);
      lcd.cursor();
      delay(50);
      lcd.noCursor();
      delay(50);
      lcd.print(num);
      
      if(hot == 1)  sum1_1 = num*1;
      if(hot == 2)  sum1_2 = num*10;
      if(hot == 3)  sum1_3 = num*100;
      if(hot == 4)  {
        sum1_4 = num*1000;
        sum_all1 = sum1_1 + sum1_2 + sum1_3 + sum1_4;
      }
    }
    
    if(hot >= 5 && hot <= 8)  { 
      lcd.setCursor(o, 1);
      lcd.cursor();
      delay(50);
      lcd.noCursor();
      delay(50);
      lcd.print(num);
      if(hot == 5)  sum2_1 = num*1;
      if(hot == 6)  sum2_2 = num*10;
      if(hot == 7)  sum2_3 = num*100;
      if(hot == 8)  {
        sum2_4 = num*1000;   
        sum_all2 = sum2_1 + sum2_2 + sum2_3 + sum2_4;
      } 
    }
    
    if(hot == 9)  {
      i = 5;
      o = 14;
      hot = 0;
    }
         
    if(customKey == '#')  {
      beep();
      EEPROM.write(Hot_cof, sum_all1);
      EEPROM.write(Ice_cof, sum_all2);
      lcd.clear();
      function = 0;
    } 
  } // end function 2

/******************** Set show HOT or ICE coffee ****************/    
  while(function == 3)  {
    lcd.setCursor(0, 0);
    lcd.print("1 : Show Kg    ");
    lcd.setCursor(0, 1);
    lcd.print("2 : Show Coffee");

    char customKey = customKeypad.getKey();
    if(customKey == '1')  {
      beep();
      lcd.clear();
      selec = 1;
      function = 0;
    } 
    if(customKey == '2')  {
      beep();
      lcd.clear();
      selec = 2;
      function = 0;
    }
  } // end function 3
} // end main loop

void beep() {
  digitalWrite(Buzzer, 0);
  delay(200);
  digitalWrite(Buzzer, 1);
}

