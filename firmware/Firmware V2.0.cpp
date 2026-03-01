#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#include <Wire.h>
#include <PCA9685.h>

/* #### ~ Justricity ~ #### */
/* ------------------------ */
/*                          */
/*          Config          */
/*                          */
/* ------------------------ */
/* ######################## */

//WIFI
const char* AP_SSID = "Scoreboard-AP";
const char* AP_PASS = "12345678";

//HTML
//HTML Page / WebInterface has to be configured individually. The most simple config is the color palette found at the top of the HTML Document



//DRIVERS

//I²C Pins
constexpr int I2C_SDA_PIN = 4;  // GPIO4 = D2
constexpr int I2C_SCL_PIN = 5;  // GPIO5 = D1

constexpr uint8_t NUM_PCA = 2; //Number of connected drivers

constexpr uint8_t PCA_ADDRBITS[NUM_PCA] = { B000000, B000001 }; // 0x40, 0x41
PCA9685 pca[NUM_PCA] = {
  PCA9685(PCA_ADDRBITS[0]),
  PCA9685(PCA_ADDRBITS[1])
};
constexpr uint16_t PWM_ON  = 4095; //On brightness
constexpr uint16_t PWM_OFF = 0; //Off brightness

//DRIVER Debugging
enum Seg : uint8_t { A=0, B, C, D, E, F, G}; //Segment order
constexpr uint8_t DIGIT_MASK[10] = { //Segment mapping
  /*0*/ 0b00111111, // A B C D E F
  /*1*/ 0b00000110, // B C
  /*2*/ 0b01011011, // A B D E G
  /*3*/ 0b01001111, // A B C D G
  /*4*/ 0b01100110, // B C F G
  /*5*/ 0b01101101, // A C D F G
  /*6*/ 0b01111101, // A C D E F G
  /*7*/ 0b00000111, // A B C
  /*8*/ 0b01111111, // A B C D E F G
  /*9*/ 0b01101111  // A B C D F G
};
enum DisplayID : uint8_t { //Driver Segment order
  D_1_1, D_1_2,
  D_2_1, D_2_2,
  // D_3_1, D_3_2, ...
  DISPLAY_COUNT
};
struct DisplayMap { //Channel Mapping (PCA1.1 = CH0-6, PCA1.2 = CH7-13, ...)
  uint8_t pcaIndex;
  uint8_t ch[7];
};

constexpr DisplayMap displays[DISPLAY_COUNT] = { //Driver wiring
  // D_1_1  (PCA index 0, display 1)
  { 0, { 0, 1, 2, 3, 4, 5, 6} },

  // D_1_2  (PCA index 0, display 2)
  { 0, { 7, 8, 9, 10, 11, 12, 13} },

  // D_2_1  (PCA index 1, display 1)
  { 1, { 0, 1, 2, 3, 4, 5, 6} },

  // D_2_2  (PCA index 1, display 2)
  { 1, { 7, 8, 9, 10, 11, 12, 13} },
};


AsyncWebServer server(80);
IPAddress apIP(192,168,4,1);
const char* espHostname   = "scorecore";

constexpr bool SERIAL_ECHO = true;

/* #### ~ Justricity ~ #### */
/* ------------------------ */
/*                          */
/*           HTML           */
/*                          */
/* ------------------------ */
/* ######################## */

#pragma region //HTML Setup

//Sending "404 Not Found" if not found


//Setting up pages
void setupServerRoutes(){

  //Setting up index.html
  //server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //      request->send(LittleFS, "/index.html", "text/html");
  //});

  server.serveStatic("/assets", LittleFS, "/assets");

  // Startseite
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *r){
    r->send(LittleFS, "/index.html", "text/html");
  });

  // fallback
  server.onNotFound([](AsyncWebServerRequest *r){
    r->redirect("/");
  });
  
}

//Void Set up File paths was here

#pragma endregion

/* #### ~ Justricity ~ #### */
/* ------------------------ */
/*                          */
/*       Driver Logic       */
/*                          */
/* ------------------------ */
/* ######################## */

#pragma region //Display Driver Region 


/* ############## ~ Justricity ~ ############## */
/* -------------------------------------------- */
/*                                              */
/*               Display Helpers                */
/*                                              */
/* -------------------------------------------- */
/* ############################################ */

