
#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <math.h>
#include <TFT_eSPI.h>
#include <SPI.h>

extern TFT_eSPI tft;

// Configuration WiFi

#define WIFI_SSID "iPhone de Michel Steeven"              //  à remplacer
#define WIFI_PASSWORD "0123456789"            // à remplacer

// Configuration serveur

#define SERVER_PORT 80

// Pins GPIO

// LED contrôlable
#define LED_PIN 32

// Boutons TTGO T-Display
#define BUTTON_1 0              // Bouton gauche - Scan WiFi
#define BUTTON_2 35             // Bouton droit - Affichage normal

// Capteurs (ADC)
#define LIGHT_SENSOR_PIN 36     // Pin ADC pour capteur lumière (UNIFIÉ)
#define TEMP_SENSOR_PIN 39      // Pin ADC pour thermistance NTC


// Paramètres

#define SENSOR_READ_INTERVAL 2000           // Lire les capteurs toutes les 2 secondes
#define DEFAULT_LIGHT_THRESHOLD 2500.0       // Seuil lumière (0-4095) - LED ON si < 2500
#define DEFAULT_TEMP_THRESHOLD 25.0         // Seuil température (°C)
#define BUTTON_DEBOUNCE_DELAY 50            // Délai anti-rebond boutons (ms)

// Paramètres thermistance NTC

#define THERMISTOR_NOMINAL 10000.0          // Résistance à 25°C (10kΩ)
#define TEMP_NOMINAL 25.0                   // Température nominale (25°C)
#define BETA_COEFFICIENT 3950.0             // Coefficient Beta de la thermistance
#define SERIES_RESISTOR 10000.0             // Résistance série dans le diviseur (10kΩ)

// Debug

#define DEBUG true              // Activer pour voir les logs (115200 baud)

#if DEBUG
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// Structures de données d'un capteur

 
struct SensorData {
  String name;              // Nom du capteur (ex: "Sensor_1")
  String type;              // "temperature" ou "light"
  float value;              // Valeur actuelle
  float minValue;           // Valeur minimale théorique
  float maxValue;           // Valeur maximale théorique
  unsigned long lastRead;   // Timestamp dernière lecture (millisecondes)
};

// Structure pour la configuration des seuils
 
struct ThresholdConfig {
  String sensorType;        // Type de capteur concerné
  float threshold;          // Valeur du seuil
  bool enabled;             // Seuil activé ou non
  bool inverted;            // false: LED ON si valeur > seuil
                            // true:  LED ON si valeur < seuil (pour lumière)
};

// Énumération pour les modes d'affichage
 
enum DisplayMode {
  DISPLAY_NORMAL,           // Affichage normal (capteurs + LED)
  DISPLAY_WIFI_SCAN         // Affichage des réseaux WiFi disponibles
};
