/*
 * Este sketch lê o estado de um botao e publica em topicos
 * 
 * Por Bemielison Bezerra
 * e-mail: bemielison@gmail.com
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>


const int PinoBotao = 2; //D4 acionar rele botao fisico
int BotaoEstado = 0;
int auxiliar = 0;
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
     
      //topicos em que o dispositivo esta inscrito
      client.subscribe("IluminacaoBotao");
      client.subscribe("IluminacaoManual");
      client.subscribe("Iluminacao");
      
    } else {
      Serial.print("failed, rc=");      Serial.print(client.state()); Serial.println(" try again in 5 seconds");  delay(5000);
    }
  }
}

void setup() {
 
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay (100);
  pinMode (PinoBotao, INPUT);
}

void loop() {
  if (!client.connected()) { reconnect();  }
  client.loop(); 

  //rotina para ler o estado do botao e publicar nos topicos IluminacaoManual,...
  BotaoEstado = digitalRead (PinoBotao);
  if (BotaoEstado == LOW) {
     switch (auxiliar){
          case 0:
             auxiliar = 1;
             Serial.print ("Botao Pressionado");
             client.publish("IluminacaoManual", "Acionado");
             client.publish("IluminacaoBotao", "Ligar"); 
             client.publish("Iluminacao", "Ligar"); 
             delay(500);
             client.publish("IluminacaoManual", "Em Espera");
             break;
          case 1:       
             auxiliar = 0;
             Serial.print ("Botao Pressionado");
             client.publish("IluminacaoManual", "Acionado");
             client.publish("IluminacaoBotao", "Desligar");  
             client.publish("Iluminacao", "Ligar"); 
             delay(500);
             client.publish("IluminacaoManual", "Em Espera");
             break;
     } 
  
  }      
}

 
