/*
  ============================================================================
  LED CONTROLLER - Contrôle de la LED et gestion des seuils
  
  MODIFICATION : Le seuil de lumière utilise une logique INVERSÉE par défaut
  - Si lumière < seuil (sombre) → LED ON
  - Si lumière > seuil (lumineux) → LED OFF
  ============================================================================
*/

class LEDController {
private:
  bool ledState;
  ThresholdConfig thresholds[2];
  bool autoMode;

public:
  LEDController()
    : ledState(false), autoMode(true) {
    thresholds[0].sensorType = "temperature";
    thresholds[0].threshold = DEFAULT_TEMP_THRESHOLD;
    thresholds[0].enabled = false;
    thresholds[0].inverted = false;

    thresholds[1].sensorType = "light";
    thresholds[1].threshold = DEFAULT_LIGHT_THRESHOLD;
    thresholds[1].enabled = false;
    thresholds[1].inverted = true;
  }


  void begin() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    DEBUG_PRINTLN("[LEDController] LED initialized");
  }

  void turnOn() {
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
    DEBUG_PRINTLN("[LEDController] LED turned ON");
  }

  void turnOff() {
    digitalWrite(LED_PIN, LOW);
    ledState = false;
    DEBUG_PRINTLN("[LEDController] LED turned OFF");
  }

  void toggle() {
    if (ledState) turnOff();
    else turnOn();
  }

  bool getState() {
    return ledState;
  }

  void setMode(String mode) {
    mode.toLowerCase();
    if (mode == "auto") {
      autoMode = true;
    } else if (mode == "manual") {
      autoMode = false;
    }
  }

  String getMode() {
    return autoMode ? "auto" : "manual";
  }

  bool isAutoMode() {
    return autoMode;
  }


  void setThreshold(String sensorType, float value) {
    for (int i = 0; i < 2; i++) {
      if (thresholds[i].sensorType == sensorType) {
        thresholds[i].threshold = value;
        DEBUG_PRINT("[LEDController] Threshold set for ");
        DEBUG_PRINT(sensorType);
        DEBUG_PRINT(": ");
        DEBUG_PRINTLN(value);
        return;
      }
    }
  }

  float getThreshold(String sensorType) {
    for (int i = 0; i < 2; i++) {
      if (thresholds[i].sensorType == sensorType) {
        return thresholds[i].threshold;
      }
    }
    return 0.0;
  }

  void enableThreshold(String sensorType) {
    for (int i = 0; i < 2; i++) {
      if (thresholds[i].sensorType == sensorType) {
        thresholds[i].enabled = true;
        DEBUG_PRINT("[LEDController] Threshold ENABLED for ");
        DEBUG_PRINTLN(sensorType);
        return;
      }
    }
  }

  void disableThreshold(String sensorType) {
    for (int i = 0; i < 2; i++) {
      if (thresholds[i].sensorType == sensorType) {
        thresholds[i].enabled = false;
        DEBUG_PRINT("[LEDController] Threshold DISABLED for ");
        DEBUG_PRINTLN(sensorType);
        return;
      }
    }
  }

  bool isThresholdEnabled(String sensorType) {
    for (int i = 0; i < 2; i++) {
      if (thresholds[i].sensorType == sensorType) {
        return thresholds[i].enabled;
      }
    }
    return false;
  }

  /**
     * Applique le seuil avec support de la logique inversée
     * 
     * Si inverted = false (température) : LED ON si valeur > seuil
     * Si inverted = true (lumière) : LED ON si valeur < seuil (sombre)
     */
  void checkAndApplyThreshold(float sensorValue, String sensorType) {
    if (!autoMode) return;
    for (int i = 0; i < 2; i++) {
      if (thresholds[i].sensorType == sensorType && thresholds[i].enabled) {
        bool shouldTurnOn;

        if (thresholds[i].inverted) {
          // Logique INVERSÉE (pour lumière)
          // LED ON si valeur < seuil (sombre)
          shouldTurnOn = sensorValue < thresholds[i].threshold;
        } else {
          // Logique NORMALE (pour température)
          // LED ON si valeur > seuil (chaud)
          shouldTurnOn = sensorValue > thresholds[i].threshold;
        }

        if (shouldTurnOn) {
          turnOn();
          DEBUG_PRINT("[LEDController] Threshold triggered (");
          DEBUG_PRINT(sensorType);
          DEBUG_PRINT("): ");
          DEBUG_PRINT(sensorValue);
          DEBUG_PRINT(thresholds[i].inverted ? " < " : " > ");
          DEBUG_PRINTLN(thresholds[i].threshold);
        } else {
          turnOff();
        }
        return;
      }
    }
  }

  ThresholdConfig getThresholdConfig(String sensorType) {
    for (int i = 0; i < 2; i++) {
      if (thresholds[i].sensorType == sensorType) {
        return thresholds[i];
      }
    }
    return thresholds[0];
  }
};

// Instance globale
LEDController ledController;
