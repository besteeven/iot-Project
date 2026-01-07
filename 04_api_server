/*
  ============================================================================
  API SERVER - Serveur REST avec 7 endpoints JSON
  ============================================================================
*/

class APIServer {
  private:
    WebServer server;
    SensorManager* sensorManager;
    LEDController* ledController;

  public:
    APIServer(int port = SERVER_PORT) : server(port), sensorManager(nullptr), ledController(nullptr) {}

    void begin(SensorManager* sensors, LEDController* led) {
      sensorManager = sensors;
      ledController = led;
      
      server.on("/api/sensors", HTTP_GET, [this]() { handleSensorsGet(); });
      server.on("/api/sensor", HTTP_GET, [this]() { handleSensorGet(); });
      server.on("/api/led", HTTP_GET, [this]() { handleLEDGet(); });
      server.on("/api/led", HTTP_POST, [this]() { handleLEDPost(); });
      server.on("/api/thresholds", HTTP_GET, [this]() { handleThresholdGet(); });
      server.on("/api/thresholds", HTTP_POST, [this]() { handleThresholdPost(); });
      server.on("/api/status", HTTP_GET, [this]() { handleStatusGet(); });
      server.onNotFound([this]() { handleNotFound(); });
      
      server.begin();
      DEBUG_PRINTLN("[APIServer] Server started on port " + String(SERVER_PORT));
    }

    void handleClient() {
      server.handleClient();
    }

  private:
    void handleSensorsGet() {
      DynamicJsonDocument doc(512);
      JsonArray sensorsArray = doc.createNestedArray("sensors");
      
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
      }
      
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    }

    void handleSensorGet() {
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
      DynamicJsonDocument doc(256);
      doc["id"] = sensorId;
      doc["name"] = sensor.name;
      doc["type"] = sensor.type;
      doc["value"] = sensor.value;
      doc["unit"] = (sensor.type == "temperature") ? "°C" : "lux";
      
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    }

    void handleLEDGet() {
      DynamicJsonDocument doc(256);
      doc["status"] = ledController->getState() ? "on" : "off";
      doc["state"] = ledController->getState();
      
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    }

    void handleLEDPost() {
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
      String response;
      serializeJson(respDoc, response);
      server.send(200, "application/json", response);
    }

    void handleThresholdGet() {
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
      
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    }

    void handleThresholdPost() {
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
      String response;
      serializeJson(respDoc, response);
      server.send(200, "application/json", response);
    }

    void handleStatusGet() {
      DynamicJsonDocument doc(256);
      doc["uptime"] = millis();
      doc["ledStatus"] = ledController->getState() ? "on" : "off";
      doc["sensorCount"] = sensorManager->getSensorCount();
      
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    }

    void handleNotFound() {
      DynamicJsonDocument doc(256);
      doc["error"] = "Endpoint not found";
      doc["path"] = server.uri();
      
      String response;
      serializeJson(doc, response);
      server.send(404, "application/json", response);
    }
};

// Instance globale
APIServer apiServer(SERVER_PORT);
