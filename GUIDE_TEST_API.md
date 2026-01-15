# Guide de test de l'API RESTful ESP32

Ce guide complète le README.md et vous permet de tester tous les endpoints de l'API de manière pratique.
Consultez le README.md pour l'installation, la configuration et l'architecture du projet.

## Prérequis

### Trouver l'IP de votre ESP32

**Méthode 1 - Écran TTGO** :
```
WiFi: OK
IP: 192.168.1.100  ← Votre IP
```

**Méthode 2 - Moniteur série** (115200 baud) :
```
[WiFiManager] IP Address: 192.168.1.100
```

---

## TESTS RAPIDES (Copier-Coller)

> **IMPORTANT** : Remplacez `192.168.1.100` par votre vraie IP !

### 1. Test connexion - Vérifier que l'API répond

```bash
curl http://192.168.1.100/api/status
```

**Résultat attendu** :
```json
{
  "uptime": 123456,
  "ledStatus": "off",
  "sensorCount": 2
}
```

---

### 2. Lire TOUS les capteurs (température + lumière)

```bash
curl http://192.168.1.100/api/sensors
```

**Résultat attendu** :
```json
{
  "sensors": [
    {
      "id": 0,
      "name": "Sensor_1",
      "type": "temperature",
      "value": 24.5,
      "unit": "°C",
      "minValue": -40,
      "maxValue": 80
    },
    {
      "id": 1,
      "name": "Sensor_2",
      "type": "light",
      "value": 1823,
      "unit": "lux",
      "minValue": 0,
      "maxValue": 4095
    }
  ]
}
```

---

### 3. Lire TEMPÉRATURE uniquement

```bash
curl http://192.168.1.100/api/sensor?id=0
```

**Résultat attendu** :
```json
{
  "id": 0,
  "name": "Sensor_1",
  "type": "temperature",
  "value": 24.5,
  "unit": "°C"
}
```

**Test** : Chauffez la thermistance avec vos doigts pour voir la valeur augmenter.

---

### 4. Lire LUMIÈRE uniquement

```bash
curl http://192.168.1.100/api/sensor?id=1
```

**Résultat attendu** :
```json
{
  "id": 1,
  "name": "Sensor_2",
  "type": "light",
  "value": 1823,
  "unit": "lux"
}
```

**Test** : Couvrez le capteur pour voir la valeur diminuer.

---

### 5. État de la LED

```bash
curl http://192.168.1.100/api/led
```

**Résultat si éteinte** :
```json
{
  "status": "off",
  "state": false
}
```

**Résultat si allumée** :
```json
{
  "status": "on",
  "state": true
}
```

---

### 6. Vérifier le mode actuel

```bash
curl http://192.168.1.100/api/mode
```

**Résultat** :
```json
{
  "mode": "auto"
}
```

**Note** : Le mode par défaut est "auto" (seuils automatiques activés).

---

### 7. Passer en mode manuel

```bash
curl -X POST http://192.168.1.100/api/mode \
  -H "Content-Type: application/json" \
  -d '{"mode":"manual"}'
```

**Résultat** :
```json
{
  "mode": "manual"
}
```

---

### 8. Allumer la LED

```bash
curl -X POST http://192.168.1.100/api/led \
  -H "Content-Type: application/json" \
  -d '{"action":"on"}'
```

**Résultat** :
```json
{
  "status": "on"
}
```

**Vérification** : La LED verte physique doit s'allumer.

---

### 9. Éteindre la LED

```bash
curl -X POST http://192.168.1.100/api/led \
  -H "Content-Type: application/json" \
  -d '{"action":"off"}'
```

**Résultat** :
```json
{
  "status": "off"
}
```

**Vérification** : La LED verte s'éteint.

---

### 10. Toggle LED (inverser l'état)

```bash
curl -X POST http://192.168.1.100/api/led \
  -H "Content-Type: application/json" \
  -d '{"action":"toggle"}'
```

**Note** : Lancez cette commande plusieurs fois pour faire clignoter la LED.

---

### 11. Repasser en mode automatique

```bash
curl -X POST http://192.168.1.100/api/mode \
  -H "Content-Type: application/json" \
  -d '{"mode":"auto"}'
```

**Résultat** :
```json
{
  "mode": "auto"
}
```

---

### 12. Voir les seuils configurés

```bash
curl http://192.168.1.100/api/thresholds
```

