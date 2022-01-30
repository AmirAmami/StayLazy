#include "PubSubClient.h"
#include "ESP8266WiFi.h" 


int capteur=3; // peut etre changer selon le PIN utilisé
int detection;


//Definition des paramètres du point d'accès
char ssid[] = "................";           // A determiner lors de la configuration du point d'accees
char wifi_password[] = ".............."; // A determiner lors de la configuration du point d'accees

//Definition des paramètres de la communication MQTT
char mqtt_server[] = "10.10.10.10";  // IP of the MQTT broker
char porte_topic[] = "Chambre/porte";
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

void setup() {
    pinMode(capteur,INPUT); // broche "capteur" (3) en ENTREE
}
    
void loop() {
    string etat;
    detection=digitalRead(capteur); // on lit la broche capteur. Si il y a un obstacle devant la porte detection=0 sinon ele egal à 1
    // la sortie OUT du capteur est active sur niveau bas
    while (detection==0) { // objet détecté = niveau 0 sur OUT capteur
    detection=digitalRead(capteur);
    etat = "ouverte"
    }
    digitalWrite(sortie,0); // on éteint L dès qu'il n'y a plus detection
    etat = "fermée"


//Publier le message selon le sujet porte
// client.publish retourne un booléen
  if (client.publish(porte_topic, String(etat).c_str())) {
    Serial.println("information sent!");
  }
//Réessayer de se publier en commençant par établir une nouvelle connection car il y a une probabilité que la connection précedente s'est interrompue
  else {
    Serial.println("information failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(porte_topic, String(etat).c_str());
  }

  
//Deconnection du gestionnaire de messages
  client.disconnect();
//entrer en mode sleep pour économiser l'utilisation de la batterie
  Serial.println("I'm awake, but I'm going into deep sleep mode for 10 seconds");
  ESP.deepSleep(10e6);
}