/*
  ============================================================================
  API SERVER - VERSION AMÉLIORÉE POUR FLUTTER
  
  AJOUTS :
  - Support CORS complet (Flutter Web compatible)
  - Timestamps sur toutes les données
  - Endpoint /api/device/info pour localisation
  - Endpoint /api/history pour statistiques (basique)
  - Gestion OPTIONS pour preflight requests
  ============================================================================
*/

class APIServer {
private:
  WebServer server;
  SensorManager* sensorManager;
  LEDController* ledController;

public:
  APIServer(int port = SERVER_PORT)
    : server(port), sensorManager(nullptr), ledController(nullptr) {}

  void begin(SensorManager* sensors, LEDController* led) {
    sensorManager = sensors;
    ledController = led;

    // Endpoints existants
    server.on("/api/sensors", HTTP_GET, [this]() {
      handleSensorsGet();
    });
    server.on("/api/sensor", HTTP_GET, [this]() {
      handleSensorGet();
    });
    server.on("/api/led", HTTP_GET, [this]() {
      handleLEDGet();
    });
    server.on("/api/led", HTTP_POST, [this]() {
      handleLEDPost();
    });
    server.on("/api/thresholds", HTTP_GET, [this]() {
      handleThresholdGet();
    });
    server.on("/api/thresholds", HTTP_POST, [this]() {
      handleThresholdPost();
    });
    server.on("/api/status", HTTP_GET, [this]() {
      handleStatusGet();
    });

    // NOUVEAUX endpoints pour Flutter
    server.on("/api/device/info", HTTP_GET, [this]() {
      handleDeviceInfo();
    });
    server.on("/api/history", HTTP_GET, [this]() {
      handleHistory();
    });

    // Handlers OPTIONS pour CORS preflight
    server.on("/api/sensors", HTTP_OPTIONS, [this]() {
      handleOptions();
    });
    server.on("/api/sensor", HTTP_OPTIONS, [this]() {
      handleOptions();
    });
    server.on("/api/led", HTTP_OPTIONS, [this]() {
      handleOptions();
    });
    server.on("/api/thresholds", HTTP_OPTIONS, [this]() {
      handleOptions();
    });
    server.on("/api/status", HTTP_OPTIONS, [this]() {
      handleOptions();
    });
    server.on("/api/device/info", HTTP_OPTIONS, [this]() {
      handleOptions();
    });
    server.on("/api/history", HTTP_OPTIONS, [this]() {
      handleOptions();
    });
    server.on("/api/mode", HTTP_GET, [this]() {
      handleModeGet();
    });
    server.on("/api/mode", HTTP_POST, [this]() {
      handleModePost();
    });


    server.onNotFound([this]() {
      handleNotFound();
    });

    server.begin();
    DEBUG_PRINTLN("[APIServer] Server started on port " + String(SERVER_PORT));
    DEBUG_PRINTLN("[APIServer] CORS enabled for Flutter compatibility");
  }

