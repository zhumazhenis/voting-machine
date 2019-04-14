#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoSTL.h>
#include <vector>
#include <map>


std::vector<int> vv;
std::map<String, String> mm;

// LCD configuration
LiquidCrystal_I2C lcd(0x3F, 16, 2);


// Keypad configuration
const byte ROWS = 2;
const byte COLS = 4;
byte rowPins[ROWS] = {3, 2};
byte colPins[COLS] = {4, 5, 6, 7};
char hexaKeys[ROWS][COLS] = {
  {'A', 'B', 'C', 'D'},
  {'1', 'L', 'R', '0'}
};
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


// RFID card reader configuration
const byte SS_PIN = 10;
const byte RST_PIN = 9;
MFRC522 mfrc522(SS_PIN, RST_PIN);


// welcome page
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
    virtual int processKey(char key) = 0;
    virtual int processRfid(String uid) = 0;
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

    int processKey(char key) override {
      if (key == '0') {
        return 0;
      }
      return 1;
    }

    int processRfid(String uid) override {
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
    MFRC522* mfrc522;
    Result* result;


    VoteBlock(LiquidCrystal_I2C* lcd, MFRC522* mfrc522, Result* result) {
      this->name = (char*)"Vote Block";
      this->lcd = lcd;
      this->mfrc522 = mfrc522;
      this->result = result;
    }

    int processKey(char key) override {
      if (key == '0') {
        return 0;
      }
      
      if (key == 'A') {
        result->a++;
        popUpMessage("      (A)");
      }
      
      if (key == 'B') {
        result->b++;
        popUpMessage("      (B)");
      }
      
      if (key == 'C') {
        result->c++;
        popUpMessage("      (C)");
      }
      
      if (key == 'D') {
        result->d++;
        popUpMessage("      (D)");
      }
      
      return 1;
    }

    int processRfid(String uid) override {
      return 1;
    }

    void draw() override {
      lcd->clear();
      lcd->print("Press name:");
      lcd->setCursor(0, 1);
      lcd->print("            BACK");
    }

  private:
    void popUpMessage(String candidate) {
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
    static int const numberOfBlocks = 2;
    Block* blocks[numberOfBlocks];
    int currentBlockIndex;
    int activeBlockIndex;

    App(LiquidCrystal_I2C* lcd, MFRC522* mfrc522) {
      this->name = (char*)"App Block";
      this->lcd = lcd;
      Result* result = new Result();
      this->result = result;
      this->blocks[0] = new VoteBlock(lcd, mfrc522, result);
      this->blocks[1] = new ResultBlock(lcd, result);
      this->currentBlockIndex = 0;
      this->activeBlockIndex = -1;
    }

    int processKey(char key) override {
      if (activeBlockIndex != -1) {
        int res = blocks[activeBlockIndex]->processKey(key);
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
        if (0 < currentBlockIndex) {
          currentBlockIndex--;
        } else {
          currentBlockIndex = numberOfBlocks - 1;
        }
      }

      if (key == 'R') {
        if (currentBlockIndex < numberOfBlocks - 1) {
          currentBlockIndex++;
        } else {
          currentBlockIndex = 0;
        }
      }

      draw();
      return 0;
    }

    int processRfid(String uid) override {
      return 1;
    }

    void draw() override {
      lcd->clear();
      lcd->print(blocks[currentBlockIndex]->name);
      lcd->setCursor(0, 1);
      lcd->print("OK    <  >   ");
    }
};

// app created
App app(&lcd, &mfrc522);

void setup() {

  // Serial monitor
  Serial.begin(9600);

  // RFID init
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

  // lcd init
  lcd.init();
  lcd.backlight();

  // welcome page
  welcomePage(lcd);
  app.draw();
}

void loop() {

  // read from keypad
  char key = keypad.getKey();

  // check keypad pressed or not
  if (keypad.keyStateChanged()) {
    app.processKey(key);
  }

  // RFID Card Reader
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (int i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "80 38 94 1A") { //change here the UID of the card/cards that you want to give access
    Serial.println("Authorized access");
    Serial.println();
    //    delay(3000);
  } else {
    Serial.println("Access denied");
    //    delay(3000);
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
