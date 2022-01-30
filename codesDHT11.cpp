//Inclusion/Introduction des bilbliothèques à utiliser
#include "DHT.h"
#include "PubSubClient.h"

#include "ESP8266WiFi.h"  
//Definition du type de capteur (dht11 ou dht22) et du Pin utilisé
#define DHTPIN D5        

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Definition des paramètres du point d'accès
char ssid[] = "................";           // A determiner lors de la configuration du point d'accees
char wifi_password[] = ".............."; // A determiner lors de la configuration du point d'accees

//Definition des paramètres de la communication MQTT
char mqtt_server[] = "10.10.10.10";  // IP of the MQTT broker
char humidity_topic[] = "Chambre/humidity";
char temperature_topic[] = "Chambre/temperature";
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
  Serial.begin(9600);
  dht.begin();
}

void loop() {

//Connection au point d'accès et au gestionnaire de messages
  connect_MQTT();
  Serial.setTimeout(2000);
  
//Stockage et affichage des valeurs de l'humidité et température acquises par le capteur  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");

// Conversion des valeurs à des string puisque MQTT ne transmet que des strings
  String hs="Hum: "+String((float)h)+" % ";
  String ts="Temp: "+String((float)t)+" C ";

//Publier le message selon le sujet temperature
// client.publish retourne un booléen
  if (client.publish(temperature_topic, String(t).c_str())) {
    Serial.println("Temperature sent!");
  }
//Réessayer de se publier en commençant par établir une nouvelle connection car il y a une probabilité que la connection précedente s'est interrompue
  else {
    Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(temperature_topic, String(t).c_str());
  }
//Publier le message selon le sujet humidity
  if (client.publish(humidity_topic, String(h).c_str())) {
    Serial.println("Humidity sent!");
  }
//Réessayer de se publier en commençant par établir une nouvelle connection car il y a une probabilité que la connection précedente s'est interrompue
  else {
    Serial.println("Humidity failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(humidity_topic, String(h).c_str());
  }
//Deconnection du gestionnaire de messages
  client.disconnect();
//entrer en mode sleep pour économiser l'utilisation de la batterie
  Serial.println("I'm awake, but I'm going into deep sleep mode for 10 seconds");
  ESP.deepSleep(10e6);
}
