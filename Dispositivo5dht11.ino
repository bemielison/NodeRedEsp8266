/* 
 * este sketch lê o estado do sensor de temperatura e umidade DHT-11 e publica em topicos.
 * na demostracao este dispositivo estava localizado na cidade de Russas-CE enquanto o dispositivo 4 estava 
 * na cidade de Pesqueira-PE e os dispositivos 1, 2 e 3 na cidade de Mossoró-RN
 * 
 * Por: Bemielison Bezerra
 * e-mail: bemielison@gmail.com
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <Wire.h>
#define DHTPIN 0
#define DHTTYPE DHT11

//const int DHTPin = 0;// DHT Sensor - GPIO 0 = D3 on ESP-12E  ------- conecte o sensor DHT-11 na porta D3 do ESP
DHT dht (DHTPIN, DHTTYPE);

long now = millis();
long lastMeasure = 0;
int i = 0;


const char* ssid = "     "; // nome da sua rede wifi
const char* password = "      "; // senha da sua rede wifi
const char* mqtt_server = "broker.mqtt-dashboard.com"; // broker mqtt on-line


WiFiClient espClient; PubSubClient client(espClient);

void setup_wifi() {

  delay(10);  Serial.println(); Serial.print("Connecting to "); Serial.println(ssid);
  WiFi.mode(WIFI_STA);  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print(".");  }
  randomSeed(micros());  Serial.println("");  Serial.println("WiFi connected");
  Serial.println("IP address: ");  Serial.println(WiFi.localIP());}

void callback(String topic, byte* message, unsigned int length) {  Serial.print("Message arrived on topic: ");  
  Serial.print(topic);  Serial.print(". Message: "); String messageAtual; 
  for (int i = 0; i < length; i++) {    Serial.print((char)message[i]);    messageAtual += (char)message[i];  }
  Serial.println();
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
     
      client.subscribe("Temperatura_Russas");
      client.subscribe("Umidade_Russas");
                
    } else {
      Serial.print("failed, rc=");      Serial.print(client.state()); Serial.println(" try again in 5 seconds");  delay(5000);
    }
  }
}

void setup() {
 
  dht.begin();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay (100);
  
}

void loop() {
  if (!client.connected()) { reconnect();  }
  client.loop();  now = millis();

 if (now - lastMeasure > 5000) {    lastMeasure = now; // Publishes new temperature and humidity
   float h = dht.readHumidity();   float t = dht.readTemperature();   float f = dht.readTemperature(true);
    
 if (isnan(h) || isnan(t) || isnan(f)) { Serial.println (F("Falha na leitura do sendor DHT11")); return;    }
  float hif = dht.computeHeatIndex(f, h);  float hic = dht.computeHeatIndex(t, h, false);
  static char temperatureTemp[7];  dtostrf(hic, 6, 2, temperatureTemp);  static char humidityTemp[7];  dtostrf(h, 6, 2, humidityTemp);
  
  client.publish("Temperatura_Russas", temperatureTemp); 
  client.publish("Umidade_Russas", humidityTemp);  }
  
}
