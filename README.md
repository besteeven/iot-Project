#iot-Project-  API RESTful IoT - Capteurs TTGO T-Display ESP32


## Auteurs

- BE Michel (Étudiant n°22015931)
- COUNDOUL FAMA (Étudiant n°22417970)
- SALEM Ahmedou (Étudiant n°22415878)

---

## Objectif du projet

L'objectif de ce projet est de développer une API RESTful complète sur un microcontrôleur ESP32 (TTGO T-Display). Cette API doit permettre de lister les capteurs connectés, de récupérer les valeurs des capteurs en temps réel, de contrôler l'allumage et l'extinction d'une LED, et de gérer des seuils automatiques permettant d'activer ou de désactiver la LED en fonction des valeurs captées. Le projet inclut également une interface utilisateur sur l'écran TFT intégré, le contrôle via des boutons physiques, et une communication WiFi robuste.

### Matériel utilisé

Le projet utilise les composants suivants :

- Microcontrôleur ESP32 (TTGO T-Display) écran TFT intégré
- Capteur de température (thermistor 10K)
- Capteur de lumière (photoresistance)
- LED contrôlable

référence du kit: https://www.adafruit.com/product/2975
---

## Fonctionnalités implémentées

Le projet implémente l'ensemble des fonctionnalités requises par l'énoncé. L'API RESTful est entièrement fonctionnelle avec des endpoints GET et POST permettant de récupérer et de contrôler les capteurs. Les deux capteurs (température et lumière) sont lus régulièrement et leurs valeurs sont disponibles via l'API. La LED peut être contrôlée de deux manières : via l'API ou automatiquement en fonction des seuils configurés.

Au-delà des exigences, le projet inclut plusieurs fonctionnalités complémentaires : l'affichage des données en temps réel sur l'écran TFT avec deux modes d'affichage (normal et scan WiFi), le scan des réseaux WiFi disponibles avec pagination (8 réseaux par page), le débounce des boutons physiques pour éviter les faux appuis, un système de mode automatique et manuel configurable, des tests unitaires validant les fonctionnalités principales (43 tests au total), une gestion d'erreurs robuste avec logs sériels détaillés, et la capacité à configurer et modifier les seuils à tout moment.

---

## Installation et configuration

### Prérequis

Pour compiler et téléverser le projet, vous aurez besoin de :

- Arduino  IDE
- Carte ESP32 version 3.3.1 ou supérieure (à installer via le Board Manager)
- Bibliothèques Arduino :
  - ArduinoJson version 6.x (par Benoit Blanchon)
  - TFT_eSPI (par Bodmer)
  - WiFi (incluse avec la carte ESP32)

### Configuration initiale

Avant de téléverser le code, vous devez configurer les paramètres spécifiques à votre installation dans le fichier 00_config.ino :

```cpp
#define WIFI_SSID "YOUR_SSID"           // Remplacez par le nom de votre WiFi
#define WIFI_PASSWORD "YOUR_PASSWORD"   // Remplacez par votre mot de passe WiFi
#define SERVER_PORT 80                   // Port de l'API (80 pour HTTP standard)

// Pins GPIO (adaptez selon votre montage)
#define LED_PIN 32                       // Broche de la LED
#define LIGHT_SENSOR_PIN 36             // Capteur de lumière
#define TEMP_SENSOR_PIN 39              // Capteur de température
#define BUTTON_1 0                      // Bouton 1 (Scan WiFi/Défilement)
#define BUTTON_2 35                     // Bouton 2 (Affichage normal)
```

Connectez l'ESP32 à l'ordinateur via un cable USB et compilez et téléversez le code.

### Première utilisation

Après le téléversement :

1. Ouvrez le Moniteur série depuis Arduino(Outils > Moniteur série) en réglant la vitesse à 115200 baud pour visualiser les logs et les résultats des tests unitaires.
2. L'adresse IP de l'ESP32 s'affichera dans le moniteur série (par exemple : 192.168.1.100)
3. Testez la connexion en visitant http://192.168.1.100/api/status dans votre navigateur

---

## API RESTful - Endpoints

L'API est disponible à l'adresse http://X.X.X.X/api où X.X.X.X est l'adresse IP de l'ESP32. 
Tous les endpoints retournent du JSON et supportent les opérations suivantes :


| Méthode | Endpoint | Description |
|---------|----------|-------------|
| GET | /api/status | État du système (uptime, état LED, nombre de capteurs) |
| GET | /api/sensors | Lister tous les capteurs avec leurs valeurs |
| GET | /api/sensor?id=X | Obtenir un capteur spécifique par son ID |
| GET | /api/led | Retourne l'état actuel de la LED |
| POST | /api/led | Contrôle la LED (allumage, extinction, basculement) |
| GET | /api/thresholds | Retourne la configuration de tous les seuils |
| POST | /api/thresholds | Configure ou modifie un seuil |
| GET | /api/mode | Retourne le mode actuel (auto/manual) |
| POST | /api/mode | Bascule entre mode automatique et manuel |

