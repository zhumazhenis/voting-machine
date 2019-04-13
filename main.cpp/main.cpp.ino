#include <LiquidCrystal_I2C.h>
#include <Keypad.h>


// for LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// for Keypad
const byte ROWS = 2;
const byte COLS = 4;
byte rowPins[ROWS] = {3, 2};
byte colPins[COLS] = {4, 5, 6, 7};
char hexaKeys[ROWS][COLS] = {
  {'A', 'B', 'C', 'D'},
  {'1', 'L', 'R', '0'}
};
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void welcomePage(LiquidCrystal_I2C &lcd);


// stores voting result
class Result {
  public:
    int a;
    int b;
    int c;
    int d;
};

// abstract UI component
class Block {
  public:
    char* name;
    virtual int process(char key) = 0;
    virtual void draw() = 0;
};

// block for showing result
class ResultBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    Result* result;

    ResultBlock(LiquidCrystal_I2C* lcd, Result* result) {
      this->name = (char*)"Result Block";
      this->lcd = lcd;
      this->result = result;
    }

    int process(char key) override {
      if (key == '0') {
        return 0;
      }
      return 1;
    }

    void draw() override {
      lcd->clear();
      lcd->print("(A) (B) (C) (D)");
      lcd->setCursor(0, 1);
      lcd->print(result->a);
      lcd->setCursor(4, 1);
      lcd->print(result->b);
      lcd->setCursor(8, 1);
      lcd->print(result->c);
      lcd->setCursor(12, 1);
      lcd->print(result->d);
    }
};

// block for voting
class VoteBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    Result* result;

    VoteBlock(LiquidCrystal_I2C* lcd, Result* result) {
      this->name = (char*)"Vote Block";
      this->lcd = lcd;
      this->result = result;
    }

    int process(char key) override {
      if (key == 'A') {
        result->a++;
        popUpMessage((char*) "      (A)");
      }
      if (key == 'B') {
        result->b++;
        popUpMessage((char*) "      (B)");
      }
      if (key == 'C') {
        result->c++;
        popUpMessage((char*) "      (C)");
      }
      if (key == 'D') {
        result->d++;
        popUpMessage((char*) "      (D)");
      }
      if (key == '0') {
        return 0;
      }
      return 1;
    }

    void draw() override {
      lcd->clear();
      lcd->print("Press name:");
      lcd->setCursor(0, 1);
      lcd->print("            BACK");
    }

  private:
    void popUpMessage(char* candidate) {
      lcd->clear();
      lcd->print(" You voted for:");
      lcd->setCursor(0, 1);
      lcd->print(candidate);
      delay(1000);
      draw();
    }
};

// main menu
class App: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    Result* result;
    Block* blocks[2];
    int currentBlockIndex;
    int activeBlockIndex;

    App(LiquidCrystal_I2C* lcd) {
      this->name = (char*)"App Block";
      this->lcd = lcd;
      Result* result = new Result();
      this->result = result;
      this->blocks[0] = new VoteBlock(lcd, result);
      this->blocks[1] = new ResultBlock(lcd, result);
      this->currentBlockIndex = 0;
      this->activeBlockIndex = -1;
    }

    int process(char key) override {
      if (activeBlockIndex != -1) {
        int res = blocks[activeBlockIndex]->process(key);
        if (res == 0) {
          activeBlockIndex = -1;
          draw();
        }
        return 0;
      }

      if (key == '1') {
        activeBlockIndex = currentBlockIndex;
        blocks[activeBlockIndex]->draw();
        return 0;
      }

      if (key == 'L') {
        if (currentBlockIndex > 0) {
          currentBlockIndex--;
        }
      }

      if (key == 'R') {
        if (currentBlockIndex < 1) {
          currentBlockIndex++;
        }
      }

      draw();
      return 0;
    }

    void draw() override {
      lcd->clear();
      lcd->print(blocks[currentBlockIndex]->name);
      lcd->setCursor(0, 1);
      lcd->print("OK    <  >   ");
    }
};

// app created
App app(&lcd);

void setup() {

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  welcomePage(lcd);
  app.draw();
}

void loop() {

  // read from keypad
  char key = keypad.getKey();

  // check keypad pressed or not
  if (keypad.keyStateChanged()) {
    app.process(key);
  }
}

// welcome page
void welcomePage(LiquidCrystal_I2C &lcd) {

  for (char c : " VOTING-MACHINE") {
    if (c) {
      lcd.print(c);
    }
    delay(80);
  }

  lcd.setCursor(0, 1);
  for (char c : "  BY JAVASSYL") {
    if (c) {
      lcd.print(c);
    }
    delay(80);
  }

  delay(2000);
  lcd.clear();
}