**Résultat attendu** :
```json
{
  "thresholds": [
    {
      "sensorType": "temperature",
      "threshold": 25.0,
      "enabled": false
    },
    {
      "sensorType": "light",
      "threshold": 2500.0,
      "enabled": true
    }
  ]
}
```

**Note** : `enabled: true` pour lumière = Seuil automatique activé

---

### 13. Modifier le seuil de lumière

```bash
curl -X POST http://192.168.1.100/api/thresholds \
  -H "Content-Type: application/json" \
  -d '{"sensorType":"light","threshold":3000,"enabled":true}'
```

**Résultat** :
```json
{
  "sensorType": "light",
  "threshold": 3000.0,
  "enabled": true
}
```

**Effet** : LED s'allume maintenant si lumière < 3000

---

### 14. Activer seuil de température

```bash
curl -X POST http://192.168.1.100/api/thresholds \
  -H "Content-Type: application/json" \
  -d '{"sensorType":"temperature","threshold":28,"enabled":true}'
```

**Effet** : LED s'allume si température > 28°C

**Test** : Chauffez la thermistance pour voir la LED s'allumer.

---

### 15. Désactiver un seuil

```bash
curl -X POST http://192.168.1.100/api/thresholds \
  -H "Content-Type: application/json" \
  -d '{"sensorType":"light","enabled":false}'
```

**Effet** : LED ne réagit plus automatiquement à la lumière

---

### 16. Informations du device (endpoint Flutter)

```bash
curl http://192.168.1.100/api/device/info
```

**Résultat** :
```json
{
  "deviceId": "ESP32-TTGO",
  "firmware": "1.0.0",
  "uptime": 123456,
  "location": "Salle IoT"
}
```

**Note** : Endpoint prévu pour l'intégration avec une application Flutter.

---

### 17. Historique des mesures (endpoint Flutter)

```bash
curl http://192.168.1.100/api/history
```

**Résultat** :
```json
{
  "history": [
    {
      "timestamp": 1234567890,
      "temperature": 24.5,
      "light": 1823
    }
  ]
}
```

**Note** : Endpoint basique pour récupérer l'historique. Prévu pour statistiques dans app Flutter.

---

## Tests en boucle (temps réel)

### Surveiller capteurs toutes les 2 secondes

**Linux/Mac** :
```bash
watch -n 2 'curl -s http://192.168.1.100/api/sensors'
```

**Windows PowerShell** :
```powershell
while($true) {
  curl http://192.168.1.100/api/sensors
  Start-Sleep -Seconds 2
}
```

---

### Faire clignoter la LED

**Linux/Mac** :
```bash
while true; do
  curl -X POST http://192.168.1.100/api/led \
    -H "Content-Type: application/json" \
    -d '{"action":"toggle"}'
  sleep 1
done
```

**Windows PowerShell** :
```powershell
while($true) {
  curl -Method POST http://192.168.1.100/api/led `
    -ContentType "application/json" `
    -Body '{"action":"toggle"}'
  Start-Sleep -Seconds 1
}
```

---

## Test depuis navigateur Web

Ouvrez votre navigateur et collez ces URLs :

### 1. Status système
```
http://192.168.1.100/api/status
```

### 2. Tous les capteurs
```
http://192.168.1.100/api/sensors
```

### 3. Température seule
```
http://192.168.1.100/api/sensor?id=0
```

### 4. Lumière seule
```
http://192.168.1.100/api/sensor?id=1
```

### 5. État LED
```
http://192.168.1.100/api/led
```

### 6. Seuils
```
http://192.168.1.100/api/thresholds
```

### 7. Mode actuel
```
http://192.168.1.100/api/mode
```

### 8. Informations device
```
http://192.168.1.100/api/device/info
```

### 9. Historique
```
http://192.168.1.100/api/history
```

---

## Test avec Postman (Alternative graphique)

### Importer dans Postman :

**Collection JSON** :
```json
{
  "info": {
    "name": "ESP32 TTGO API",
    "schema": "https://schema.getpostman.com/json/collection/v2.1.0/collection.json"
  },
  "item": [
    {
      "name": "Get All Sensors",
      "request": {
        "method": "GET",
        "url": "http://192.168.1.100/api/sensors"
      }
    },
    {
      "name": "Get Temperature",
      "request": {
        "method": "GET",
        "url": "http://192.168.1.100/api/sensor?id=0"
      }
    },
    {
      "name": "Get Light",
      "request": {
        "method": "GET",
        "url": "http://192.168.1.100/api/sensor?id=1"
      }
    },
    {
      "name": "LED ON",
      "request": {
        "method": "POST",
        "url": "http://192.168.1.100/api/led",
        "header": [{"key": "Content-Type", "value": "application/json"}],
        "body": {
          "mode": "raw",
          "raw": "{\"action\":\"on\"}"
        }
      }
    }
  ]
}
```

