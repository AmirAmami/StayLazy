"""Inclusion/Introduction des bilbliothèques à utiliser"""
from influxdb import InfluxDBClient
import paho.mqtt.client as mqtt
from time import sleep



MQTT_ADDRESS = '10.10.10.10'
MQTT_USER = '........   ' # a determiner durant la configuration des clients MQTT
MQTT_PASSWORD = '........' # a determiner durant la configuration des clients MQTT
MQTT_TOPIC = 'Chambre/volet'
MQTT_CLIENT_ID = 'MQTTInfluxDBBridge2'
loop_value=1
"""utilisation d'une bouce while et de try except else pour assurer l'envoie des donnés au site web"""

while loop_value==1:       
        """fonction pour obtenir le dernier état du vehicule"""
        def get_periode(periode):
                result=client.query(('select %s(periode) from Decision where time >= now() - 60m')%(periode))
                points = result.get_points()
                for item in points:
                        return item[periode]
                         
        """établir une connection avec la base de donnée"""
        client = InfluxDBClient('localhost', 8086, '.........', '..........', 'Chambre')  #a remplir avec client et mot de passe MQTT determinée avant
        client.get_list_database()
        client.switch_database('Chambre')
        
        """Obtention du parametre souhaitee de la base de donnees"""
        periode=get_periode('last')
        
         # creer un client MQTT
        mqtt = mqtt.Client(MQTT_CLIENT_ID)

        #Determiner utilisateur et mot de passe avant authentification au gestionnaire de message
        mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)

        # connecter le client au gestionnaire de message
        mqtt.connect(MQTT_ADDRESS, 1883)

        #si c'est la nuit envoyer la valeur 1 à l'esp si c'est le jour envoyer la valeur 0 a l'esp
        if periode =='nuit':
            mqtt.publish(MQTT_TOPIC, "1")
        elif periode =='jour':
            mqtt.publish(MQTT_TOPIC, "0")

 # establish a two-second timeout
sleep (100)