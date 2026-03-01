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
constexpr uint8_t NUM_PCA = 2; //Number of connected drivers
constexpr uint8_t PCA_ADDR[NUM_PCA] = { 0x40, 0x41 }; //Adresses of connected drivers
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
struct DisplayMap {
  uint8_t pca;
  uint8_t ch[7];   // A..G
};
constexpr DisplayMap displays[DISPLAY_COUNT] = { //Driver wiring
  // D_1_1  (PCA index 0, display 1)
  { 0, { 0, 1, 2, 3, 4, 5, 6} },

  // D_1_2  (PCA index 0, display 2)
  { 0, { 7, 8, 9, 10, 11, 12, 13} },

  // D_2_1  (PCA index 1, display 1)
  { 1, { 0, 1, 2, 3, 4, 5, 6} },

  // D_2_2  (PCA index 1, display 2)
  { 1, { 8, 9, 10, 11, 12, 13} },
};


AsyncWebServer server(80);
IPAddress apIP(192,168,4,1);
const char* espHostname   = "scorecore";

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


//Driver logic was here


/* #### ~ Justricity ~ #### */
/* ------------------------ */
/*                          */
/*         Program          */
/*                          */
/* ------------------------ */
/* ######################## */
void setup() {
  //Starting Serial Monitor
  Serial.begin(115200);

  delay (1000);
  
  static uint32_t boots = 0;
  boots++;
  Serial.printf("\nBOOT #%lu\n", (unsigned long)boots);

  //Setting up AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  WiFi.softAP(AP_SSID, AP_PASS);
  delay(100); 
  yield();

  Serial.print("AP IP: ");
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

  Serial.println("System set up and running");
}

void loop() {


}
