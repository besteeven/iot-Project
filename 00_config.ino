#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <math.h>
#include <TFT_eSPI.h>
#include <SPI.h>

extern TFT_eSPI tft;


#define WIFI_SSID "iPhone de Michel Steeven"              // ← À REMPLACER
#define WIFI_PASSWORD "0123456789"      // ← À REMPLACER



// Exemple:
// #define WIFI_SSID "MonWiFi"
// #define WIFI_PASSWORD "MonMotDePasse123"

// ============================================================================
// CONFIGURATION SERVEUR
// ============================================================================

#define SERVER_PORT 80

// ============================================================================
// GPIO PINS
// ============================================================================

#define LED_PIN 32             // LED contrôlable
#define LIGHT_SENSOR_PIN 36     // Capteur lumière (ADC)
#define TEMP_SENSOR_PIN 39      // Capteur température
#define BUTTON_UP_PIN 38        // Bouton UP (optionnel)
#define BUTTON_DOWN_PIN 37      // Bouton DOWN (optionnel)

// ============================================================================
// PARAMÈTRES
// ============================================================================

#define SENSOR_READ_INTERVAL 500          // Lire les capteurs toutes les 0.5 secondes
#define DEFAULT_LIGHT_THRESHOLD 2048        // Seuil lumière (0-4095)
#define DEFAULT_TEMP_THRESHOLD 25.0         // Seuil température (°C)

// ============================================================================
// DEBUG - Logs en série
// ============================================================================

#define DEBUG true              // Activer pour voir les logs (115200 baud)

#if DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// ============================================================================
// STRUCTURES DE DONNÉES
// ============================================================================

struct SensorData {
  String name;
  String type;        // "temperature" ou "light"
  float value;
  float minValue;
  float maxValue;
  unsigned long lastRead;
};

struct ThresholdConfig {
  String sensorType;
  float threshold;
  bool enabled;
  bool inverted;
};
