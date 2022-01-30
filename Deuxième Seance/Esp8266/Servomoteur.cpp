
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
//Definition des paramètres du point d'accès
const char* ssid = "..................";
const char* password = "...............";

//Definition des paramètres de la communication MQTT
const char* mqtt_server = "10.10.10.10";

// Creation d'un client WiFi 
WiFiClient espClient;
PubSubClient client(mqtt_server, 1883,espClient);


void setup_wifi() {
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
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// Cette fonction est exécutée lorsque le Raspberry publie un message dans un topic auquel l'ESP8266 est abonné.
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(Chambre/volet);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

// Cett fonction reconnect l'ESP au gstionnaire de message
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // S'abonner ou se reabonner a un sujet
      client.subscribe("Chambre/volet");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Créer un objet Servo pour contrôler le servomoteur 
Servo monServomoteur;
void setup() {

  // Attache le servomoteur à la broche D9
  monServomoteur.attach(9);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");

  if(messageTemp=="1"){
    // Fait bouger le bras de 0° à 180°
    for (unsigned long position = 1000; position <= 2000; position += 5) {
        monServomoteur.writeMicroseconds(position);
        delay(15);
    }
    }else if(messageTemp == "0"){
      // Fait bouger le bras de 180° à 0°
      for (unsigned long position = 2000; position >= 1000; position -= 5) {
      monServomoteur.writeMicroseconds(position);
      delay(15);
  }
  }
}

