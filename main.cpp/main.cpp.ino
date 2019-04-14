#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoSTL.h>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <iterator>


struct cmp_str {
   bool operator() (char const *a, char const *b) const {
      return strcmp(a, b) < 0;
   }
};


// Database of card and names
std::map<char*, char*, cmp_str> db = {
  {"a", "a_zhuma\n"}, 
  {"b", "b_zhuma\n"},
  {"c", "c_zhuma\n"},
  {"d", "d_zhuma\n"},
  {"e", "e_zhuma\n"},
  {"f", "f_zhuma\n"},
  {"g", "g_zhuma\n"},
  {"h", "h_zhuma\n"},
  {"i", "i_zhuma\n"},
  {"j", "j_zhuma\n"},
  {"k", "k_zhuma\n"},
  {"l", "l_zhuma\n"},
  {"m", "m_zhuma\n"},
  {"n", "n_zhuma\n"}
};


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
    std::set<const char*, cmp_str> a;
    std::set<const char*, cmp_str> b;
    std::set<const char*, cmp_str> c;
    std::set<const char*, cmp_str> d;
};

// abstract UI component
class Block {
  public:
    char* name;
    virtual int processKey(char key) = 0;
    virtual int processRfid(const char* uid) = 0;
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

    int processRfid(const char* uid) override {
      return 1;
    }

    void draw() override {
      lcd->clear();
      lcd->print("(A) (B) (C) (D)");
      lcd->setCursor(0, 1);
      lcd->print(result->a.size());
      lcd->setCursor(4, 1);
      lcd->print(result->b.size());
      lcd->setCursor(8, 1);
      lcd->print(result->c.size());
      lcd->setCursor(12, 1);
      lcd->print(result->d.size());
    }
};


// block VoteBlockRfid
class VoteBlockRfid: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    Result* result;
    char candidate;

    VoteBlockRfid(LiquidCrystal_I2C* lcd, Result* result) {
      this->name = (char*)"Vote Block";
      this->lcd = lcd;
      this->result = result;
    }

    void setCandidate(char candidate) {
      this->candidate = candidate;
    }

    int processKey(char key) override {
      if (key == '0') {
        return 0;
      }
      return 1;
    }

    int processRfid(const char* uid) override {
      
      if (candidate == 'A') {
        this->result->a.insert(uid);
        popUpMessage(uid);
      }

      if (candidate == 'B') {
        this->result->b.insert(uid);
        popUpMessage(uid);
      }

      if (candidate == 'C') {
        this->result->c.insert(uid);
        popUpMessage(uid);
      }

      if (candidate == 'D') {
        this->result->d.insert(uid);
        popUpMessage(uid);
      }
      return 0;
    }

    void draw() override {
      lcd->clear();
      lcd->print('(');
      lcd->print(candidate);
      lcd->print(')');
      lcd->print(" Put your ID!");
      lcd->setCursor(0, 1);
      lcd->print("            BACK");
    }

  private:
    void popUpMessage(const char* uid) {
      lcd->clear();
      lcd->print(uid);
      lcd->setCursor(0, 1);
      lcd->print(candidate);
      delay(1000);
//      draw();
    }
};


// block for voting
class VoteBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    Result* result;
    VoteBlockRfid* voteBlockRfid;
    boolean isVoteBlockRfidActive;

    VoteBlock(LiquidCrystal_I2C* lcd, Result* result) {
      this->name = (char*)"Vote Block";
      this->lcd = lcd;
      this->result = result;
      this->voteBlockRfid = new VoteBlockRfid(lcd, result);
      this->isVoteBlockRfidActive = false;
    }

    int processKey(char key) override {

      if (this->isVoteBlockRfidActive) {
        int res = this->voteBlockRfid->processKey(key);
        if (res == 0) {
          this->draw();
          this->isVoteBlockRfidActive = false;
        }
        return 1;
      }
      
      if (key == 'A') {
        this->isVoteBlockRfidActive = true;
        voteBlockRfid->setCandidate('A');
        voteBlockRfid->draw();
//        result->a.insert("");
//        popUpMessage("      (A)");
      }
      
      if (key == 'B') {
        this->isVoteBlockRfidActive = true;
        voteBlockRfid->setCandidate('B');
        voteBlockRfid->draw();
//        result->b.insert("");
//        popUpMessage("      (B)");
      }
      
      if (key == 'C') {
        this->isVoteBlockRfidActive = true;
        voteBlockRfid->setCandidate('C');
        voteBlockRfid->draw();
//        result->c.insert("");
//        popUpMessage("      (C)");
      }
      
      if (key == 'D') {
        this->isVoteBlockRfidActive = true;
        voteBlockRfid->setCandidate('D');
        voteBlockRfid->draw();
//        result->d.insert("");
//        popUpMessage("      (D)");
      }

      if (key == '0') {
        return 0;
      }
      
      return 1;
    }

    int processRfid(const char* uid) override {
      if (this->isVoteBlockRfidActive) {
        int res = this->voteBlockRfid->processRfid(uid);
        if (res == 0) {
          this->isVoteBlockRfidActive = false;
          this->draw();
        }
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

    int processKey(char key) override {
      if (activeBlockIndex != -1) {
        int res = blocks[activeBlockIndex]->processKey(key);
        if (res == 0) {
          activeBlockIndex = -1;
          draw();
        }
        return 1;
      }

      if (key == '1') {
        activeBlockIndex = currentBlockIndex;
        blocks[activeBlockIndex]->draw();
        return 1;
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
      return 1;
    }

    int processRfid(const char* uid) override {
      if (activeBlockIndex != -1) {
        int res = blocks[activeBlockIndex]->processRfid(uid);
        if (res == 0) {
          activeBlockIndex = -1;
          draw();
        }
      } 
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
App app(&lcd);

void setup() {

  // Serial monitor
  Serial.begin(9600);

  for (std::map<char*, char*, cmp_str>::iterator it = db.begin(); it != db.end(); it++) {
    Serial.print(it->first);
    Serial.print(it->second);
  }

//  Serial.println(db["a"]);

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

  if (strcmp("a", "a")) {
    Serial.println("Jeska\n");
  }

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
  Serial.println("Content: " + content);
  if (content.substring(1) == "80 38 94 1A") { //change here the UID of the card/cards that you want to give access
    Serial.println("Authorized access");
    Serial.println();
    //    delay(3000);
  } else {
    Serial.println("Access denied");
    //    delay(3000);
  }

  char* temp = new char[12];
  content.substring(1).toCharArray(temp, 12);
  Serial.print("Temp = ");
  Serial.println(temp);
  app.processRfid(temp);
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
