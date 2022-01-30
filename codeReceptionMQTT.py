 """Inclusion/Introduction des bilbliothèques à utiliser"""
import re
from typing import NamedTuple
from time import sleep
import paho.mqtt.client as mqtt
from influxdb import InfluxDBClient
from pygame import mixer

"""Definition des paramètres de la communication MQTT et execution d'un sleep pour éviter une interference entre ce code et un autre"""
sleep (120)

INFLUXDB_ADDRESS = '10.10.10.10'
INFLUXDB_USER = 'mqtt'      # a determiner durant la configuration des utilisateurs de la base de données
INFLUXDB_PASSWORD = 'mqtt'   # a determiner durant la configuration des utilisateurs de la base de données
INFLUXDB_DATABASE = 'Chambre'

MQTT_ADDRESS = '10.10.10.10'
MQTT_USER = '........   ' # a determiner durant la configuration des clients MQTT
MQTT_PASSWORD = '........' # a determiner durant la configuration des clients MQTT
MQTT_TOPIC = 'Chambre/+'
MQTT_REGEX = 'Chambre/([^/]+)'
MQTT_CLIENT_ID = 'MQTTInfluxDBBridge'

"""creation d'un objet influxdb_client qui permet d'accéder à la base de données influxdb"""
influxdb_client = InfluxDBClient(INFLUXDB_ADDRESS, 8086, INFLUXDB_USER, INFLUXDB_PASSWORD, None)

"""class qui permet de detrminer le type des variables qui seront stocké dans Influxdb"""
class SensorData(NamedTuple):
    measurement: str
    value: float

"""fonction qui affiche la reponse du gestionnaire de message à l'essai de connexion puis abonne le client au sujet souhaité"""
def on_connect(client, userdata, flags, rc):
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)

"""fonction determine les parties du message et retourne les variables comme namedtuple"""
def _parse_mqtt_message(topic, payload):
    match = re.match(MQTT_REGEX, topic)
    if match:
        measurement = match.group(1)
        if measurement == 'status':
            return None
        return SensorData(measurement, float(payload))
    else:
        return None
"""fonction qui crée à partir de la variable named tuple une structure json qui sera ensuite envoyé afin d'être stocker dans la BD"""
def _send_sensor_data_to_influxdb(sensor_data):
    json_body = [
        {
            'measurement': sensor_data.measurement,
            'fields': {
                'value': sensor_data.value
            }
        }
    ]
    influxdb_client.write_points(json_body)
    
"""fonction qui reçoit le message, et affiche son contenu, ensuite appelle la fonction _send_sensor_data_to_influxdb 
pour stocker les données dans la base de données"""
def on_message(client, userdata, msg):
    print(msg.topic + ' ' + str(msg.payload))
    sensor_data = _parse_mqtt_message(msg.topic, msg.payload.decode('utf-8'))
    if sensor_data is not None:
        _send_sensor_data_to_influxdb(sensor_data)

"""fonction qui choisi la base de donnée ou l'a crée dans le cas ou cette base de donnée est introuvable"""
def _init_influxdb_database():
    databases = influxdb_client.get_list_database()
    if len(list(filter(lambda x: x['name'] == INFLUXDB_DATABASE, databases))) == 0:
        influxdb_client.create_database(INFLUXDB_DATABASE)
    influxdb_client.switch_database(INFLUXDB_DATABASE)

def main():
    _init_influxdb_database()
    """crée un nouveau client"""
    mqtt_client = mqtt.Client(MQTT_CLIENT_ID)
    
    """Determiner utilisateur et mot de passe avant authentification au gestionnaire de message"""
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    
    """associer les fonctions au Callbacks"""
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    
    """connecter le client au gestionnaire de message"""
    mqtt_client.connect(MQTT_ADDRESS, 1883)
    
    """entrz dans un état de surveillance des callback (voir rapport)"""
    mqtt_client.loop_forever()

if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()

