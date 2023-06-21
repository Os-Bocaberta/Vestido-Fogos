// ############# LIBRARIES ############### //

#include <WiFi.h>
#include <HTTPClient.h>
#include <stdio.h>
#include <stdlib.h>
#include <FastLED.h>

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

// ############## SETUP ################# //

#define NUM_LEDS 300         // Defina o número total de LEDs que você está usando
#define DATA_PIN 15          // Pino de dados ao qual os LEDs estão conectados
#define COLOR_ORDER GRB     // Ordem das cores dos LEDs
#define FADE_DURATION 1200  // Duração da respiração em milissegundos
#define COLOR_ORDER GRB     // Ordem das cores dos LEDs

CRGB leds[NUM_LEDS];

int status_save;

// Paleta de cores do fogo
const CRGBPalette16 fireColorsPalette = {
  CRGB::Black,
  CRGB::Black,
  CRGB::Red,
  CRGB::Orange,
  CRGB::Yellow,
  CRGB::White,
  CRGB::White,
  CRGB::White,
  CRGB::White,
  CRGB::Yellow,
  CRGB::Orange,
  CRGB::Red,
  CRGB::Black,
  CRGB::Black,
  CRGB::Black,
  CRGB::Black
};

void setup() {
  initSerial();
  initWiFi();
  
  FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);  // Defina o tipo de LED e as configurações
}

// ############### LOOP ################# //

void loop() {
  Serial.println("[GET] /sensors - sending request...");
  Serial.println("");

  httpRequest("/dress/");

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

  char data[25];
  strcpy(data, payload.c_str());
  int status = data[12] - '0';

  Serial.println(status);

  if(status != 0 && status != 1 && status != 2 && status != 3 && status != 4) {
    status = status_save;
  }

  if (status == 0) {
    status_save = status;
    FastLED.clear();
    FastLED.show();
  }

  if (status == 1) {
    status_save = status;
    rainbowEffect(20);
  }

  if (status == 2) {
    status_save = status;
    fireEffect(20);
  }

  if (status == 3) {
    status_save = status;
    spectrumEffect();
  }

  if (status == 4) {
    status_save = status;
    heartBeatEffect(CRGB::Red, FADE_DURATION);
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

// ##############Effects################ //

// ##############Rainbow Effect################# //

void rainbowEffect(int speed) {
  static uint8_t startIndex = 0;

  // Cria um efeito de arco-íris
  fill_rainbow(leds, NUM_LEDS, startIndex, 255 / NUM_LEDS);

  FastLED.show();
  FastLED.delay(speed);

  startIndex++;
}


// ##############Fire Effect################# //

void fireEffect(int cooling) {
  static byte heat[NUM_LEDS];

  // Resfriamento de todos os pixels
  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random(0, ((cooling * 10) / NUM_LEDS) + 2));
  }

  // Propagação do fogo
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Efeito de chama na primeira linha de LEDs
  int flicker = random(160);

  for (int j = 0; j < NUM_LEDS / 2; j++) {
    byte index = scale8(random8(flicker), 240);
    leds[j] = ColorFromPalette(fireColorsPalette, heat[j] + index);
  }

  // Efeito de chama na segunda linha de LEDs (espelhada)
  for (int j = NUM_LEDS / 2; j < NUM_LEDS; j++) {
    leds[j] = leds[NUM_LEDS - j - 1];
  }

  FastLED.show();
  delay(30);
}

// ##############Spectrum Effect################# //

void spectrumEffect() {
    for (int hue = 0; hue < 255; hue++) {
    // Define a cor com base no valor de 'hue'
    CRGB cor = CHSV(hue, 255, 255);

    // Atribui a cor a todos os LEDs
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = cor;
    }

    FastLED.show();  // Mostra as cores definidas nos LEDs
    delay(10);       // Aguarda um curto período de tempo antes de atualizar as cores
  }
}

// ##############Heartbeat Effect################# //

void heartBeatEffect(const CRGB& color, int duration) {
  static unsigned long startTime = 0;

  if (millis() - startTime >= duration) {
    startTime = millis();
  }

  float elapsed = millis() - startTime;
  float progress = elapsed / duration;
  float brightness = (1 - cos(progress * PI)) * 0.7;  // Função de respiração (cos^2)

  CRGB adjustedColor = color;
  adjustedColor.nscale8_video(brightness * 255);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = adjustedColor;
  }

  FastLED.show();
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