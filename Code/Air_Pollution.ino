#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "MQ135.h"
#include <Arduino.h>
#include "DHT.h"

#define D1 5
#define D2 4
#define D4 2
#define D3 0
#define DHTPIN 14
#define DHTTYPE DHT11

// assign the SPI bus to pins
#define BMP_SCK D1
#define BMP_MISO D4
#define BMP_MOSI D2
#define BMP_CS D3

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO, BMP_SCK);
DHT dht(DHTPIN, DHTTYPE);
unsigned long delayTime;
float h, t, p,  dp;
char temperatureFString[6];
char dpString[6];
char humidityString[6];
char pressureString[7];
String apiKey = "T628M0MQYLRH4JY1";
const char* ssid = "siddhi";
const char* password = "siddhi14";
const char* server = "api.thingspeak.com";
WiFiClient client;

void setup() {
  Serial.begin(115200);   
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);  
  WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());    
  delay(500);
  dht.begin();
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    return;
  }
}

void loop() {
      MQ135 gasSensor = MQ135(A0);
      float air_quality = gasSensor.getPPM();
      Serial.print("Air Quality: ");  
      Serial.print(air_quality);
      Serial.println("  PPM");   
      Serial.println();
      //delay(2000);
      h = dht.readHumidity();
      t = bmp.readTemperature();
      t = t*1.8+32.0;
      dp = t-((100.0-h)/5);    
      p = bmp.readPressure()/100.0F;
      dtostrf(t, 5, 1, temperatureFString);
      dtostrf(h, 5, 1, humidityString);
      dtostrf(p, 6, 1, pressureString);
      dtostrf(dp, 5, 1, dpString);
      Serial.print("Temperature = ");
      Serial.println(temperatureFString);
      Serial.print("Humidity = ");
      Serial.println(h,1);
      Serial.print("Pressure = ");
      Serial.println(pressureString);
      Serial.print("Dew Point = ");
      Serial.println(dp,1);
      Serial.println("...............................................");

      if (client.connect(server,80))  // "184.106.153.149" or api.thingspeak.com
     {
        String postStr = apiKey;
        postStr +="&field1=";
        postStr += String(air_quality);
        postStr +="&field2=";
        postStr += String(pressureString);
        postStr +="&field3=";
        postStr += String(humidityString);
        postStr +="&field4=";
        postStr += String(dpString);
        postStr +="&field5=";
        postStr += String(temperatureFString);
        postStr += "\r\n\r\n";

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr); 
    }   
    client.stop();  
}  