inline void setChannel(uint8_t pcaIndex, uint8_t channel, bool on) { //PCA Command Helper
  pca[pcaIndex].setChannelPWM(channel, on ? PWM_ON : PWM_OFF);
}


/* ############## ~ Justricity ~ ############## */
/* -------------------------------------------- */
/*                                              */
/*                 Programm API                 */
/*                                              */
/* -------------------------------------------- */
/* ############################################ */

void initDisplays() {
  Wire.begin();

  for (uint8_t i = 0; i < NUM_PCA; i++) {
    pca[i].resetDevices();            
    pca[i].init();         
    pca[i].setPWMFrequency(1000);     
  }

  // Start with everything off
  for (uint8_t i = 0; i < DISPLAY_COUNT; i++) {
    for (uint8_t s = 0; s < 7; s++) {
      setChannel(displays[i].pcaIndex, displays[i].ch[s], false);
    }
  }
}


void setDisplay(DisplayID id, int digit, bool dp = false) { //Applying Masks, Setting Display Digits
  uint8_t mask = 0;

  if (digit >= 0 && digit <= 9) mask = DIGIT_MASK[digit]; //Aplying Digit Mask on Digit

  const DisplayMap &d = displays[id]; //Applying Digit Mask to Display

  // Set Digits A through G
  for (uint8_t s = 0; s < 7; s++) {
    bool on = (mask >> s) & 0x01;
    setChannel(d.pcaIndex, d.ch[s], on);
  }
}

// Convenience Function: Display a double Digit Number 
void show2(DisplayID left, DisplayID right, int value, bool blankLeadingZero = true) {
  int tens = (value / 10) % 10;
  int ones = value % 10;

  if (blankLeadingZero && value < 10) {
    setDisplay(left, -1);   // blank
  } else {
    setDisplay(left, tens);
  }
  setDisplay(right, ones);
}

// Convenience Funtion: Show score from Digits
void showScore(int heim, int gast) {
  show2(D_1_1, D_1_2, heim, true);
  show2(D_2_1, D_2_2, gast, true);
}

// Global Shutoff
void allOff() {
    for (uint8_t i = 0; i < NUM_PCA; i++) {
    for (uint8_t ch = 0; ch < 16; ch++) {
      setChannel(i, ch, false);
    }
  }
}


#pragma endregion




/* #### ~ Justricity ~ #### */
/* ------------------------ */
/*                          */
/*         Program          */
/*                          */
/* ------------------------ */
/* ######################## */

// ======== Runtime state ========
int g_home = 0;
int g_guest = 0;

// ======== Small helpers ========
void setScore(int home, int guest) {
  g_home  = constrain(home, 0, 99);
  g_guest = constrain(guest, 0, 99);

  // deine Display-Funktion
  showScore(g_home, g_guest);
}

void printHelp() {
  Serial.println();
  Serial.println(F("Commands:"));
  Serial.println(F("  HELP                -> show this help"));
  Serial.println(F("  S <home> <guest>    -> set score (0..99)"));
  Serial.println(F("  H <home>            -> set home only"));
  Serial.println(F("  G <guest>           -> set guest only"));
  Serial.println(F("  D <id> <digit>      -> set single display by ID (digit 0..9, -1 blank)"));
  Serial.println(F("  OFF                 -> turn all segments off"));
  Serial.println(F("  TEST                -> run segment/digit test"));
  Serial.println();
}

// ======== Test routine ========
void runDisplayTest() {
  // 1) Jede Anzeige zeigt 0..9
  for (int id = 0; id < (int)DISPLAY_COUNT; id++) {
    for (int d = 0; d <= 9; d++) {
      setDisplay((DisplayID)id, d);
      delay(200);
    }
    setDisplay((DisplayID)id, -1); // blank
  }

  // 2) Score zählt kurz hoch
  for (int i = 0; i <= 20; i++) {
    setScore(i, 20 - i);
    delay(150);
  }

  // 3) Alles aus
  allOff();
}

