#include <Arduino.h>
#include <SoftwareSerial.h>
#include <WiFly.h>
#include "HTTPClient.h"
#include<stdlib.h> 

#define SSID      "MiPhone"
#define KEY       "ruban123"
// WIFLY_AUTH_OPEN / WIFLY_AUTH_WPA1 / WIFLY_AUTH_WPA1_2 / WIFLY_AUTH_WPA2_PSK
#define AUTH      WIFLY_AUTH_WPA2_PSK

const unsigned long postingInterval = 10L * 30000L; //30Second
unsigned long lastConnectionTime = 0, startTime = 0;

SoftwareSerial uart(2, 3);
WiFly wifly(uart);
HTTPClient http;
char get;

#include <dht.h>

#define dht_dpin A1 //no ; here. Set equal to channel sensor is on

dht DHT;
 
int measurePin = 5;
int ledPower = 12;
 
int samplingTime = 3600;
int deltaTime = 40;
int sleepTime = 9680;
 
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0.0;
float temp = 0.0;
float humidity = 0.0;
 
void setup(){
  Serial.begin(9600);
  pinMode(ledPower,OUTPUT);

  Serial.println("------- WIFLY HTTP --------");
  
  
  uart.begin(9600);         // WiFly UART Baud Rate: 9600
  // Wait WiFly to init
 delay(3000);

  // check if WiFly is associated with AP(SSID)
  if (!wifly.isAssociated(SSID)) {
    while (!wifly.join(SSID, KEY, AUTH)) {
      Serial.println("Failed to join " SSID);
      Serial.println("Wait 0.1 second and try again...");
      delay(100);
    }
    
    wifly.save();    // save configuration, 
}
}
 
void loop(){
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);
 
  voMeasured = analogRead(measurePin); // read the dust value
 
  delayMicroseconds(deltaTime);
  DHT.read11(dht_dpin);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
 
  // 0 - 3.3V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (3.3 / 1024);
 
  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 0.17 * calcVoltage - 0.1;

  char duststr[10];
  //float d = dustDensity;
  float d = 1.23;
  dtostrf(d, 6, 2, duststr);
 
  Serial.print("Dust Density: ");
  Serial.print(duststr);

  char humiditystr[10];
  float h = DHT.humidity;
  //float h = 98.00;
  dtostrf(h, 6, 2, humiditystr);
  
  Serial.print(" - Humidity: ");
  Serial.print(humiditystr);

  char tempstr[10];
  float t = DHT.temperature;
  //float t = 25.00;
  dtostrf(t, 6, 2, tempstr);

  Serial.print(" - Temp: "); 
  Serial.println(tempstr); 
 
  delay(1000);


  String url   = "http://zyten.xyz/testo/transitThingSpeak.php?temp=";
  String url2  = "&humidity=";
  String url3  = "&dust=";
  String myURL = url + tempstr+ url2 + humiditystr + url3 + duststr;
  //myURL = myURL.simplified();
  myURL.replace( " ", "" );
  const char * HTTP_GET_URL = myURL.c_str();

  //Serial.println(HTTP_GET_URL);
  
  //const char *HTTP_GET_URL = "http://zyten.xyz/testo/transitThingSpeak.php?temp=80.00&humidity=60.00&dust=1.20";

  //#define HTTP_GET_URL "http://api.thingspeak.com/update?key=MECXDF1CRRLAKISK&temp=12.00&humidity=55.00&dust=0.25"
  Serial.print("\r\nTry to get url - "); Serial.println(HTTP_GET_URL); 
  Serial.println("------------------------------");
  delay(30000);
  while (http.get(HTTP_GET_URL, postingInterval) < 0) {
  }
  while (wifly.receive((uint8_t *)&get, 1, 1000) == 1) {
    Serial.print(get);
  }
  delay(30000);

  if (wifly.commandMode()) {
    Serial.println("\r\n\r\nEnter command mode. Send \"exit\"(with \\r) to exit command mode");
  }
  
  int c;
  while (wifly.available()) {
    c = wifly.read();
    if (c > 0) {
      Serial.write((char)c);
    }
  }
}