---

## Dépannage

### Erreur : `curl: (7) Failed to connect`

**Cause** : Mauvaise IP ou ESP32 non connecté

**Solution** :
```bash
# Vérifier que l'ESP32 répond
ping 192.168.1.100

# Vérifier l'IP sur l'écran TTGO ou moniteur série
```

---

### Erreur : `404 Not Found`

**Cause** : URL incorrecte

**Vérification** :
```bash
# Bon
http://192.168.1.100/api/sensors

# Mauvais
http://192.168.1.100/sensors  # manque /api
```

---

### Erreur : `400 Bad Request` ou `Invalid JSON`

**Cause** : JSON malformé

**Vérification** :
```bash
# Bon
curl -X POST http://192.168.1.100/api/led \
  -H "Content-Type: application/json" \
  -d '{"action":"on"}'

# Mauvais (guillemets manquants)
curl -X POST http://192.168.1.100/api/led \
  -H "Content-Type: application/json" \
  -d '{action:on}'
```

---

### Les valeurs ne changent pas

**Cause** : Capteurs pas connectés correctement

**Test** :
```bash
# Regardez les valeurs dans le moniteur série
[SensorManager] Temp: 24.5°C | Light: 1823

# Si valeurs fixes à 0 ou 4095 → Problème connexion
```

---

## Valeurs normales attendues

| Capteur | Condition | Valeur | Note |
|---------|-----------|--------|------|
| **Température** | Ambiante | 20-25°C | Normal |
| | Chauffée (doigt) | 28-35°C | Test OK |
| | Hors plage | < -40 ou > 80 | Erreur |
| **Lumière** | Obscurité | 0-300 | Très sombre |
| | Pénombre | 300-1000 | Sombre |
| | Normal | 1000-2500 | Éclairage normal |
| | Lumineux | 2500-4095 | Très lumineux |

---

## Checklist de validation

### Tests de base
- [ ] `GET /api/status` → Retourne uptime
- [ ] `GET /api/sensors` → Retourne 2 capteurs
- [ ] `GET /api/sensor?id=0` → Température raisonnable
- [ ] `GET /api/sensor?id=1` → Lumière raisonnable
- [ ] `GET /api/led` → État LED
- [ ] `GET /api/mode` → Mode actuel (auto/manual)
- [ ] `GET /api/device/info` → Info device
- [ ] `GET /api/history` → Historique mesures

### Tests mode
- [ ] `POST /api/mode {"mode":"manual"}` → Passe en mode manuel
- [ ] `POST /api/mode {"mode":"auto"}` → Passe en mode auto

### Tests LED
- [ ] `POST /api/led {"action":"on"}` → LED s'allume physiquement
- [ ] `POST /api/led {"action":"off"}` → LED s'éteint
- [ ] `POST /api/led {"action":"toggle"}` → LED change d'état

### Tests capteurs
- [ ] Couvrir capteur lumière → Valeur diminue
- [ ] Chauffer thermistance → Température augmente

### Tests seuils
- [ ] `GET /api/thresholds` → Seuil lumière activé (2500)
- [ ] Couvrir capteur → LED s'allume automatiquement
- [ ] Découvrir capteur → LED s'éteint

---

## Récapitulatif des endpoints

| Endpoint | Méthode | Description |
|----------|---------|-------------|
| /api/status | GET | État du système |
| /api/sensors | GET | Tous les capteurs |
| /api/sensor?id=0 | GET | Température |
| /api/sensor?id=1 | GET | Lumière |
| /api/led | GET | État LED |
| /api/mode | GET | Mode actuel |
| /api/mode | POST | Changer mode |
| /api/led | POST | Contrôler LED |
| /api/thresholds | GET | Lire seuils |
| /api/thresholds | POST | Modifier seuils |
| /api/device/info | GET | Info device |
| /api/history | GET | Historique |

**Total : 9 endpoints distincts**


