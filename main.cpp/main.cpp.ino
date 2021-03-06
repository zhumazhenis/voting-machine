#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoSTL.h>
#include <vector>
#include <set>
#include <map>



// char pointer comparator for set and map
struct compareString {
  bool operator() (char const *a, char const *b) const {
    return strcmp(a, b) < 0;
  }
};


// Database of card and names

std::map<const char*, const char*, compareString> db = {
  {"E3 8C 87 C9", "Zhumazhenis D."},
  {"80 38 94 1A", "Bekassyl S."}
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
    std::set<const char*, compareString> a;
    std::set<const char*, compareString> b;
    std::set<const char*, compareString> c;
    std::set<const char*, compareString> d;
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
class AllResultBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    Result* result;

    AllResultBlock(LiquidCrystal_I2C* lcd, Result* result) {
      this->name = (char*)"All Result";
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


// block AResultBlock
class AResultBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    Result* result;
    int currentIndexOfVoter;
    std::map<const char*, const char*, compareString> *db;

    AResultBlock(LiquidCrystal_I2C* lcd, Result* result, std::map<const char*, const char*, compareString> *db) {
      this->name = (char*)"A - Result";
      this->lcd = lcd;
      this->db = db;
      this->result = result;
      this->currentIndexOfVoter = 0;
    }

    int processKey(char key) override {
      if (key == '0') {
        return 0;
      }

      if (result->a.size() == 0) {
        draw();
        return 1;
      }

      if (key == 'L') {
        if (0 < currentIndexOfVoter) {
          currentIndexOfVoter--;
        } else {
          currentIndexOfVoter = result->a.size() - 1;
        }
      }

      if (key == 'R') {
        if (currentIndexOfVoter < result->a.size() - 1) {
          currentIndexOfVoter++;
        } else {
          currentIndexOfVoter = 0;
        }
      }

      draw();
      return 1;
    }

    int processRfid(const char* uid) override {
      return 1;
    }

    void draw() override {
      lcd->clear();
      std::set<const char*, compareString>::iterator it = result->a.begin();
      std::advance(it, currentIndexOfVoter);
      lcd->print((*db)[*it]);
      //      lcd->print(*it);
      lcd->setCursor(0, 1);
      lcd->print("<");
      lcd->setCursor(5, 1);
      lcd->print("/");
      lcd->print(result->a.size());
      lcd->setCursor(10, 1);
      lcd->print(">");
      lcd->setCursor(12, 1);
      lcd->print("BACK");

      int len = 0;
      int temp = currentIndexOfVoter + 1;
      while (temp) {
        temp /= 10;
        len++;
      }
      lcd->setCursor(5 - len, 1);

      if (0 != result->a.size()) {
        lcd->print(currentIndexOfVoter + 1);
      } else {
        lcd->print(0);
      }

      if (result->a.size() == 0) {
        lcd->setCursor(0, 0);
        lcd->print("No Vote!");
      }
    }
};


// block BResultBlock
class BResultBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    std::map<const char*, const char*, compareString> *db;
    Result* result;
    int currentIndexOfVoter;

    BResultBlock(LiquidCrystal_I2C* lcd, Result* result, std::map<const char*, const char*, compareString> *db) {
      this->name = (char*)"B - Result";
      this->lcd = lcd;
      this->db = db;
      this->result = result;
      this->currentIndexOfVoter = 0;
    }

    int processKey(char key) override {
      if (key == '0') {
        return 0;
      }

      if (result->b.size() == 0) {
        draw();
        return 1;
      }

      if (key == 'L') {
        if (0 < currentIndexOfVoter) {
          currentIndexOfVoter--;
        } else {
          currentIndexOfVoter = result->b.size() - 1;
        }
      }

      if (key == 'R') {
        if (currentIndexOfVoter < result->b.size() - 1) {
          currentIndexOfVoter++;
        } else {
          currentIndexOfVoter = 0;
        }
      }

      draw();
      return 1;
    }

    int processRfid(const char* uid) override {
      return 1;
    }

    void draw() override {
      lcd->clear();
      std::set<const char*, compareString>::iterator it = result->b.begin();
      std::advance(it, currentIndexOfVoter);
      lcd->print((*db)[*it]);
      lcd->setCursor(0, 1);
      lcd->print("<");
      lcd->setCursor(5, 1);
      lcd->print("/");
      lcd->print(result->b.size());
      lcd->setCursor(10, 1);
      lcd->print(">");
      lcd->setCursor(12, 1);
      lcd->print("BACK");

      int len = 0;
      int temp = currentIndexOfVoter + 1;
      while (temp) {
        temp /= 10;
        len++;
      }
      lcd->setCursor(5 - len, 1);

      if (0 != result->b.size()) {
        lcd->print(currentIndexOfVoter + 1);
      } else {
        lcd->print(0);
      }

      if (result->b.size() == 0) {
        lcd->setCursor(0, 0);
        lcd->print("No Vote!");
      }
    }
};


