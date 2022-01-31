## Preparation du Raspberry:

### Installation d'un système d'exploitation
Avant de commencer l’implémentation des codes, il faut tout d’abord installer un système d’exploitation sur le Raspberry, c’est une étape assez simple et très documentée, donc on  ne va pas la détailler dans ce manuel. On précise qu’il faut installer ***Raspbian*** comme système d’exploitation pour éviter d’éventuels problèmes ultérieurement.

### Configuration du rapsberry comme point d'accès
Avant d’entamer cette partie, il faut tout d’abord configurer le Raspberry comme point d’accès pour les ateliers. Cette étape est très compliquée à faire, donc on va se servir     d’un code open source qu’on va importer de [GitHub](https://github.com/km4ack/pi-scripts/blob/master/autohotspotN-setup) vers le Raspberry en utilisant l’instruction suivante     sur le terminal du Raspberry : « wget https://raw.githubusercontent.com/km4ack/pi-scripts/master/autohotspotN-setup ». La vidéo dans ce [lien](https://www.youtube.com/watch?       v=qMT-0mz1lkI) explique étape par étape comment configurer le Raspberry comme point d’accès.
Après la configuration, le Raspberry se comportera comme un point d’accès et aura l’adresse IP fixe 10.10.10.10, mais il pourra aussi permuter vers un fonctionnement normal si   jamais une connexion Wifi est requise.

### Installation et configuration du gestionnaire de message
Afin d'utiliser le Raspberry comme gestionnaire de message, il faut installer un gestionnaire de message MQTT. Celui utilisé dans ce projet est ***Mosquitto***.
Pour l’installer, il faut insérer la commande ***sudo apt-get install mosquito*** sur le terminal du Raspberry, ensuite il faut configurer ce gestionnaire de message. Pour accéder au fichier de configuration, il faut utiliser l’instruction ***sudo nano /etc/mosquitto/mosquitto.conf***. Ensuite on ajoute l’instruction ***allow_anonymous false*** au fichier pour qu’on ne permette pas à n’importe qui de se connecter au gestionnaire de message.
Maintenant nous allons ajouter le nom d’utilisateur des Publishers et Subscribers qui pourront communiquer avec le gestionnaire de message en utilisant l’instruction ***sudo mosquitto_passwd -c /etc/mosquitto/pwfile nomUtilisateur***. Ces utilisateurs sont très importants car ils seront utilisés dans les codes pour permettre aux utilisateurs de se connecter au Raspberry.
Enfin, il faut installer la bibliothèque paho_mqtt en inserant ***sudo pip3 install paho-mqtt*** au terminal afin qu’on puisse utiliser le Raspberry comme Subscriber.

### Installation de la base de données InfluxDB
L’installation de la base de données se fait en utilisant les instructions ***sudo apt install influxdb*** et ***sudo apt install influxdb-client***. Ensuite il faut activer la communication http dans le fichier de configuration de InfluxDB. On utilise l’instruction ***sudo nano /etc/influxdb/influxdb.conf*** pour accéder au fichier, puis on descend à la partie http et on enlève le # devant ***enabled = true***.
Ensuite, il faut créer la base de données dans laquelle on va stocker nos données et il faut créer des utilisateurs. Cette partie est faite manuellement et elle est bien documenté sur le site de [InfluxDB](https://docs.influxdata.com/influxdb/v2.1/get-started/).
Enfin, il faut installer la bibliothèque influxdb en insérant ***sudo pip3 install influxdb*** au terminal.

## Explication des roles et des relations entre les codes
En général, il y a 3 entités qui communiquent entre eux, il y a le smartphone, le Raspberry et l’ESP8266. Le Raspberry jouera le rôle du gestionnaire de message et en même temps il sera un Subscriberet un Publisher, L’ESP et le smartphone quant à eux seront en même temps des Publishers et Subscribers.
Le smartphone communiquera avec le broker en utilisant un Application, Le Raspberry sera codé en utilisant le langage Python et l’ESP en utilisant le langage C++.
Commençant tout d'abord par le Raspberry.

### Raspberry
Comme cité précédemment, le Raspberry jouera 3 rôles. Le premier rôle de gestionnaire de message a été configuré au début. Le deuxième rôle de Subscriber est assuré par le code [SubscriberMQTT.py](https://github.com/AmirAmami/StayLazy/blob/main/Deuxi%C3%A8me%20Seance/Raspberry/SubscriberMQTT.py). Ce code permet au Raspberry d’observer tous les topics qui commencent initialement par ***Chambre/***. Il reçoit une trame par exemple de la forme **Chambre/température/24**. Ce code permet de séparer la trame en trois partie, la première partie ‘Chambre’ représente la base donnes. La deuxième partie représente le tableau et enfin la troisième partie représente la donnée. Après avoir séparé la trame, le code se charge de stocker dans la base de données InfluxDB, la valeur d’humidité dans le tableau température dans la base de données Chambre. Ce qui est intéressant dans ce code est qu’il n’est pas nécessaire de créer la base de données et le tableau avant d’envoyer la trame. Si le code ne trouve pas le tableau ou la base de données, il va les créer lui-même.

Les codes [PublishServoMoteur.py](https://github.com/AmirAmami/StayLazy/blob/main/Deuxi%C3%A8me%20Seance/Raspberry/PublishServoMoteur.py) et [PublishMoteur.py](https://github.com/AmirAmami/StayLazy/blob/main/Deuxi%C3%A8me%20Seance/Raspberry/PublishMoteur.py) envoient des messages à l’esp8266. Dans notre situation, on traite le cas relié à la détection de luminosité extérieure. Si c’est la nuit, le volet doit se fermer donc on récupère de la base de données la période sachant que cette dernière est reçue à partir d’un message envoyé par l’esp8266. Ceci sera détaillé dans la section suivante. Après la récupération de la période, on envoie les valeurs 1 ou 0 pour actionner le moteur afin de fermer ou d’ouvrir les volets. La seule différence entre les deux codes est que l'un concerne un servomoteur alors que l’autre concerne un moteur pas à pas.

### ESP8266
Les ESPs utilisés joueront soit le rôle de Publisher ou de Subscriber. Les scripts qui jouent le rôle de Publisher envoient les données collectées des capteurs afin de les visualiser sur le smartphone ou afin de s’en servir comme dans le cas précèdent pour ouvrir ou fermer les volets. Les codes qui jouent le rôle de Publisher sont :
- [CapteurDHT11.cpp](https://github.com/AmirAmami/StayLazy/blob/main/Deuxi%C3%A8me%20Seance/Esp8266/CapteurDHT11.cpp) : Ce code renvoie sur les topics "Chambre/temperature" et "Chambre/humidite" les valeurs de la température et de l'humidité dans la chambre.
- [CapteurLumiere.cpp](https://github.com/AmirAmami/StayLazy/blob/main/Deuxi%C3%A8me%20Seance/Esp8266/CapteurLumiere.cpp) : Ce code renvoie sur le topic "Chambre/periode" si c'est la nuit ou le jour.
- [CapteurProximité.cpp](https://github.com/AmirAmami/StayLazy/blob/main/Deuxi%C3%A8me%20Seance/Esp8266/CapteurProximit%C3%A9.cpp) : Ce code renvoie sur le topic "Chambre/porte" l'état de la porte: ouverte ou fermée.

Les fichiers Subscriber sont : [Moteur.cpp](https://github.com/AmirAmami/StayLazy/blob/main/Deuxi%C3%A8me%20Seance/Esp8266/Moteur.cpp) et [Servomoteur.cpp](https://github.com/AmirAmami/StayLazy/blob/main/Deuxi%C3%A8me%20Seance/Esp8266/Servomoteur.cpp). Ces deux scripts comme cité précédemment récupére du raspberry soit la valeur 1 ou 0 selon la période afin de fermer ou d'ouvrir un volet en actionnant les moteurs.

## Remarques
- Tous les scripts sont expliqués en utilisant des commentaires.
- Les pins utilisés pour les capteurs ne sont pas spécifiés. Ce sont des valeurs aléatoire, donc c'est au formatteur de les specifier.
- Tous les scripts de l'ESP ont été séparés vu que l'atelier sera séparé en sous ateliers. Néanmoins tous les fichiers peuvent être regrouper dans un seul.
- Possibilité d'ajouter Grafana au Raspberry afin de visualiser les données des capteurs.
