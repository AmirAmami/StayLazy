## Etapes initiales:

### Insallation d'un système d'exploitation
Avant de commencer l’implémentation des codes, il faut tout d’abord installer un système d’exploitation sur le Raspberry, c’est une étape assez simple et très documenté, donc on ne va pas la détaillé sur ce manuel, on précise qu’il faut installer Raspbian comme système d’exploitation pour éviter d’éventuelles problèmes ultérieurement.

### Configuration du rapsberry comme point d'accès
Avant d’entamer cette partie, il faut tout d’abord configurer le Raspberry comme point d’accès pour les ateliers. Cette étape est très compliquée à faire, donc on va se servir d’un code open source qu’on va importer de [GitHub](https://github.com/km4ack/pi-scripts/blob/master/autohotspotN-setup) vers le Raspberry en utilisant l’instruction suivante sur le terminal du Raspberry : « wget https://raw.githubusercontent.com/km4ack/pi-scripts/master/autohotspotN-setup ». La vidéo dans ce [lien](https://www.youtube.com/watch?v=qMT-0mz1lkI) explique étape par étape comment configurer le Raspberry comme point d’accès.
Après la configuration, le Raspberry ce comportera comme un point d’accès et aura l’adresse IP fixe 10.10.10.10, mais il pourra aussi permuter vers un fonctionnement normale si jamais une connexion Wifi est requise.

### Installation et configuration du gestionnaire de message
Afin d'utiliser le Raspberry comme gestionnaire de message, il faut installer un gestionnaire de message MQTT, celui utilisé dans ce projet est Mosquitto.
Pour l’installer, il faut insérer la commande sudo apt-get install mosquito sur le terminal du Raspberry, ensuite il faut configurer ce gestionnaire de message, pour accéder au fichier de configuration, il faut utiliser l’instruction sudo nano /etc/mosquitto/mosquitto.conf, Ensuite on ajoute l’instruction allow_anonymous false au fichier pour qu’on ne permet pas à n’importe qui a se connecter au gestionnaire de message.
Maintenant nous allons ajouter le nom d’utilisateur des Publishers et Subscribers qui pourront communiquer avec le gestionnaire de message en utilisant l’instruction sudo mosquitto_passwd -c /etc/mosquitto/pwfile nomUtilisateur, ces utilisateurs sont très importants car ils seront utilisés dans les codes pour permettre aux utilisateurs de se connecter au Raspberry.
Enfin, il faut installer la bibliothèque paho_mqtt en inserant sudo pip3 install paho-mqtt au terminal afin qu’on puisse utiliser le Raspberry comme Subscriber.

### Installation de la base de données InfluxDB
L’installation de la base de données se fait en utilisant les instructions sudo apt install influxdb et sudo apt install influxdb-client, ensuite il faut activer la communication http dans le fichier de configuration de InfluxDB, on utilise l’instruction sudo nano /etc/influxdb/influxdb.conf pour accéder au fichier, ensuite on descend à la partie http et on enlève le # devant enabled = true.
Ensuite, il faut créer la base de données dans laquelle on va stocker nos données et il faut créer des utilisateurs, cette partie est faite manuellement et elle est bien documenté sur le site de [InfluxDB](https://docs.influxdata.com/influxdb/v2.1/get-started/).
Enfin, il Enfin, il faut installer la bibliothèque influxdb en inserant sudo pip3 install influxdb au terminal.