// block CResultBlock
class CResultBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    std::map<const char*, const char*, compareString> *db;
    Result* result;
    int currentIndexOfVoter;

    CResultBlock(LiquidCrystal_I2C* lcd, Result* result, std::map<const char*, const char*, compareString> *db) {
      this->name = (char*)"C - Result";
      this->lcd = lcd;
      this->db = db;
      this->result = result;
      this->currentIndexOfVoter = 0;
    }

    int processKey(char key) override {
      if (key == '0') {
        return 0;
      }

      if (result->c.size() == 0) {
        draw();
        return 1;
      }

      if (key == 'L') {
        if (0 < currentIndexOfVoter) {
          currentIndexOfVoter--;
        } else {
          currentIndexOfVoter = result->c.size() - 1;
        }
      }

      if (key == 'R') {
        if (currentIndexOfVoter < result->c.size() - 1) {
          currentIndexOfVoter++;
        } else {
          currentIndexOfVoter = 0;
        }
      }

      draw();
      return 1;
    }

    int processRfid(const char* uid) override {
      return 1;
    }

    void draw() override {
      lcd->clear();
      std::set<const char*, compareString>::iterator it = result->c.begin();
      std::advance(it, currentIndexOfVoter);
      lcd->print((*db)[*it]);
      lcd->setCursor(0, 1);
      lcd->print("<");
      lcd->setCursor(5, 1);
      lcd->print("/");
      lcd->print(result->c.size());
      lcd->setCursor(10, 1);
      lcd->print(">");
      lcd->setCursor(12, 1);
      lcd->print("BACK");

      int len = 0;
      int temp = currentIndexOfVoter + 1;
      while (temp) {
        temp /= 10;
        len++;
      }
      lcd->setCursor(5 - len, 1);

      if (0 != result->c.size()) {
        lcd->print(currentIndexOfVoter + 1);
      } else {
        lcd->print(0);
      }

      if (result->c.size() == 0) {
        lcd->setCursor(0, 0);
        lcd->print("No Vote!");
      }
    }
};


// block DResultBlock
class DResultBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    std::map<const char*, const char*, compareString> *db;
    Result* result;
    int currentIndexOfVoter;

    DResultBlock(LiquidCrystal_I2C* lcd, Result* result, std::map<const char*, const char*, compareString> *db) {
      this->name = (char*)"D - Result";
      this->lcd = lcd;
      this->db = db;
      this->result = result;
      this->currentIndexOfVoter = 0;
    }

    int processKey(char key) override {
      if (key == '0') {
        return 0;
      }

      if (result->d.size() == 0) {
        draw();
        return 1;
      }

      if (key == 'L') {
        if (0 < currentIndexOfVoter) {
          currentIndexOfVoter--;
        } else {
          currentIndexOfVoter = result->d.size() - 1;
        }
      }

      if (key == 'R') {
        if (currentIndexOfVoter < result->d.size() - 1) {
          currentIndexOfVoter++;
        } else {
          currentIndexOfVoter = 0;
        }
      }

      draw();
      return 1;
    }

    int processRfid(const char* uid) override {
      return 1;
    }

    void draw() override {
      lcd->clear();
      std::set<const char*, compareString>::iterator it = result->d.begin();
      std::advance(it, currentIndexOfVoter);
      lcd->print((*db)[*it]);
      lcd->setCursor(0, 1);
      lcd->print("<");
      lcd->setCursor(5, 1);
      lcd->print("/");
      lcd->print(result->d.size());
      lcd->setCursor(10, 1);
      lcd->print(">");
      lcd->setCursor(12, 1);
      lcd->print("BACK");

      int len = 0;
      int temp = currentIndexOfVoter + 1;
      while (temp) {
        temp /= 10;
        len++;
      }
      lcd->setCursor(5 - len, 1);

      if (0 != result->d.size()) {
        lcd->print(currentIndexOfVoter + 1);
      } else {
        lcd->print(0);
      }

      if (result->d.size() == 0) {
        lcd->setCursor(0, 0);
        lcd->print("No Vote!");
      }
    }
};

