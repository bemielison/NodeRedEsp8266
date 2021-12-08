/* 
 *  Este sketch ler o estado do sensor ultrasom HC-SR04 e publica
 *  no topico Distancia.
 *  
 *  Por Bemielison Bezerra
 * e-mail: bemielison@gmail.com
 * 
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Ultrasonic.h>


#define pino_trigger 4
#define pino_echo 5
Ultrasonic ultrasonic(pino_trigger, pino_echo);

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
    Serial.print("Attempting MQTT connection...");  String clientId = "ESP8266Client-"; clientId += String(random(0xffff), HEX);
   
    if (client.connect(clientId.c_str())) {  Serial.println("connected");
    
      client.subscribe("Distancia"); // topico em que o dispositivo esta inscrito
      
    } else {  Serial.print("failed, rc=");      Serial.print(client.state()); Serial.println(" try again in 5 seconds");  delay(5000);  }}
}

void setup() {
  Serial.begin(115200);  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setServer(mqtt_server, 8883);
  client.setCallback(callback);  delay (100);
}

void loop() {
  if (!client.connected()) { reconnect();  }  client.loop(); 
  float cmMsec;  long microsec = ultrasonic.timing();  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);  Serial.print("Distancia em cm: ");  Serial.print(cmMsec); delay(1000);
  static char DistString[7];
  dtostrf(cmMsec, 6, 2, DistString);
  client.publish("Distancia",DistString); // publicando no topico Distancia a informacao contida em DistString
 

}