Pour consommer ces endpoints, l'appareil qui envoie la requête doit être connecté sur le même réseau wifi que l'ESP32
Il est possible d'accéder à l'API via un navigateur ou via `curl` dans le terminal.

### Exemples de requêtes
Pour vérifier la connexion à l'api:

```bash
curl http://192.168.1.100/api/status
```

Pour allumer la LED :

```bash
curl -X POST http://192.168.1.100/api/led \
  -H "Content-Type: application/json" \
  -d '{"action":"on"}'
```

Pour récupérer tous les capteurs :

```bash
curl http://192.168.1.100/api/sensors
```

Pour configurer un seuil de lumière :

```bash
curl -X POST http://192.168.1.100/api/thresholds \
  -H "Content-Type: application/json" \
  -d '{"sensorType":"light","threshold":2000,"enabled":true}'
```

Pour une documentation complète de chaque endpoint avec les structures JSON détaillées, consultez le fichier GUIDE_TEST_API.md.

---

## Utilisation des boutons physiques

Le TTGO T-Display dispose de deux boutons physiques permettant de contrôler l'affichage. Ils n'ont aucun effet sur le contrôle de la LED.

### Bouton de droite (GPIO 0) - Scan WiFi / Défilement
- **Premier appui** : Active le mode scan WiFi et affiche les réseaux disponibles
- **Appuis suivants** : Fait défiler la liste des réseaux 
- **Mise à jour automatique** : La liste se rafraîchit toutes les 10 secondes

### Bouton de gauche (GPIO 35) - Affichage normal
- **Appui** : Retourne à l'affichage normal (capteurs + état LED + connexion WiFi)

### Mode scan WiFi
Le scan WiFi fonctionne même si l'ESP32 n'est pas encore connecté à un réseau et fonctionne sans le moindre montage. 
L'affichage montre :
- Le nombre total de réseaux détectés
- Les noms des réseaux (SSID uniquement)
- Un indicateur de pagination (Page X/Y) si plus de 8 réseaux sont disponibles



---

### Tests des capteurs

Pour vérifier que les capteurs fonctionnent correctement :

Couvrez le capteur de lumière avec votre main. La valeur de lumière devrait diminuer et être affichée sur l'écran TFT.

Chauffez le capteur de température l'entourant avec 2 doigts. La température devrait augmenter progressivement.

### Tests des seuils

Configurez un seuil de lumière pour allumer la LED automatiquement :

```bash
curl -X POST http://192.168.1.100/api/thresholds \
  -H "Content-Type: application/json" \
  -d '{"sensorType":"light","threshold":2000,"enabled":true}'
```

Couvrez ensuite le capteur de lumière. La LED devrait s'allumer automatiquement.

---

## Structure du code

Le code est organisé de manière modulaire avec chaque composant dans un fichier séparé. Cette architecture permet une maintenance facile et facilite l'ajout de nouvelles fonctionnalités.
Les fichiers sont numérotées pour donner un ordre de compilation à Arduino (car Arduino compile dans l'ordre alphabétique).
Un fichier dummy au même nom que le dossier du projet est aussi présent pour éviter que Arduino réclame de créer un nouveau dossier.

### Fichiers principaux

00_config.ino : Contient toutes les configurations globales, les définitions de pins, les macros de debug et les structures de données utilisées par les autres modules.

01_sensor_manager.ino : Gère la lecture des capteurs via les entrées ADC, convertit les valeurs brutes en unités exploitables, et maintient l'état des capteurs.

02_led_controller.ino : Contrôle la LED, gère les seuils d'activation automatique, et permet de basculer entre les modes automatique et manuel.

03_wifi_manager.ino : Gère la connexion WiFi, la reconnexion automatique en cas de déconnexion, et récupère l'adresse IP du microcontrôleur.

04_api_server.ino : Implémente le serveur HTTP et tous les endpoints de l'API RESTful. Gère la sérialisation JSON et valide les requêtes entrantes.

05_unit_tests.ino : Contient les tests unitaires qui valident le fonctionnement de chaque composant. Ces tests peuvent être exécutés au démarrage du système les résultats peuvent être visualisés dans le moniteur série.

06_button_manager.ino : Gère la détection des appuis sur les boutons physiques, implémente le débounce, et permet le contrôle local de la LED.

07_main.ino : Contient la fonction setup() qui initialise tous les composants et la boucle loop() qui orchestre l'exécution du système. Gère également l'affichage sur l'écran TFT.

---

## Documentation supplémentaire

Pour plus d'informations sur les endpoints de l'API et comment les tester, consultez le fichier GUIDE_TEST_API.md qui contient des exemples détaillés et des cas de test complets.

Le moniteur série (115200 baud) fournit des logs détaillés du fonctionnement du système. Ces logs aident au débogage en cas de problème.

