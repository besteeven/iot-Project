#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <math.h>
#include <TFT_eSPI.h>
#include <SPI.h>

extern TFT_eSPI tft;

// ============================================================================
// CONFIGURATION WIFI - À PERSONNALISER
// ============================================================================

#define WIFI_SSID "SALEM Ahmedou"              //  à remplacer
#define WIFI_PASSWORD "Min3etity!"            // à remplacer

// ============================================================================
// CONFIGURATION SERVEUR
// ============================================================================

#define SERVER_PORT 80

// ============================================================================
// GPIO PINS - TTGO T-Display
// ============================================================================

// LED contrôlable
#define LED_PIN 32

// Capteurs (ADC)
#define LIGHT_SENSOR_PIN 36     // Pin ADC pour capteur lumière (UNIFIÉ)
#define TEMP_SENSOR_PIN 39      // Pin ADC pour thermistance NTC

// Boutons TTGO T-Display (avec pull-up interne)
#define BUTTON_UP_PIN 35        // Bouton haut (GPIO 35)
#define BUTTON_DOWN_PIN 0       // Bouton bas (GPIO 0)

// ============================================================================
// PARAMÈTRES SYSTÈME
// ============================================================================

#define SENSOR_READ_INTERVAL 2000           // Lire les capteurs toutes les 2 secondes
#define DEFAULT_LIGHT_THRESHOLD 2500.0       // Seuil lumière (0-4095) - LED ON si < 2500
#define DEFAULT_TEMP_THRESHOLD 25.0         // Seuil température (°C)

// ============================================================================
// PARAMÈTRES THERMISTANCE NTC (pour calcul température)
// ============================================================================

#define THERMISTOR_NOMINAL 10000.0          // Résistance à 25°C (10kΩ)
#define TEMP_NOMINAL 25.0                   // Température nominale (25°C)
#define BETA_COEFFICIENT 3950.0             // Coefficient Beta de la thermistance
#define SERIES_RESISTOR 10000.0             // Résistance série dans le diviseur (10kΩ)

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

/**
 * Structure pour stocker les données d'un capteur
 */
struct SensorData {
  String name;              // Nom du capteur (ex: "Sensor_1")
  String type;              // Type: "temperature" ou "light"
  float value;              // Valeur actuelle
  float minValue;           // Valeur minimale théorique
  float maxValue;           // Valeur maximale théorique
  unsigned long lastRead;   // Timestamp dernière lecture (millis)
};

/**
 * Structure pour la configuration des seuils
 */
struct ThresholdConfig {
  String sensorType;        // Type de capteur concerné
  float threshold;          // Valeur du seuil
  bool enabled;             // Seuil activé ou non
  bool inverted;            // false: LED ON si valeur > seuil
                            // true:  LED ON si valeur < seuil (pour lumière)
};
