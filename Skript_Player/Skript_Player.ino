#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

// LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DFPlayer Mini Setup
SoftwareSerial mySoftwareSerial(D6, D7); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

ESP8266WebServer server(80);

const int switchPin = D3;

// WLAN-Settings
const char* ssid = "yourSSID";
const char* password = "yourPASSWORD";

// AP-Settings 
const char* ap_ssid = "ESP_AP";
const char* ap_password = "configportal";

bool isAPMode = false;

void setup() {
  Serial.begin(115200);
  
  // LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initialisierung");

  // DFPlayer
  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    lcd.setCursor(0, 1);
    lcd.print("DFPlayer Fehler");
    while (true);
  }
  myDFPlayer.setTimeOut(500);
  myDFPlayer.volume(10);
  myDFPlayer.play(1);

  pinMode(switchPin, INPUT_PULLUP);

  WiFiManager wifiManager;

  if (digitalRead(switchPin) == LOW) {
    // AP
    WiFi.softAP(ap_ssid, ap_password);
    isAPMode = true;
  } else {
    // Client
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    isAPMode = false;
  }

  server.on("/", handleRoot);
  server.on("/play", handlePlay);
  server.on("/pause", handlePause);
  server.on("/stop", handleStop);
  server.on("/next", handleNext);
  server.on("/previous", handlePrevious);
  server.on("/title", handleTitle);

  server.begin();
  Serial.println("HTTP server started");

  updateLCD();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>ESP8266 MP3 Player</h1>";
  html += "<p><a href='/play'>Play</a></p>";
  html += "<p><a href='/pause'>Pause</a></p>";
  html += "<p><a href='/stop'>Stop</a></p>";
  html += "<p><a href='/next'>Next</a></p>";
  html += "<p><a href='/previous'>Previous</a></p>";
  html += "<p>Current Title: <a href='/title'>Show</a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handlePlay() {
  myDFPlayer.start();
  server.send(200, "text/plain", "Play");
}

void handlePause() {
  myDFPlayer.pause();
  server.send(200, "text/plain", "Pause");
}

void handleStop() {
  myDFPlayer.stop();
  server.send(200, "text/plain", "Stop");
}

void handleNext() {
  myDFPlayer.next();
  server.send(200, "text/plain", "Next");
}

void handlePrevious() {
  myDFPlayer.previous();
  server.send(200, "text/plain", "Previous");
}

void handleTitle() {
  String title = getMP3Title();
  server.send(200, "text/plain", title);
}

String getMP3Title() {
  int currentFile = myDFPlayer.readCurrentFileNumber();
  char title[30];
  sprintf(title, "Titel %d", currentFile);
  return String(title);
}

void updateLCD() {
  lcd.clear();
  if (isAPMode) {
    lcd.setCursor(0, 0);
    lcd.print("SSID: ");
    lcd.print(ap_ssid);
    lcd.setCursor(0, 1);
    lcd.print("PW: ");
    lcd.print(ap_password);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("IP: ");
    lcd.print(WiFi.localIP());
  }
}
