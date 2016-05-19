/*
 Dust Monitoring System with Respiratory Health Care DSS
 #dmon-duino
 
 Program by Ruban Selvarajah
 Written February 2016

 http://arduinodev.woofex.net/2012/12/01/standalone-sharp-dust-sensor/

 COMPONENTS
 Arduino Uno
 Sharp Optical Dust Sensor – Datasheet (http://www.sparkfun.com/datasheets/Sensors/gp2y1010au_e.pdf)
 DHT-11 - Datasheet (http://www.micropik.com/PDF/dht11.pdf)
 SeeedStudio Wifi Shield 2.0 - Datasheet (http://www.seeedstudio.com/wiki/Wifi_Shield_V2.0)
 220 uF Capacitor
 150 Ω Resistor
 Breadboard
 M/M jumper cables

 SETUP
 Sharp pin 1 (V-LED)   => 3.3V (connected to 150ohm resister)
 Sharp pin 2 (LED-GND) => Arduino GND pin
 Sharp pin 3 (LED)     => Arduino dpin 12
 Sharp pin 4 (S-GND)   => Arduino GND pin
 Sharp pin 5 (Vo)      => Arduino pin A4
 Sharp pin 6 (Vcc)     => 3.3V (direct)

 DHT Dat => Arduino pin A1
 DHT Vcc => 3.3V
 DHT Gnd => Arduino GND

 SETUP DIAGRAM: http://github.com/Zyten/dmon-web/diagrams/SETUP.png

 
 */

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
#define dht_dpin A1

dht DHT;
 
int measurePin = 4;
int ledPower = 12;
 
int samplingTime = 280;
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

  Serial.println("------- REALTIME D-MON --------");
  
  //Initialise wireless connection
  uart.begin(9600);         // WiFly UART Baud Rate: 9600
  // Wait for WiFly to start
  delay(3000);
  Serial.println("Attemping connection to " SSID " ..");
  // check if WiFly is associated with AP(SSID)
  if (!wifly.isAssociated(SSID)) {
    while (!wifly.join(SSID, KEY, AUTH)) {
      Serial.println("Failed to join " SSID);
      Serial.println("Wait 0.1 second and try again...");
      delay(100);
    }
   Serial.println("Connected to " SSID "."); 
   wifly.save(); // save wifi configuration
  }
}
 
void loop(){
  //Read Dust Sensor
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);
 
  voMeasured = analogRead(measurePin); // read the dust value
 
  delayMicroseconds(deltaTime);
  
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
 
  // 0 - 3.3V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);
 
  //Apply linear equation to approximate values to match Output Voltage vs. Dust Density graph in datasheet
  dustDensity = (calcVoltage - 0.6)/6*1000;
  
  if (dustDensity < 0 )
      dustDensity = 0;
  
  //Read DHT-11
  DHT.read11(dht_dpin);

  //Stringify float values to ensure that they are url-friendly
  char duststr[10];
  float d = dustDensity;
  dtostrf(d, 6, 2, duststr);
 
  Serial.print("\r\nDust Density: ");
  Serial.print(duststr);

  char humiditystr[10];
  float h = DHT.humidity;
  dtostrf(h, 6, 2, humiditystr);
  
  Serial.print(" - Humidity: ");
  Serial.print(humiditystr);

  char tempstr[10];
  float t = DHT.temperature;
  dtostrf(t, 6, 2, tempstr);

  Serial.print(" - Temp: "); 
  Serial.println(tempstr); 
 
  delay(1000);

  //Prepare GET url
  String url   = "http://188.166.224.15/dmon-duino/transitThingSpeak.php?temp=";
  String url2  = "&humidity=";
  String url3  = "&dust=";
  String myURL = url + tempstr+ url2 + humiditystr + url3 + duststr;
  myURL.replace( " ", "" );
  const char * HTTP_GET_URL = myURL.c_str();

  Serial.print("\r\nAttemping transmission to - "); Serial.println(HTTP_GET_URL); 
  Serial.println("------------------------------");
  while (http.get(HTTP_GET_URL, postingInterval) < 0) {
  }
  while (wifly.receive((uint8_t *)&get, 1, 1000) == 1) {
    Serial.print(get);
  }

  //Command mode for debug purposes
  if (wifly.commandMode()) {
    //Serial.println("\r\n\r\nEnter command mode. Send \"exit\"(with \\r) to exit command mode");
  }

  //Print response from server
  int c;
  while (wifly.available()) {
    c = wifly.read();
    if (c > 0) {
      Serial.write((char)c);
    }
  }

  Serial.print("\r\nWait for 30 seconds before next reading.");
  delay(30000);
}
