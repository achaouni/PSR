Mini Serveur Web en C
--
Ce projet consiste à développer un mini serveur web en langage C avec plusieurs fonctionnalités clés. Le serveur est capable de gérer les connexions des clients, d'analyser les requêtes HTTP, de fournir des réponses appropriées et d'interagir avec un autre exécutable appelé "Ablette" pour collecter et analyser les paquets réseau.

## Fonctionnalités.

### Le mini serveur web en C comporte les fonctionnalités suivantes :

**1) Lancement de l'exécutable "Sioux"** : Le serveur lance un exécutable nommé **"Sioux"** en spécifiant le port associé au serveur web en tant que paramètre.

**2) Gestion des connexions des clients** : Le serveur gère les connexions entrantes des clients. À chaque nouvelle connexion, un thread distinct est créé pour analyser les requêtes **HTTP** du client et répondre de manière appropriée.

**3) Exécutable "Ablette"** : Le projet inclut un autre exécutable appelé **"Ablette"** qui est chargé de scanner les paquets réseau reçus sur la machine locale. Ablette filtre les paquets à destination de la machine locale et vers les ports **80** ou **443**, mais vous pouvez également personnaliser les ports avec une option **-p**. Il collecte des statistiques sur ces paquets, en mettant l'accent sur les cinq sources qui envoient le plus de paquets vers les ports ciblés.

**4) Mémoire partagée IPC** : Pour partager les statistiques collectées par **"Ablette"** avec le serveur **"Sioux"**, une mémoire partagée IPC est utilisée. Les statistiques sont envoyées au serveur sous forme de mémoire partagée pour un accès efficace et en temps réel (Plus d'informations ci-dessous).

## Utilisation.

### Compilation :

Vous pouvez compiler le projet en utilisant le fichier **Makefile** fourni. Pour compiler **sioux** et **Ablette** vous utilisez la commande :
```
$ make
```
### Exécution :

Pour démarrer le serveur, utilisez la commande suivante : 
```
$ ./sioux [-p,--port] <port>
```
Exemple : 
```
$ ./sioux --port 8000
```

Assurez-vous de spécifier les ports sur lequel le serveur doit écouter et -i pour spécifier l'interface réseau :

Pour exécuter Ablette, utilisez la commande suivante : 
```
$ ./Ablette -p <port1,port2...> -i <interface>
```
Exemple : 
```
$ sudo ./Ablette -p 443,80 -i bridge
```
L'utilisation de **sudo** est parfois obligatoire pour avoir l'accés à l'interface réseau passée en paramètre. 

## Configuration :

Pour personnaliser davantage le serveur web et Ablette, veuillez consulter les fichiers de code source et les commentaires pertinents.

### Fonctionnement de la mémoire partagée :

Lorsque **Ablette** est exécutée, elle commence à capturer les paquets réseau reçus, puis applique un filtre pour ne garder que les top 10 des sources envoyant le plus de paquets sur les ports ciblés. Ces statistiques sont ensuite stockées dans une mémoire partagée en se basant sur la bibliothèque **Partage_memoire.h**.

Lorsque **sioux** est exécutée (après Ablette), elle initialise le serveur sur le port ciblé tout en lisant les statistiques qu'Ablette avait analysé, depuis la mémoire partagée en se basant de même sur la bibliothèque **Partage_memoire.h**. Ces statistiques sont ensuite restockées dans **Ablette.html** pour garder leur traçabilité.

## Auteurs :

Ce projet a été développé par **EL HASNAOUI Bilal** et **CHAOUNI Ayoub** dans le cadre du module Programmation des Systèmes d'exploitations avec **Pr. REDON Xavier** et **Pr. VANTROYS Thomas**.