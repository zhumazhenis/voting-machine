#include <LiquidCrystal.h>
#include <Keypad.h>
#include<SoftwareSerial.h>

String ssid = "\"NU\""; //WiFi name
String pwd = "\"1234512345\""; //WiFi password

SoftwareSerial mySerial(10, 13);

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

    int processEsp(char command) override {
      Serial.print("From process ESP: ");
      Serial.println(command);

      if (command == '1') {

        isEnabled = true;
        draw();
        return 1;
      } else if (command == '2') {
        isEnabled = false;
        draw();
        return 1;
      } else if (command == '3') {
        // esp send result

        String conf = "D";
        //        int lenA, lenB;
        //        if (result->a >= 10) {
        //          lenA = 2;
        //        } else {
        //          lenA = 1;
        //        }
        //
        //        if (result->b >= 10) {
        //          lenB = 2;
        //        } else {
        //          lenB = 1;
        //        }
        //
        //        if (lenA == 1) {
        //          conf = conf + "0"  + String(result->a);
        //        } else {
        //          conf = conf + String(result->a);
        //        }
        //
        //        if (lenB == 1) {
        //          conf = conf + "0"  + String(result->b);
        //        } else {
        //          conf = conf + String(result->b);
        //        }
        //
        //        // send conf

        return 1;
      }

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
      }
      popUpMessage();
      draw();
      return 0;
    }


    void draw() override {
      lcd->clear();
      if (isEnabled) {
        lcd->print("En: ");
      } else {
        lcd->print("Dis: ");
      }

      lcd->print(result->a);
      lcd->print(" ");
      lcd->print(result->b);
      lcd->setCursor(0, 1);
    }

    void popUpMessage() {
      lcd->clear();
      if (isEnabled) {
        lcd->print("Successfully voted!");
      } else {
        lcd->print("Not voted!");
      }
      delay(1000);
    }

};

// app created
App app(&lcd);

void setup() {




  /////////////////////////
  char c;
  int bytenum = 0;
  String request = "";
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.println("AT");
  delay(10);

  while (mySerial.available()) {
    c = mySerial.read();
    //Serial.println(String(bytenum) + " "+String(c));
    request += c;
    //bytenum++;
  }
  //Serial.println(request);
  if (request.substring(7, 9) == "OK")
    Serial.println("Detected ESP");
  else
    Serial.println("Could not detect ESP");
  //Send the AT client to configure the ESP as a station (server/client)
  mySerial.println("AT+CWMODE_CUR=1");
  delay(100);
  request = "";
  bytenum = 0;
  while (mySerial.available()) { //Read the request from ESP
    c = mySerial.read();
    //Serial.println(String(i) + " "+String(c));
    request += c;
    //i++;
  }
  if (request.substring(20, 22) == "OK") //Check the request from ESP to check whether OK is present at position 20 and 21
    Serial.println("Successfully configured as a station");
  else
    Serial.println("Error, cannot configure as a station");
  mySerial.println("AT+CWJAP_CUR=" + ssid + "," + pwd); //Connect to WIFI by providing WiFi name and password
  delay(10000); //Give ample time for ESP to connect before sending any new client
  request = "";
  while (mySerial.available()) { //Read the request for the connect client
    c = mySerial.read();
    //Serial.println(String(bytenum) + " "+String(c));
    request += c;
    //bytenum++;
  }
  Serial.println("WIFI Connected");
  delay(1000);
  mySerial.println("AT+CIFSR"); //Send AT client to get the IP address of ESP
  delay(1000);
  request = "";
  bytenum = 0;
  while (mySerial.available()) {
    c = mySerial.read();
    //Serial.println(String(bytenum) + " "+String(c));
    request += c;
    //bytenum++;
  }
  Serial.println("IP Address: " + request.substring(25, 39)); //If the WiFi connection was successful, IP address will be position 25 to 37 in the request
  mySerial.println("AT+CIPMUX=1"); //Send AT client to enable multiple connections to ESP
  delay(1000);
  mySerial.println("AT+CIPSERVER=1,80"); //Send AT client to start an HTTP server on port 80
  delay(1000);
  while (mySerial.available()) { //read the request for previous clients and discard
    c = mySerial.read();
    request += c;
  }
  Serial.println("Started HTTP Server on port 80");

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  app.draw();
}

void loop() {




  // for ESP
  char c;
  int bytenum = 0;
  String request = "";
  String channelNumber, client;
  String conf = "";
  //Wait for some data from ESP. ESP will have data when
  //it receives a request over internet from a client

  while (!mySerial.available()) {
    char key = keypad.getKey();

    // check keypad pressed or not
    if (keypad.keyStateChanged()) {
      if (key == 'A' || key == 'B') {
        app.process(key);
      }
    }
  }
  delay(2000);
  //Read the complete request
  while (mySerial.available()) {
    c = mySerial.read();
    Serial.print(bytenum);
    Serial.print(" ");

    Serial.println(c);
    request += c;
  }
  Serial.println(request);
  client = request.substring(22, 29);
  Serial.println("Recieved client is " + client);
  channelNumber = request.substring(0, 1);
  Serial.println("Received HTTP request on channel " + channelNumber);

  if (client.substring(0, 1) == "1") {
    Serial.println("GOT THE THING");
    app.processEsp('1');
    Serial.println("JEska");




  }




  if (client.substring(0, 1) == "2") {
    Serial.println("GOT THE THING FROM SECOND CLIENT");
    app.processEsp('2');




  }

  if (client.substring(0, 1) == "3") {

    //    app.processEsp('3');
    Serial.println("ResultsT");

    String conf = "D";
    int lenA, lenB;
    if (app.result->a >= 10) {
      lenA = 2;
    } else {
      lenA = 1;
    }

    if (app.result->b >= 10) {
      lenB = 2;
    } else {
      lenB = 1;
    }

    if (lenA == 1) {
      conf = conf + "0"  + String(app.result->a);
    } else {
      conf = conf + String(app.result->a);
    }

    if (lenB == 1) {
      conf = conf + "0"  + String(app.result->b);
    } else {
      conf = conf + String(app.result->b);
    }

    Serial.println("Confirmation:" + conf);
    mySerial.println("AT+CIPSEND=" + channelNumber + ",5");
    delay(100);
    mySerial.println(conf); //Send the confirmation
    delay(100);

  } else {
    Serial.println("IT IS TOO COLD ");
  }

  //Close the channel so that the data is sent to the client
  mySerial.println("AT+CIPCLOSE=" + channelNumber);
  delay(1000);
  while (mySerial.available()) {
    c = mySerial.read();
    char key = keypad.getKey();

    // check keypad pressed or not
    if (keypad.keyStateChanged()) {
      if (key == 'A' || key == 'B') {
        app.process(key);
      }
    }
  }
  Serial.println("Closed.");

  char key = keypad.getKey();

  // check keypad pressed or not
  if (keypad.keyStateChanged()) {
    if (key == 'A' || key == 'B') {
      app.process(key);
    }
  }

}
