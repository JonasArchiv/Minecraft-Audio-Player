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

// AP-Settigs
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
