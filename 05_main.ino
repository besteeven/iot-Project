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
  // Initialiser communication série
  Serial.begin(115200);
  delay(1000);
  
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  

  DEBUG_PRINTLN("Initializing system...");
  
  // Initialiser les composants dans l'ordre
  sensorManager.begin();      // Capteurs en premier
  ledController.begin();      // LED ensuite
  
  // Connexion WiFi
  if (wifiManager.begin(WIFI_SSID, WIFI_PASSWORD)) {
    DEBUG_PRINTLN("✓ WiFi connected!");
    DEBUG_PRINT("✓ IP: ");
    DEBUG_PRINTLN(wifiManager.getIPAddress());
  } else {
    DEBUG_PRINTLN("⚠ WiFi failed - retrying...");
  }
  
  // Démarrer le serveur API
  apiServer.begin(&sensorManager, &ledController);
  DEBUG_PRINT("✓ API running at http://");
  DEBUG_PRINT(wifiManager.getIPAddress());
  DEBUG_PRINTLN("/api/sensors");
  
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("╔═══════════════════════════════════════════════╗");
  DEBUG_PRINTLN("║          System Ready! 🚀                     ║");
  DEBUG_PRINTLN("╚═══════════════════════════════════════════════╝");
  DEBUG_PRINTLN("");
  ledController.begin();
  pinMode(36,INPUT);
}

// ============================================================================
// LOOP - BOUCLE PRINCIPALE
// ============================================================================
void loop() {
  static bool wasConnected = false;
  static unsigned long lastDisplayUpdate = 0;
  
  tft.fillScreen(TFT_BLACK);
  int valeurLumiere=analogRead(36);
  if (valeurLumiere < 500) {
  tft.println("LED STATE : ON");
  tft.print("valeur lum: ");
  tft.println(valeurLumiere);
  ledController.turnOn();
  }
  else {
  tft.println("LED STATE : OFF");
  tft.print("valeur lum: ");
  tft.println(valeurLumiere);
  ledController.turnOff();
  }







  


  // Gérer la connexion WiFi
  wifiManager.handleConnection();
  
  bool isConnected = wifiManager.isWiFiConnected();
  
  // Actualiser l'écran tous les 1000ms (1 seconde) OU quand le status change
  if (isConnected != wasConnected || (millis() - lastDisplayUpdate > 1000)) {
    wasConnected = isConnected;
    lastDisplayUpdate = millis();
    
    //tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    
    if (isConnected) {
      tft.printf("Connected on the wifi '%s'!\n", WIFI_SSID);
      tft.printf("IP: %s\n", wifiManager.getIPAddress().c_str());
      tft.printf("Temp: %.1f C\n", sensorManager.getTemperature());
      tft.printf("Light: %.0f\n", sensorManager.getLightLevel());
    } else {
      tft.printf("Connecting...\n");
      tft.printf("SSID: %s\n", WIFI_SSID);
    }
  }
  
  // Traiter les requêtes API
  apiServer.handleClient();
  
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
  
  // Petite pause pour éviter watchdog
  delay(2000);
}