void setup() {
  //Starting Serial Monitor
  Serial.begin(115200);

  delay (5000);
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("ScoreCore OS V2.0");
  Serial.println("booting . . .");
  
  static uint32_t boots = 0;
  boots++;
  Serial.printf("\nBOOT #%lu\n", (unsigned long)boots);

  //Setting up AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100); 
  yield();

  Serial.print("Accesspoint set up. IP: ");
  Serial.println(WiFi.softAPIP());

  //Initializing File System
  if(LittleFS.begin()){
      Serial.println("Filesystem Initialized");
    }else{
      Serial.println("Error while initializing filesystem");
    }
  
  //Setting up Server and starting it
  setupServerRoutes();
  //setupFilePaths();

  server.begin();

  Serial.println("Server set up");

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); //Starting I²C

  initDisplays();      // Mapping Displays
  setScore(0, 0);      // Default setting

  Serial.println("ScoreCore done booting");
  Serial.println("Happy Game");
  Serial.println(F(R"ASCII(
                       ___
   o__        o__     |   |\
  /|          /\      |   |X\
  / > o        <\     |   |XX\
  )ASCII"));
}


/* #### ~ Justricity ~ #### */
/* ------------------------ */
/*                          */
/*        Serial            */
/*        Interface         */
/*                          */
/* ------------------------ */
/* ######################## */

#pragma region //Serial Interface

String g_line;
void handleSerialLine(const String& line) {
  String s = line;
  s.trim();
  if (s.length() == 0) return;

  // Uppercase copy for command comparison
  String u = s;
  u.toUpperCase();

  // HELP
  if (u == "HELP" || u == "HLP" || u == "?") {
    printHelp();
    return;
  }

  // OFF
  if (u == "OFF") {
    allOff();
    Serial.println(F("OK: allOff()"));
    return;
  }

  // TEST
  if (u == "TEST") {
    Serial.println(F("Running TEST..."));
    runDisplayTest();
    Serial.println(F("TEST done."));
    return;
  }

  // Split first token (command)
  int sp = s.indexOf(' ');
  String cmd = (sp == -1) ? s : s.substring(0, sp);
  cmd.trim();
  String rest = (sp == -1) ? "" : s.substring(sp + 1);
  rest.trim();

  cmd.toUpperCase();

  // S <home> <guest>
  if (cmd == "S") {
    int sp2 = rest.indexOf(' ');
    if (sp2 == -1) {
      Serial.println(F("ERR: use S <home> <guest>"));
      return;
    }
    int home = rest.substring(0, sp2).toInt();
    int guest = rest.substring(sp2 + 1).toInt();
    setScore(home, guest);
    Serial.printf("OK: score %d:%d\n", g_home, g_guest);
    return;
  }

  // H <home>
  if (cmd == "H") {
    int home = rest.toInt();
    setScore(home, g_guest);
    Serial.printf("OK: home=%d guest=%d\n", g_home, g_guest);
    return;
  }

  // G <guest>
  if (cmd == "G") {
    int guest = rest.toInt();
    setScore(g_home, guest);
    Serial.printf("OK: home=%d guest=%d\n", g_home, g_guest);
    return;
  }

  // D <id> <digit>
  if (cmd == "D") {
    int sp2 = rest.indexOf(' ');
    if (sp2 == -1) {
      Serial.println(F("ERR: use D <id> <digit>"));
      return;
    }
    int id = rest.substring(0, sp2).toInt();
    int digit = rest.substring(sp2 + 1).toInt();

    if (id < 0 || id >= (int)DISPLAY_COUNT) {
      Serial.println(F("ERR: id out of range"));
      return;
    }
    if (!(digit >= -1 && digit <= 9)) {
      Serial.println(F("ERR: digit must be -1..9"));
      return;
    }

    setDisplay((DisplayID)id, digit);
    Serial.printf("OK: display %d = %d\n", id, digit);
    return;
  }

  Serial.println(F("ERR: unknown command. Type HELP"));
}

void handleSerial() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (SERIAL_ECHO) Serial.write(c);

    // Zeilenende
    if (c == '\n' || c == '\r') {
      if (g_line.length() > 0) {
        handleSerialLine(g_line);
        g_line = "";
      }
      continue;
    }

    // einfache Zeilenlänge begrenzen
    if (g_line.length() < 120) g_line += c;
  }
}
#pragma endregion

void loop() {

  handleSerial();

}