  void handleClient() {
    server.handleClient();
  }

private:
  // ========================================================================
  // HELPER : Headers CORS (pour Flutter Web)
  // ========================================================================
  void setCorsHeaders() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    server.sendHeader("Access-Control-Max-Age", "86400");
  }

  // ========================================================================
  // OPTIONS handler (preflight CORS)
  // ========================================================================
  void handleOptions() {
    setCorsHeaders();
    server.send(204);  // No Content
  }

  // ========================================================================
  // GET /api/sensors - Liste capteurs (AVEC TIMESTAMPS)
  // ========================================================================
  void handleSensorsGet() {
    setCorsHeaders();

    DynamicJsonDocument doc(1024);
    JsonArray sensorsArray = doc.createNestedArray("sensors");

    unsigned long currentTime = millis();

    for (int i = 0; i < sensorManager->getSensorCount(); i++) {
      SensorData sensor = sensorManager->getSensor(i);
      JsonObject sensorObj = sensorsArray.createNestedObject();
      sensorObj["id"] = i;
      sensorObj["name"] = sensor.name;
      sensorObj["type"] = sensor.type;
      sensorObj["value"] = sensor.value;
      sensorObj["unit"] = (sensor.type == "temperature") ? "°C" : "lux";
      sensorObj["minValue"] = sensor.minValue;
      sensorObj["maxValue"] = sensor.maxValue;
      sensorObj["timestamp"] = currentTime;     // ← NOUVEAU pour Flutter
      sensorObj["lastRead"] = sensor.lastRead;  // ← NOUVEAU pour Flutter
    }

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  }

  // ========================================================================
  // GET /api/sensor?id=X - Capteur spécifique (AVEC TIMESTAMP)
  // ========================================================================
  void handleSensorGet() {
    setCorsHeaders();

    if (!server.hasArg("id")) {
      DynamicJsonDocument doc(256);
      doc["error"] = "Missing parameter: id";
      String response;
      serializeJson(doc, response);
      server.send(400, "application/json", response);
      return;
    }

    int sensorId = server.arg("id").toInt();
    if (sensorId < 0 || sensorId >= sensorManager->getSensorCount()) {
      DynamicJsonDocument doc(256);
      doc["error"] = "Invalid sensor ID";
      String response;
      serializeJson(doc, response);
      server.send(404, "application/json", response);
      return;
    }

    SensorData sensor = sensorManager->getSensor(sensorId);
    DynamicJsonDocument doc(512);
    doc["id"] = sensorId;
    doc["name"] = sensor.name;
    doc["type"] = sensor.type;
    doc["value"] = sensor.value;
    doc["unit"] = (sensor.type == "temperature") ? "°C" : "lux";
    doc["timestamp"] = millis();        // ← NOUVEAU pour Flutter
    doc["lastRead"] = sensor.lastRead;  // ← NOUVEAU pour Flutter
    doc["minValue"] = sensor.minValue;
    doc["maxValue"] = sensor.maxValue;

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  }

  // ========================================================================
  // GET /api/led
  // ========================================================================
  void handleLEDGet() {
    setCorsHeaders();

    DynamicJsonDocument doc(256);
    doc["status"] = ledController->getState() ? "on" : "off";
    doc["state"] = ledController->getState();
    doc["timestamp"] = millis();
    doc["mode"] = ledController->getMode();


    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  }

  // ========================================================================
  // POST /api/led
  // ========================================================================
  void handleLEDPost() {
    setCorsHeaders();

    if (ledController->isAutoMode()) {
      DynamicJsonDocument respDoc(256);
      respDoc["error"] = "LED is in AUTO mode. Switch to MANUAL to control it.";
      respDoc["hint"] = "POST /api/mode {\"mode\":\"manual\"}";
      String response;
      serializeJson(respDoc, response);
      server.send(409, "application/json", response);
      return;
    }


    if (!server.hasArg("plain")) {
      DynamicJsonDocument doc(256);
      doc["error"] = "Body is empty";
      String response;
      serializeJson(doc, response);
      server.send(400, "application/json", response);
      return;
    }

    String body = server.arg("plain");
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      DynamicJsonDocument respDoc(256);
      respDoc["error"] = "Invalid JSON";
      String response;
      serializeJson(respDoc, response);
      server.send(400, "application/json", response);
      return;
    }

    if (!doc.containsKey("action")) {
      DynamicJsonDocument respDoc(256);
      respDoc["error"] = "Missing 'action' field";
      String response;
      serializeJson(respDoc, response);
      server.send(400, "application/json", response);
      return;
    }

    String action = doc["action"];
    if (action == "on") ledController->turnOn();
    else if (action == "off") ledController->turnOff();
    else if (action == "toggle") ledController->toggle();
    else {
      DynamicJsonDocument respDoc(256);
      respDoc["error"] = "Invalid action";
      String response;
      serializeJson(respDoc, response);
      server.send(400, "application/json", response);
      return;
    }

    DynamicJsonDocument respDoc(256);
    respDoc["status"] = ledController->getState() ? "on" : "off";
    respDoc["state"] = ledController->getState();
    respDoc["timestamp"] = millis();  // ← NOUVEAU
    String response;
    serializeJson(respDoc, response);
    server.send(200, "application/json", response);
  }

  void handleModeGet() {
    DynamicJsonDocument doc(128);
    doc["mode"] = ledController->getMode();

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  }

  void handleModePost() {
    if (!server.hasArg("plain")) {
      DynamicJsonDocument doc(128);
      doc["error"] = "Body is empty";
      String response;
      serializeJson(doc, response);
      server.send(400, "application/json", response);
      return;
    }

    String body = server.arg("plain");
    DynamicJsonDocument doc(128);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      DynamicJsonDocument respDoc(128);
      respDoc["error"] = "Invalid JSON";
      String response;
      serializeJson(respDoc, response);
      server.send(400, "application/json", response);
      return;
    }

    if (!doc.containsKey("mode")) {
      DynamicJsonDocument respDoc(128);
      respDoc["error"] = "Missing 'mode' field";
      String response;
      serializeJson(respDoc, response);
      server.send(400, "application/json", response);
      return;
    }

    String mode = doc["mode"];
    mode.toLowerCase();
    if (mode != "auto" && mode != "manual") {
      DynamicJsonDocument respDoc(128);
      respDoc["error"] = "Invalid mode (use 'auto' or 'manual')";
      String response;
      serializeJson(respDoc, response);
      server.send(400, "application/json", response);
      return;
    }

    ledController->setMode(mode);

    // Optionnel: si on repasse en AUTO, appliquer tout de suite (au lieu d'attendre 500ms)
    if (mode == "auto" && sensorManager != nullptr) {
      ledController->checkAndApplyThreshold(sensorManager->getTemperature(), "temperature");
      ledController->checkAndApplyThreshold(sensorManager->getLightLevel(), "light");
    }

    DynamicJsonDocument respDoc(128);
    respDoc["mode"] = ledController->getMode();
    String response;
    serializeJson(respDoc, response);
    server.send(200, "application/json", response);
  }


  // ========================================================================
  // GET /api/thresholds
  // ========================================================================
  void handleThresholdGet() {
    setCorsHeaders();

    DynamicJsonDocument doc(512);
    JsonArray thresholdsArray = doc.createNestedArray("thresholds");

    ThresholdConfig tempThreshold = ledController->getThresholdConfig("temperature");
    JsonObject tempObj = thresholdsArray.createNestedObject();
    tempObj["sensorType"] = "temperature";
    tempObj["threshold"] = tempThreshold.threshold;
    tempObj["enabled"] = tempThreshold.enabled;

    ThresholdConfig lightThreshold = ledController->getThresholdConfig("light");
    JsonObject lightObj = thresholdsArray.createNestedObject();
    lightObj["sensorType"] = "light";
    lightObj["threshold"] = lightThreshold.threshold;
    lightObj["enabled"] = lightThreshold.enabled;

    doc["timestamp"] = millis();  // ← NOUVEAU

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  }

  // ========================================================================
  // POST /api/thresholds
  // ========================================================================
  void handleThresholdPost() {
    setCorsHeaders();

    if (!server.hasArg("plain")) {
      DynamicJsonDocument doc(256);
      doc["error"] = "Body is empty";
      String response;
      serializeJson(doc, response);
      server.send(400, "application/json", response);
      return;
    }

    String body = server.arg("plain");
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      DynamicJsonDocument respDoc(256);
      respDoc["error"] = "Invalid JSON";
      String response;
      serializeJson(respDoc, response);
      server.send(400, "application/json", response);
      return;
    }

    if (!doc.containsKey("sensorType")) {
      DynamicJsonDocument respDoc(256);
      respDoc["error"] = "Missing 'sensorType' field";
      String response;
      serializeJson(respDoc, response);
      server.send(400, "application/json", response);
      return;
    }

    String sensorType = doc["sensorType"];
    if (doc.containsKey("threshold")) {
      ledController->setThreshold(sensorType, doc["threshold"]);
    }
    if (doc.containsKey("enabled")) {
      bool enabled = doc["enabled"];
      if (enabled) ledController->enableThreshold(sensorType);
      else ledController->disableThreshold(sensorType);
    }

    ThresholdConfig config = ledController->getThresholdConfig(sensorType);
    DynamicJsonDocument respDoc(256);
    respDoc["sensorType"] = config.sensorType;
    respDoc["threshold"] = config.threshold;
    respDoc["enabled"] = config.enabled;
    respDoc["timestamp"] = millis();  // ← NOUVEAU
    String response;
    serializeJson(respDoc, response);
    server.send(200, "application/json", response);
  }

  // ========================================================================
  // GET /api/status
  // ========================================================================
  void handleStatusGet() {
    setCorsHeaders();

    DynamicJsonDocument doc(512);
    doc["uptime"] = millis();
    doc["ledStatus"] = ledController->getState() ? "on" : "off";
    doc["sensorCount"] = sensorManager->getSensorCount();
    doc["freeHeap"] = ESP.getFreeHeap();  // ← NOUVEAU (mémoire disponible)
    doc["timestamp"] = millis();

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  }

  // ========================================================================
  // NOUVEAU : GET /api/device/info (pour Flutter - localisation)
  // ========================================================================
  void handleDeviceInfo() {
    setCorsHeaders();

    DynamicJsonDocument doc(1024);

    // Informations device
    doc["deviceId"] = "ESP32_TTGO_001";  // ← Personnalisable
    doc["deviceName"] = "TTGO T-Display";
    doc["location"] = "Laboratoire IoT - Salle TP3";  // ← À configurer

    // Informations réseau
    doc["ip"] = WiFi.localIP().toString();
    doc["mac"] = WiFi.macAddress();
    doc["rssi"] = WiFi.RSSI();
    doc["ssid"] = WiFi.SSID();

    // Informations système
    doc["uptime"] = millis();
    doc["freeHeap"] = ESP.getFreeHeap();
    doc["firmwareVersion"] = "1.1.0";  // ← Version avec CORS + Flutter
    doc["chipModel"] = ESP.getChipModel();
    doc["cpuFreq"] = ESP.getCpuFreqMHz();

    // Capteurs disponibles
    JsonArray sensors = doc.createNestedArray("sensors");
    for (int i = 0; i < sensorManager->getSensorCount(); i++) {
      SensorData sensor = sensorManager->getSensor(i);
      JsonObject sensorObj = sensors.createNestedObject();
      sensorObj["id"] = i;
      sensorObj["name"] = sensor.name;
      sensorObj["type"] = sensor.type;
    }

    doc["timestamp"] = millis();

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);

    DEBUG_PRINTLN("[APIServer] Device info requested");
  }

  // ========================================================================
  // NOUVEAU : GET /api/history (basique - pour statistiques Flutter)
  // ========================================================================
  void handleHistory() {
    setCorsHeaders();

    // Note : Ceci est une version BASIQUE
    // Pour un vrai historique, il faudrait stocker les données dans SPIFFS ou SD
    // Ici on renvoie juste les valeurs actuelles comme "historique"

    DynamicJsonDocument doc(2048);

    // Paramètres optionnels
    String sensorType = server.hasArg("sensor") ? server.arg("sensor") : "all";
    int limit = server.hasArg("limit") ? server.arg("limit").toInt() : 10;

    // Pour cette version simple, on crée un historique fictif
    // En production, il faudrait lire depuis un stockage persistant
    JsonArray history = doc.createNestedArray("history");

    unsigned long currentTime = millis();
    float tempValue = sensorManager->getTemperature();
    float lightValue = sensorManager->getLightLevel();

    // Générer quelques points "historiques" (simulation)
    for (int i = 0; i < min(limit, 10); i++) {
      JsonObject point = history.createNestedObject();
      point["timestamp"] = currentTime - (i * 5000);  // Points espacés de 5s

      if (sensorType == "all" || sensorType == "temperature") {
        point["temperature"] = tempValue + (random(-10, 10) / 10.0);
      }

      if (sensorType == "all" || sensorType == "light") {
        point["light"] = lightValue + random(-50, 50);
      }
    }

    doc["count"] = history.size();
    doc["sensorType"] = sensorType;
    doc["timestamp"] = currentTime;

    // NOTE pour étudiants
    doc["note"] = "This is a BASIC history endpoint. For production, implement persistent storage (SPIFFS/SD)";

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);

    DEBUG_PRINTLN("[APIServer] History requested");
  }

  // ========================================================================
  // 404 Not Found
  // ========================================================================
  void handleNotFound() {
    setCorsHeaders();

    DynamicJsonDocument doc(256);
    doc["error"] = "Endpoint not found";
    doc["path"] = server.uri();
    doc["method"] = (server.method() == HTTP_GET) ? "GET" : "POST";

    String response;
    serializeJson(doc, response);
    server.send(404, "application/json", response);
  }
};

// Instance globale
APIServer apiServer(SERVER_PORT);
