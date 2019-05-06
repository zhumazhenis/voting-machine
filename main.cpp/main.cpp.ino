#include <LiquidCrystal.h>
#include <Keypad.h>


// for LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

// const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

//const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// for Keypad
const byte ROWS = 2;
const byte COLS = 2;

byte rowPins[ROWS] = {8, 9};
byte colPins[COLS] = {6, 7};

char hexaKeys[ROWS][COLS] = {
  {'B', 'A'}, {'0', '1'}

};


Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


// stores voting result
class Result {
  public:
    int a;
    int b;
};

// abstract UI component
class Block {
  public:
    char* name;
    virtual int process(char key) = 0;
    virtual int processEsp(char command) = 0;
    virtual void draw() = 0;
};



// main menu
class App: public Block {
  public:
    LiquidCrystal* lcd;
    Result* result;
    int isEnabled;

    App(LiquidCrystal* lcd) {
      this->name = (char*)"App Block";
      this->lcd = lcd;
      Result* result = new Result();
      this->result = result;
      this->isEnabled = true;
    }

    int processEsp(char key) override {
      return 1;
    }

    int process(char key) override {
      
      Serial.print("from App: ");
      Serial.println(key);

      if (isEnabled) {
        if (key == 'A') {
          result->a++;
        } else if (key == 'B') {
          result->b++;
        }
        popUpMessage();
      }

      
      
      draw();

      return 0;
    }

    

    void draw() override {
      lcd->clear();
      lcd->print(result->a);
      lcd->print(" ");
      lcd->print(result->b);
      lcd->setCursor(0, 1);
      lcd->print("OK    <  >   ");
    }

    void popUpMessage() {
      lcd->clear();
      lcd->print("Successfully voted!");
      delay(1000);
    }

};

// app created
App app(&lcd);

void setup() {

  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  app.draw();
}

void loop() {

  char key = keypad.getKey();
  
  // check keypad pressed or not
  if (keypad.keyStateChanged()) {
    if (key == 'A' || key == 'B') {
      app.process(key);
    }
  }
  
}