// block IndividualResultBlock
class IndividualResultBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    std::map<const char*, const char*, compareString> *db;
    Result* result;
    static int const numberOfBlocks = 4;
    Block* blocks[numberOfBlocks];
    int currentBlockIndex;
    int activeBlockIndex;

    IndividualResultBlock(LiquidCrystal_I2C* lcd, Result* result, std::map<const char*, const char*, compareString> *db) {
      this->name = (char*)"Indiv. Result";
      this->lcd = lcd;
      this->result = result;
      this->db = db;
      this->blocks[0] = new AResultBlock(lcd, result, db);
      this->blocks[1] = new BResultBlock(lcd, result, db);
      this->blocks[2] = new CResultBlock(lcd, result, db);
      this->blocks[3] = new DResultBlock(lcd, result, db);
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
      } else if (key == '0') {
        return 0;
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
      return 1;
    }

    void draw() override {
      lcd->clear();
      // TODO: need to implement
      lcd->print(blocks[currentBlockIndex]->name);
      lcd->setCursor(0, 1);
      lcd->print("OK <     >  BACK");
      lcd->setCursor(6, 1);
      lcd->print("/");
      lcd->print(numberOfBlocks);
      lcd->setCursor(5, 1);
      lcd->print(currentBlockIndex + 1);
    }
};


// block VoteRfidBlock
class VoteRfidBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    std::map<const char*, const char*, compareString> *db;
    Result* result;
    char candidate;

    VoteRfidBlock(LiquidCrystal_I2C* lcd, Result* result, std::map<const char*, const char*, compareString> *db) {
      this->name = (char*)"Vote RFID Block";
      this->lcd = lcd;
      this->db = db;
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
        // 0 = not allowed
        // 1 = already voted
        // 2 = successful
        int type = 0;
        if (db->find(uid) == db->end()) {
          type = 0;
        } else if (result->a.find(uid) != result->a.end() ||
                   result->b.find(uid) != result->b.end() ||
                   result->c.find(uid) != result->c.end() ||
                   result->d.find(uid) != result->d.end()) {
          type = 1;
        } else {
          type = 2;
          result->a.insert(uid);
        }
        popUpMessage(uid, type);
      }

      if (candidate == 'B') {
        // 0 = not allowed
        // 1 = already voted
        // 2 = successful
        int type = 0;
        if (db->find(uid) == db->end()) {
          type = 0;
        } else if (result->a.find(uid) != result->a.end() ||
                   result->b.find(uid) != result->b.end() ||
                   result->c.find(uid) != result->c.end() ||
                   result->d.find(uid) != result->d.end()) {
          type = 1;
        } else {
          type = 2;
          this->result->b.insert(uid);
        }
        popUpMessage(uid, type);
      }

      if (candidate == 'C') {
        // 0 = not allowed
        // 1 = already voted
        // 2 = successful
        int type = 0;
        if (db->find(uid) == db->end()) {
          type = 0;
        } else if (result->a.find(uid) != result->a.end() ||
                   result->b.find(uid) != result->b.end() ||
                   result->c.find(uid) != result->c.end() ||
                   result->d.find(uid) != result->d.end()) {
          type = 1;
        } else {
          type = 2;
          this->result->c.insert(uid);
        }
        popUpMessage(uid, type);
      }

      if (candidate == 'D') {
        // 0 = not allowed
        // 1 = already voted
        // 2 = successful
        int type = 0;
        if (db->find(uid) == db->end()) {
          type = 0;
        } else if (result->a.find(uid) != result->a.end() ||
                   result->b.find(uid) != result->b.end() ||
                   result->c.find(uid) != result->c.end() ||
                   result->d.find(uid) != result->d.end()) {
          type = 1;
        } else {
          type = 2;
          this->result->d.insert(uid);
        }
        popUpMessage(uid, type);
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
    void popUpMessage(const char* uid, int type) {
      lcd->clear();

      if (type == 0) {
        lcd->print("ERROR! Not found");
        lcd->setCursor(0, 1);
        lcd->print("in database");
      } else if (type == 1) {
        lcd->print("ERROR! Already");
        lcd->setCursor(0, 1);
        lcd->print("voted");
      } else if (type == 2) {
        lcd->print("Succeed! Voted");
        lcd->setCursor(0, 1);
        lcd->print(candidate);
      }

      delay(1000);
    }
};


