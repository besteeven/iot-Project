/*
  ============================================================================
  LED CONTROLLER - Contrôle de la LED et gestion des seuils
  ============================================================================
*/

class LEDController {
  private:
    bool ledState;
    ThresholdConfig thresholds[2];

  public:
    LEDController() : ledState(false) {
      thresholds[0].sensorType = "temperature";
      thresholds[0].threshold = DEFAULT_TEMP_THRESHOLD;
      thresholds[0].enabled = false;
      thresholds[0].inverted = false;
      
      thresholds[1].sensorType = "light";
      thresholds[1].threshold = DEFAULT_LIGHT_THRESHOLD;
      thresholds[1].enabled = false;
      thresholds[1].inverted = false;
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

    void setThreshold(String sensorType, float value) {
      for (int i = 0; i < 2; i++) {
        if (thresholds[i].sensorType == sensorType) {
          thresholds[i].threshold = value;
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
          return;
        }
      }
    }

    void disableThreshold(String sensorType) {
      for (int i = 0; i < 2; i++) {
        if (thresholds[i].sensorType == sensorType) {
          thresholds[i].enabled = false;
          return;
        }
      }
    }

    void setInverted(String sensorType, bool inverted) {
  for (int i = 0; i < 2; i++) {
    if (thresholds[i].sensorType == sensorType) {
      thresholds[i].inverted = inverted;
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

    void checkAndApplyThreshold(float sensorValue, String sensorType) {
      for (int i = 0; i < 2; i++) {
        if (thresholds[i].sensorType == sensorType && thresholds[i].enabled) {
          bool shouldTurnOn = thresholds[i].inverted ? 
            sensorValue < thresholds[i].threshold :
            sensorValue > thresholds[i].threshold;
          
          if (shouldTurnOn) turnOn();
          else turnOff();
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
