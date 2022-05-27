#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define RX 16
#define TX 17

#define WIFI_SSID "Keobongmay"
#define WIFI_PASSWORD "135792468"
#define API_KEY "AIzaSyDCWKOVcceLB-iuc4gMIErYAfGbOxQ7ldc"
#define DATABASE_URL "https://esp32-firebase-demo-2d982-default-rtdb.asia-southeast1.firebasedatabase.app"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

bool stringComplete = false;
String inputString = "";
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void ReadSensor();
void TestFirebase();

void setup()
{

  Serial.begin(115200);
  Wire.begin();
  //------LCD-------
  lcd.init();
  lcd.backlight();
  lcd.begin(16, 2);
  //------LCD-------

  //------UART------
  //  Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial2.begin(9600, SERIAL_8N1, RX, TX);
  Serial.println("Serial Txd is on pin: " + String(TX));
  Serial.println("Serial Rxd is on pin: " + String(RX));
  //------UART------

  //------WIFI------
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  //------WIFI------

}

void loop()
{
//  ReadSensor();
  TestFirebase();
}

void ReadSensor() {
  // put your main code here, to run repeatedly:
  //  while (Serial.available())
  while (Serial2.available())
  {
    //    char inChar = (char)Serial.read();
    char inChar = (char)Serial2.read();
    inputString += inChar;
    if (inChar == '\n')
    {
      stringComplete = true;
    }
    if (stringComplete)
    {
      Serial.print("Data nhan: ");
      Serial.println(inputString);

      DynamicJsonDocument LCD(256);
      deserializeJson(LCD, inputString);
      Serial.print("chuỗi LCD Là:");
      // in ra kết qua json
      serializeJson(LCD, Serial);
      Serial.println("\n");

      float t = LCD["temperature"];
      float h = LCD["humidity"];

      Serial.print("Nhiet do :");
      Serial.println(t);
      lcd.setCursor(0, 0);
      lcd.print(t);
      lcd.setCursor(0, 1);
      lcd.print(h);
      Serial.print("Do am:");
      Serial.println(h);
      inputString = "";
      stringComplete = false;
    }
  }
}
void TestFirebase() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "test/int", count)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;

    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", 0.01 + random(0, 100))) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