// block for voting
class VoteBlock: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    std::map<const char*, const char*> *db;
    Result* result;
    VoteRfidBlock* voteRfidBlock;
    boolean isVoteRfidBlockActive;

    VoteBlock(LiquidCrystal_I2C* lcd, Result* result, std::map<const char*, const char*, compareString> *db) {
      this->name = (char*)"Vote";
      this->lcd = lcd;
      this->result = result;
      this->voteRfidBlock = new VoteRfidBlock(lcd, result, db);
      this->isVoteRfidBlockActive = false;
    }

    int processKey(char key) override {

      if (this->isVoteRfidBlockActive) {
        int res = this->voteRfidBlock->processKey(key);
        if (res == 0) {
          this->draw();
          this->isVoteRfidBlockActive = false;
        }
        return 1;
      }

      if (key == 'A') {
        this->isVoteRfidBlockActive = true;
        voteRfidBlock->setCandidate('A');
        voteRfidBlock->draw();
      }

      if (key == 'B') {
        this->isVoteRfidBlockActive = true;
        voteRfidBlock->setCandidate('B');
        voteRfidBlock->draw();
      }

      if (key == 'C') {
        this->isVoteRfidBlockActive = true;
        voteRfidBlock->setCandidate('C');
        voteRfidBlock->draw();
      }

      if (key == 'D') {
        this->isVoteRfidBlockActive = true;
        voteRfidBlock->setCandidate('D');
        voteRfidBlock->draw();
      }

      if (key == '0') {
        return 0;
      }

      return 1;
    }

    int processRfid(const char* uid) override {
      if (this->isVoteRfidBlockActive) {
        int res = this->voteRfidBlock->processRfid(uid);
        if (res == 0) {
          this->isVoteRfidBlockActive = false;
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

};

// main menu
class App: public Block {
  public:
    LiquidCrystal_I2C* lcd;
    std::map<const char*, const char*, compareString> *db;
    Result* result;
    static int const numberOfBlocks = 3;
    Block* blocks[numberOfBlocks];
    int currentBlockIndex;
    int activeBlockIndex;

    App(LiquidCrystal_I2C* lcd, std::map<const char*, const char*, compareString> *db) {
      this->name = (char*)"App Block";
      this->lcd = lcd;
      this->db = db;
      Result* result = new Result();
      this->result = result;
      this->blocks[0] = new VoteBlock(lcd, result, db);
      this->blocks[1] = new AllResultBlock(lcd, result);
      this->blocks[2] = new IndividualResultBlock(lcd, result, db);
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
      lcd->print("OK   <     >    ");
      lcd->setCursor(8, 1);
      lcd->print("/");
      lcd->print(numberOfBlocks);
      lcd->setCursor(7, 1);
      lcd->print(currentBlockIndex + 1);
    }
};

// app created
App app(&lcd, &db);

void setup() {



  // Serial monitor
  Serial.begin(9600);

  Serial.println("Kazakhstan");
  Serial.println(db["beka"]);

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
  Serial.print("\nSize: ");
  Serial.println(mfrc522.uid.size);
  for (int i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  char *temp = new char[12];
  content.substring(1).toCharArray(temp, 12);
  Serial.print("Temp = ");
  Serial.println(temp);
  app.processRfid(temp);
  //  delete temp;
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
