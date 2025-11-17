// Trabalho de Julya: sensor de gás com led, buzzer e mensagem no zap
// https://github.com/LuanBispo/WhatsAppESP32 (dúvidas sobre a api do callmebot)
// callmebot atualizado: +34 644 94 58 67
//

#include <WiFi.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

//REDE WIFI
const char* ssid     = "DTEL_TAMARA";
const char* password = "stitch2019";

bool flag = 1; //QUANTIDADE DE MENSAGENS ENVIADAS

void sendMessage(String message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi não conectado. Não foi possível enviar mensagem.");
    return;
  }

  String url = "https://api.callmebot.com/whatsapp.php?phone=558187005616&text=" + urlEncode(message) + "&apikey=7075196";

  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200) {
    Serial.print("Mensagem enviada com sucesso");
  } else {
    Serial.println("Erro no envio da mensagem");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}

// DEFINIÇÕES DE PINOS
#define pinSensorGas 34
#define pinLed       26
#define pinBuzzer    27

// DEFINIÇÕES
#define VAZAMENTO 1200

// DECLARAÇÃO DE FUNÇÕES
void disparaAlerta(byte pin, int intervalo);
void desligaAlerta(byte pin);

void setup() {
  Serial.begin(9600);
  pinMode(pinSensorGas, INPUT);
  pinMode(pinLed, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  Serial.println("Esperando o sensor aquecer um pouco...");
  delay(10000);
  Serial.println("Fim do setup");

  WiFi.begin(ssid, password);
  Serial.println("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado ao WiFi neste IP ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int nivelGas = analogRead(pinSensorGas);

  // Média a cada 1 s (opcional, só para monitorar)
  static unsigned long t0 = millis();
  static long soma = 0;
  static int n = 0;

  soma += nivelGas; 
  n++;

  if (millis() - t0 >= 1000) {
    int media = soma / n;
    Serial.print("Média = ");
    Serial.println(media);
    soma = 0; 
    n = 0; 
    t0 = millis();
  }

  if (nivelGas >= VAZAMENTO){
    disparaAlerta(pinLed, 1000);
    disparaAlerta(pinBuzzer, 1000);

    Serial.println("VAZAMENTO DETECTADO!");
    sendMessage("VAZAMENTO DE GÁS DETECTADO!");

  } else {
    desligaAlerta(pinLed);
    desligaAlerta(pinBuzzer);
  }
}

// IMPLEMENTO DE FUNÇÕES
void disparaAlerta(byte pin, int intervalo){
  static bool nivel = HIGH;
  static unsigned long ultimaTroca = 0;

  if (millis() - ultimaTroca > (unsigned long)intervalo){
    nivel = !nivel;
    ultimaTroca = millis();
    Serial.println("DISPARO");
  }
  digitalWrite(pin, nivel);
}

void desligaAlerta (byte pin){
  digitalWrite(pin, LOW);
}
