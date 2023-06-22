// ############# LIBRARIES ############### //

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <stdio.h>
#include <stdlib.h>

#include <WiFiClient.h>
//WiFiClient wifiClient;

// ############# VARIABLES ############### //

const char* SSID = "BocaBrida-2G"; // rede wifi
const char* PASSWORD = "total123**"; // senha da rede wifi
String BASE_URL = "http://192.168.1.24";

//const char* SSID = "Felipe-Network"; // rede wifi
//const char* PASSWORD = "Felipe2050"; // senha da rede wifi
//String BASE_URL = "http://192.168.123.73";

// ############# PROTOTYPES ############### //

void initSerial();
void initWiFi();
void httpRequest(String path);

// ############### OBJECTS ################# //

WiFiClient client;
HTTPClient http;

// ############### Pinos ################# //

#define relay1 5
#define relay2 4
#define relay3 0
#define relay4 2
#define relay5 14
#define relay6 12

void setup() {
  initSerial();
  initWiFi();

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(relay5, OUTPUT);
  pinMode(relay6, OUTPUT);

  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
  digitalWrite(relay5, HIGH);
  digitalWrite(relay6, HIGH);
}

// ############### LOOP ################# //

void loop() {
  Serial.println("[GET] /sensors - sending request...");
  Serial.println("");

  httpRequest("/fireEffects/");

  Serial.println("");
  delay(1000);
}

// ############# HTTP REQUEST ################ //

void httpRequest(String path)
{
  if (WiFi.status() != WL_CONNECTED) {
    initWiFi();
  }

  String payload = makeRequest(path);

  if (!payload) {
    return;
  }

  Serial.println(payload);

  char data[256];
  strcpy(data, payload.c_str());
  int status1 = data[12] - '0';
  int status2 = data[27] - '0';
  int status3 = data[42] - '0';
  int status4 = data[57] - '0';
  int status5 = data[72] - '0';
  int status6 = data[87] - '0';

  Serial.println(status1);
  Serial.println(status2);
  Serial.println(status3);
  Serial.println(status4);
  Serial.println(status5);
  Serial.println(status6);

  if (status1 == 1) {
    digitalWrite(relay1, LOW);
  } else {
    digitalWrite(relay1, HIGH);    
  }

  if (status2 == 1) {
    digitalWrite(relay2, LOW);
  } else {
    digitalWrite(relay2, HIGH);    
  }

  if (status3 == 1) {
    digitalWrite(relay3, LOW);
  } else {
    digitalWrite(relay3, HIGH);    
  }

  if (status4 == 1) {
    digitalWrite(relay4, LOW);
  } else {
    digitalWrite(relay4, HIGH);    
  }

  if (status5 == 1) {
    digitalWrite(relay5, LOW);
  } else {
    digitalWrite(relay5, HIGH);    
  }

  if (status6 == 1) {
    digitalWrite(relay6, LOW);
  } else {
    digitalWrite(relay6, HIGH);    
  }
}

String makeRequest(String path)
{
  
  if (http.begin(client, BASE_URL + path)){
   int httpCode = http.GET();

   if (httpCode < 0) {
     Serial.println("request error - " + httpCode);
     Serial.println(httpCode);
     return "";

   }

   if (httpCode != HTTP_CODE_OK) {
    return "";
   }

  String response =  http.getString();
  http.end();
  return response;
    } else{ Serial.println("begin error"); return "";
  }
}

// implementacao dos prototypes

void initSerial() {
  Serial.begin(115200);
}

void initWiFi() {
  delay(10);
  Serial.println("Conectando-se em: " + String(SSID));

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado na Rede " + String(SSID) + " | IP => ");
  Serial.println(WiFi.localIP());
}