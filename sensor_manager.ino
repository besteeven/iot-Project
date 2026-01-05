/*
  ============================================================================
  SENSOR MANAGER - Gestion des capteurs
  ============================================================================
*/

class SensorManager {
  private:
    SensorData sensors[2];
    unsigned long lastReadTime;
    int sensorCount;

  public:
    SensorManager() : sensorCount(2), lastReadTime(0) {
      // Initialiser le capteur de température
      sensors[0].name = "Sensor_1";
      sensors[0].type = "temperature";
      sensors[0].minValue = -40.0;
      sensors[0].maxValue = 80.0;
      sensors[0].value = 0.0;
      sensors[0].lastRead = 0;

      // Initialiser le capteur de lumière
      sensors[1].name = "Sensor_2";
      sensors[1].type = "light";
      sensors[1].minValue = 0.0;
      sensors[1].maxValue = 4095.0;
      sensors[1].value = 0.0;
      sensors[1].lastRead = 0;
    }

    void begin() {
      // Configurer ADC pour capteur lumière
      pinMode(LIGHT_SENSOR_PIN, INPUT);
      analogSetAttenuation(ADC_11db);
      
      // Configurer pin capteur température
      pinMode(TEMP_SENSOR_PIN, INPUT);
      
      DEBUG_PRINTLN("[SensorManager] Sensors initialized");
    }

    void readSensors() {
      sensors[0].value = readTemperature();
      sensors[0].lastRead = millis();
      
      sensors[1].value = readLightLevel();
      sensors[1].lastRead = millis();
      
      lastReadTime = millis();
      DEBUG_PRINTLN("[SensorManager] Sensors read");
    }

    SensorData getSensor(int index) {
      if (index >= 0 && index < sensorCount) {
        return sensors[index];
      }
      return sensors[0];
    }

    int getSensorCount() {
      return sensorCount;
    }

    float getTemperature() {
      return sensors[0].value;
    }

    float getLightLevel() {
      return sensors[1].value;
    }

    bool shouldRead() {
      return (millis() - lastReadTime) >= SENSOR_READ_INTERVAL;
    }

  private:
    float readTemperature() {
      // Simulation température entre 20-30°C
      float simulated = 20.0 + (analogRead(TEMP_SENSOR_PIN) / 4095.0) * 10.0;
      return simulated;
    }

    float readLightLevel() {
      int raw = analogRead(LIGHT_SENSOR_PIN);
      return (float)raw;
    }
};

// Instance globale
SensorManager sensorManager;
