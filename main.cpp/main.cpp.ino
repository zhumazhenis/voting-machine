#include <LiquidCrystal.h>
#include <Keypad.h>

// for LCD
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// for Keypad
const byte ROWS = 2;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', '4'},
  {'5', '6', '7', '8'}
};

byte rowPins[ROWS] = {8, 9};
byte colPins[COLS] = {10, 11, 12, 13};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {

  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Print a message to the LCD.
  lcd.print("Key pressed:");

}

void loop() {

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);

  // read from keypad
  char customKey = customKeypad.getKey();

  // print on terminal and LCD
  if (customKey) {
    Serial.println(customKey);
    lcd.print(customKey);
  }
  
}
