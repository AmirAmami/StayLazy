/*Pour ce capteur, il faut connecter le capteur à une resitance de 10K avant de le brancher, si vous utilisez une autre resistance
il faut changer les valeurs dans la condition if qui renvoie si c'est la journee ou la nuit */
#include "PubSubClient.h"
#include "ESP8266WiFi.h" 
 
int photocellPin = 0;     // peut etre changer selon le PIN utilisé
int photocellReading;     // valeur dans laquelle on stocke les valeurs renvoyées par le capteur
 
//Definition des paramètres du point d'accès
char ssid[] = "................";           // A determiner lors de la configuration du point d'accees
char wifi_password[] = ".............."; // A determiner lors de la configuration du point d'accees

//Definition des paramètres de la communication MQTT
char mqtt_server[] = "10.10.10.10";  // IP of the MQTT broker
char periode_topic[] = "Chambre/periode";
char mqtt_username[] = "............."; // MQTT username
char mqtt_password[] = "............"; // MQTT password
char clientID[] = "client_test"; // MQTT client ID

// Creation d'un client WiFi et d'un client MQTT a partir des paramètres çi-dessus
WiFiClient wifiClient;

PubSubClient client(mqtt_server, 1883, wifiClient); 

// Fonction pour se connecter au point d'accès puis se connecter au gestionnaire de messages Mosquitto 
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //Connection au Point d'accès
  WiFi.begin(ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connection au gestionnaire de messages
  // client.connect retourne un booléen 
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void setup(void) {
  Serial.begin(9600);   
}
 
void loop(void) {
  photocellReading = analogRead(photocellPin);  
  string per;
  // La condition ci dessous determine si c'est la journee ou la nuit en se basant sur les valeurs renvoyées par le capteur
  if (photocellReading < 150) {
    per="nuit";
    } else {
    per="jour";
  }
//Publier le message selon le sujet periode
// client.publish retourne un booléen
  if (client.publish(periode_topic, String(per).c_str())) {
    Serial.println("information sent!");
  }
//Réessayer de se publier en commençant par établir une nouvelle connection car il y a une probabilité que la connection précedente s'est interrompue
  else {
    Serial.println("information failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(periode_topic, String(per).c_str());
  }

//Deconnection du gestionnaire de messages
  client.disconnect();
//entrer en mode sleep pour économiser l'utilisation de la batterie
  Serial.println("I'm awake, but I'm going into deep sleep mode for 10 seconds");
  ESP.deepSleep(10e6);
}