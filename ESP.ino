#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "GravityTDS.h"
#define TdsSensorPin A1
GravityTDS gravityTds;
float temperature = 25, tdsValue = 0;
int pH_Value;
float Voltage;
float calibration_value = 21.34 - 0.7;

const char* ssid     = "Mi10T";
const char* password = "123456789";

const char* host = "35.228.160.214"; // only electronics-fun.com not https://electronics-fun.com

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //TDS Sensor
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);
  gravityTds.setAdcRange(1024);
  gravityTds.begin();

  pinMode(pH_Value, INPUT);
}

void loop() {
  delay(5000);

  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  const int httpPort = 80; // 80 is for HTTP
  
  client.setInsecure(); // this is the magical line that makes everything work
  
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }

  //Reading Temperature
  temp_Value = analogRead(A2);
  float volt=(float)temp_Value*5.0/1024/6;
  float temp_act = 5.0 * volt;

  //Reading TDS
  gravityTds.setTemperature(temp);
  gravityTds.update();
  tdsValue = gravityTds.getTdsValue();
  Serial.print(tdsValue, 0);

  //Reading pH
  pH_Value = analogRead(A0);
  float volt=(float)pH_Value*5.0/1024/6;
  float ph_act = 5.0 * volt;
  delay(1000);

  // We now create a URI for the request
  String url = "/index.php?tds="+string(tdsValue)+"&temp="+string(temp_act)+"&ph="+string(ph_act);

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

  Serial.println();
  Serial.println("Closing connection");
}