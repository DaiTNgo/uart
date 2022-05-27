// Include the Software Serial library
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "DHT.h"

const int DHTPIN = 2;      //Đọc dữ liệu từ DHT11 ở chân 2 trên mạch Arduino
const int DHTTYPE = DHT11; // Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22

DHT dht(DHTPIN, DHTTYPE);

// Define a Software Serial object and the used pins
SoftwareSerial softSerial(8, 9); // RX, TX
void setup()
{
  Serial.begin(115200);
  dht.begin();
  softSerial.begin(9600);
}
void loop()
{
  //  softSerial.println("Ngo Tan Dai");
  float h = dht.readHumidity();    //Đọc độ ẩm
  float t = dht.readTemperature(); //Đọc nhiệt độ

  String json =  "{\"temperature\" : " + String(t) + ", \"humidity\" : " + String(h) + "}";
  softSerial.println(json);
  softSerial.flush();

  delay(1000); //Đợi 1 giây
}
