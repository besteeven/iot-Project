/*
  ============================================================================
  SENSOR MANAGER - Gestion des capteurs (VERSION CORRIGÉE)
  
  Corrections:
  - Calcul température avec équation de Steinhart-Hart pour thermistance NTC
  - Utilisation du pin 36 pour capteur lumière (unifié)
  - Amélioration de la lecture ADC avec moyennage
  ============================================================================
*/

class SensorManager {
  private:
    SensorData sensors[2];
    unsigned long lastReadTime;
    int sensorCount;

  public:
    /**
     * Constructeur - Initialise les capteurs
     */
    SensorManager() : sensorCount(2), lastReadTime(0) {
      // Capteur 0 : Température (thermistance NTC)
      sensors[0].name = "Sensor_1";
      sensors[0].type = "temperature";
      sensors[0].minValue = -40.0;
      sensors[0].maxValue = 80.0;
      sensors[0].value = 0.0;
      sensors[0].lastRead = 0;

      // Capteur 1 : Lumière (photorésistance LDR)
      sensors[1].name = "Sensor_2";
      sensors[1].type = "light";
      sensors[1].minValue = 0.0;
      sensors[1].maxValue = 4095.0;
      sensors[1].value = 0.0;
      sensors[1].lastRead = 0;
    }

    /**
     * Initialisation des pins capteurs
     */
    void begin() {
      // Configurer ADC pour capteur lumière (pin 36)
      pinMode(LIGHT_SENSOR_PIN, INPUT);
      
      // Configurer pin capteur température (pin 39)
      pinMode(TEMP_SENSOR_PIN, INPUT);
      
      // Configuration ADC : atténuation 11dB pour plage 0-3.3V
      analogSetAttenuation(ADC_11db);
      
      DEBUG_PRINTLN("[SensorManager] Sensors initialized");
      DEBUG_PRINT("[SensorManager] Light sensor on pin: ");
      DEBUG_PRINTLN(LIGHT_SENSOR_PIN);
      DEBUG_PRINT("[SensorManager] Temperature sensor on pin: ");
      DEBUG_PRINTLN(TEMP_SENSOR_PIN);
    }

    /**
     * Lecture des capteurs
     */
    void readSensors() {
      sensors[0].value = readTemperature();
      sensors[0].lastRead = millis();
      
      sensors[1].value = readLightLevel();
      sensors[1].lastRead = millis();
      
      lastReadTime = millis();
      
      DEBUG_PRINT("[SensorManager] Temp: ");
      DEBUG_PRINT(sensors[0].value);
      DEBUG_PRINT("°C | Light: ");
      DEBUG_PRINTLN(sensors[1].value);
    }

    /**
     * Obtenir les données d'un capteur par index
     */
    SensorData getSensor(int index) {
      if (index >= 0 && index < sensorCount) {
        return sensors[index];
      }
      // Retourner le premier capteur par défaut si index invalide
      return sensors[0];
    }

    /**
     * Obtenir le nombre de capteurs
     */
    int getSensorCount() {
      return sensorCount;
    }

    /**
     * Obtenir la température actuelle
     */
    float getTemperature() {
      return sensors[0].value;
    }

    /**
     * Obtenir le niveau de lumière actuel
     */
    float getLightLevel() {
      return sensors[1].value;
    }

    /**
     * Vérifier s'il est temps de relire les capteurs
     */
    bool shouldRead() {
      return (millis() - lastReadTime) >= SENSOR_READ_INTERVAL;
    }

  private:
    /**
     * Lire la température depuis la thermistance NTC
     * Utilise l'équation de Steinhart-Hart simplifiée
     * 
     * @return Température en degrés Celsius
     */
    float readTemperature() {
      // Faire plusieurs lectures et moyenner pour réduire le bruit
      const int NUM_SAMPLES = 5;
      float sum = 0;
      
      for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += analogRead(TEMP_SENSOR_PIN);
        delay(10);
      }
      
      float average = sum / NUM_SAMPLES;
      
      // Convertir ADC (0-4095) en résistance de la thermistance
      // Circuit : 3.3V ---[THERMISTOR]---+---[10kΩ]--- GND
      //                                  |
      //                                  ADC
      
      if (average >= 4095) {
        // Protection contre division par zéro
        return TEMP_NOMINAL;
      }
      
      // Calcul de la résistance de la thermistance
      float resistance = SERIES_RESISTOR / ((4095.0 / average) - 1.0);
      
      // Équation de Steinhart-Hart simplifiée (modèle Beta)
      // 1/T = 1/T0 + (1/B) * ln(R/R0)
      // où T est en Kelvin
      
      float steinhart;
      steinhart = resistance / THERMISTOR_NOMINAL;        // R/R0
      steinhart = log(steinhart);                         // ln(R/R0)
      steinhart /= BETA_COEFFICIENT;                      // 1/B * ln(R/R0)
      steinhart += 1.0 / (TEMP_NOMINAL + 273.15);         // + 1/T0
      steinhart = 1.0 / steinhart;                        // Inverser
      steinhart -= 273.15;                                // Convertir Kelvin → Celsius
      
      // Vérifier que la valeur est dans une plage raisonnable
      if (steinhart < -40.0 || steinhart > 80.0) {
        DEBUG_PRINTLN("[SensorManager] WARNING: Temperature out of range!");
        return TEMP_NOMINAL; // Retourner valeur par défaut si aberrante
      }
      
      return steinhart;
    }

    /**
     * Lire le niveau de lumière depuis la photorésistance
     * 
     * @return Niveau de lumière (0-4095, 0=sombre, 4095=très lumineux)
     */
    float readLightLevel() {
      // Faire plusieurs lectures et moyenner pour réduire le bruit
      const int NUM_SAMPLES = 5;
      float sum = 0;
      
      for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += analogRead(LIGHT_SENSOR_PIN);
        delay(10);
      }
      
      float average = sum / NUM_SAMPLES;
      
      // Retourner la valeur brute ADC (0-4095)
      // Plus la valeur est élevée, plus il y a de lumière
      return average;
    }
};

// Instance globale
SensorManager sensorManager;
