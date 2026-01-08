/*
  ============================================================================
  IoT SENSOR API SERVER - MAIN
  TTGO T-Display ESP32 (Version 1.0.0)
  
  INSTALLATION:
  1. Créer un dossier: iot_sensor_api
  2. Copier TOUS les 6 fichiers .ino dans ce dossier
  3. Ouvrir main.ino dans Arduino IDE
  4. Modifier config.ino (WiFi)
  5. Installer ArduinoJSON (Benoit Blanchon)
  6. Upload
  
  ENDPOINTS API:
  - GET  /api/sensors              Lister tous les capteurs
  - GET  /api/sensor?id=X          Obtenir un capteur
  - GET  /api/led                  État LED
  - POST /api/led                  Contrôler LED
  - GET  /api/thresholds           Lister seuils
  - POST /api/thresholds           Configurer seuil
  - GET  /api/status               État du système
  
  ============================================================================
*/

// ============================================================================
// INCLUDES - OBLIGATOIRE POUR QUE LES CLASSES SOIENT VISIBLES
// ============================================================================



// Note: Les autres fichiers .ino (config, sensor_manager, etc.)
// seront inclus automatiquement par Arduino IDE
// Pas besoin de les inclure manuellement

// ============================================================================
// FORWARD DECLARATIONS - Pour que main.ino compile correctement
// ============================================================================

// Ces variables seront définies dans les autres fichiers .ino
extern SensorManager sensorManager;
extern LEDController ledController;
extern WiFiManager wifiManager;
extern APIServer apiServer;
TFT_eSPI tft = TFT_eSPI();
// ============================================================================
// SETUP - INITIALISATION
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  
  DEBUG_PRINTLN("Initializing system...");
  
  // Initialiser les composants dans l'ordre
  sensorManager.begin();      // Capteurs en premier (gère déjà le port 34)
  ledController.begin();      // LED ensuite
   ledController.setThreshold("light", 2000);  // Seuil à 2000
   ledController.setInverted("light", true);  // ← Ajoute ça!
  ledController.enableThreshold("light");
 
  
  // Connexion WiFi
  if (wifiManager.begin(WIFI_SSID, WIFI_PASSWORD)) {
    DEBUG_PRINTLN("✓ WiFi connected!");
    DEBUG_PRINT("✓ IP: ");
    DEBUG_PRINTLN(wifiManager.getIPAddress());
  } else {
    DEBUG_PRINTLN("⚠ WiFi failed - retrying...");
  }
  
  apiServer.begin(&sensorManager, &ledController);
  DEBUG_PRINT("✓ API running at http://");
  DEBUG_PRINT(wifiManager.getIPAddress());
  DEBUG_PRINTLN("/api/sensors");
}

void loop() {
  static bool wasConnected = false;
  static unsigned long lastDisplayUpdate = 0;
  
  // Gérer la connexion WiFi
  wifiManager.handleConnection();
  
  bool isConnected = wifiManager.isWiFiConnected();
  
  // Lire les capteurs périodiquement
  if (sensorManager.shouldRead()) {
    sensorManager.readSensors();
    
    // Appliquer les seuils d'activation
    ledController.checkAndApplyThreshold(
      sensorManager.getTemperature(), 
      "temperature"
    );
    ledController.checkAndApplyThreshold(
      sensorManager.getLightLevel(), 
      "light"
    );
  }
  
  // Actualiser l'écran tous les 1000ms
  if (isConnected != wasConnected || (millis() - lastDisplayUpdate > 1000)) {
    wasConnected = isConnected;
    lastDisplayUpdate = millis();
    
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    
    if (isConnected) {
      tft.printf("Connected!\n");
      tft.printf("IP: %s\n", wifiManager.getIPAddress().c_str());
    } else {
      tft.printf("Connecting...\n");
      tft.printf("SSID: %s\n", WIFI_SSID);
    }
    
    // Afficher les capteurs via sensorManager
    tft.printf("\nTemp: %.1f C\n", sensorManager.getTemperature());
    tft.printf("Light: %.0f\n", sensorManager.getLightLevel());
    tft.printf("LED: %s\n", ledController.getState() ? "ON" : "OFF");
  }
  
  // Traiter les requêtes API
  apiServer.handleClient();
  
  delay(100);
}
