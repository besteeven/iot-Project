/*
  ============================================================================
  IoT SENSOR API SERVER - MAIN (VERSION FINALE)
  TTGO T-Display ESP32
  
  CORRECTIONS APPLIQUÉES :
  - Suppression du contrôle LED en dur qui contournait l'API
  - Ajout de la gestion des boutons
  - Activation AUTOMATIQUE du seuil lumière au démarrage
  - Logique inversée pour lumière (LED ON quand sombre)
  
  COMPORTEMENT :
  - Au démarrage : seuil lumière ACTIVÉ automatiquement
  - LED s'allume automatiquement si lumière < 500
  - Boutons permettent d'activer/désactiver les seuils
  - API permet contrôle manuel ET configuration seuils
  
  ENDPOINTS API:
  - GET  /api/sensors              Lister tous les capteurs
  - GET  /api/sensor?id=X          Obtenir un capteur
  - GET  /api/led                  État LED
  - POST /api/led                  Contrôler LED
  - GET  /api/thresholds           Lister seuils
  - POST /api/thresholds           Configurer seuil
  - GET  /api/status               État du système
  
  BOUTONS TTGO :
  - Bouton UP (pin 35) : Toggle seuil température
  - Bouton DOWN (pin 0) : Toggle seuil lumière
  
  ============================================================================
*/

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

extern SensorManager sensorManager;
extern LEDController ledController;
extern WiFiManager wifiManager;
extern APIServer apiServer;

TFT_eSPI tft = TFT_eSPI();

// ============================================================================
// VARIABLES GLOBALES POUR BOUTONS
// ============================================================================

bool lastButtonUpState = HIGH;
bool lastButtonDownState = HIGH;
unsigned long lastButtonPressTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;

// ============================================================================
// FONCTION : Gestion des boutons
// ============================================================================

void handleButtons() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastButtonPressTime < DEBOUNCE_DELAY) {
    return;
  }
  
  bool currentButtonUpState = digitalRead(BUTTON_UP_PIN);
  bool currentButtonDownState = digitalRead(BUTTON_DOWN_PIN);
  
  // BOUTON UP : Toggle seuil température
  if (lastButtonUpState == HIGH && currentButtonUpState == LOW) {
    bool isEnabled = ledController.isThresholdEnabled("temperature");
    
    if (isEnabled) {
      ledController.disableThreshold("temperature");
      DEBUG_PRINTLN("[BUTTON] Temperature threshold DISABLED");
    } else {
      ledController.enableThreshold("temperature");
      DEBUG_PRINTLN("[BUTTON] Temperature threshold ENABLED");
    }
    
    lastButtonPressTime = currentTime;
  }
  
  // BOUTON DOWN : Toggle seuil lumière
  if (lastButtonDownState == HIGH && currentButtonDownState == LOW) {
    bool isEnabled = ledController.isThresholdEnabled("light");
    
    if (isEnabled) {
      ledController.disableThreshold("light");
      DEBUG_PRINTLN("[BUTTON] Light threshold DISABLED");
    } else {
      ledController.enableThreshold("light");
      DEBUG_PRINTLN("[BUTTON] Light threshold ENABLED");
    }
    
    lastButtonPressTime = currentTime;
  }
  
  lastButtonUpState = currentButtonUpState;
  lastButtonDownState = currentButtonDownState;
}

// ============================================================================
// FONCTION : Mise à jour de l'écran
// ============================================================================

void updateDisplay() {
  static unsigned long lastDisplayUpdate = 0;
  static bool wasConnected = false;
  
  bool isConnected = wifiManager.isWiFiConnected();
  
  // Actualiser l'écran tous les 2 secondes OU quand le status change
  if (isConnected != wasConnected || (millis() - lastDisplayUpdate > 2000)) {
    wasConnected = isConnected;
    lastDisplayUpdate = millis();
    
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    
    if (isConnected) {
      // En-tête WiFi
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.printf("WiFi: OK\n");
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      tft.printf("IP: %s\n", wifiManager.getIPAddress().c_str());
      
      // Données capteurs
      tft.setTextColor(TFT_YELLOW, TFT_BLACK);
      tft.printf("Temp: %.1fC\n", sensorManager.getTemperature());
      tft.printf("Light: %.0f\n", sensorManager.getLightLevel());
      
      // État LED avec couleur dynamique
      tft.setTextColor(ledController.getState() ? TFT_GREEN : TFT_RED, TFT_BLACK);
      tft.printf("LED: %s\n", ledController.getState() ? "ON" : "OFF");
      
      
    } else {
      tft.setTextColor(TFT_ORANGE, TFT_BLACK);
      tft.println("Connecting...");
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.printf("SSID: %s\n", WIFI_SSID);
    }
  }
}

// ============================================================================
// SETUP - INITIALISATION
// ============================================================================

void setup() {
  // Initialiser communication série
  Serial.begin(115200);
  delay(1000);
  
  DEBUG_PRINTLN("\n\n╔═══════════════════════════════════════════════╗");
  DEBUG_PRINTLN("║   IoT Sensor API Server - TTGO T-Display     ║");
  DEBUG_PRINTLN("╚═══════════════════════════════════════════════╝\n");
  
  // Initialiser l'écran TFT
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("Booting...");
  delay(500);
  
  // Configurer les boutons en INPUT_PULLUP
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  DEBUG_PRINTLN("[SETUP] Buttons configured");
  
  // Initialiser les composants
  DEBUG_PRINTLN("[SETUP] Initializing sensors...");
  sensorManager.begin();
  
  DEBUG_PRINTLN("[SETUP] Initializing LED controller...");
  ledController.begin();
  
  // Première lecture des capteurs
  sensorManager.readSensors();
  
  // ★★★ ACTIVER LE SEUIL LUMIÈRE PAR DÉFAUT ★★★
  // Ceci permet à la LED de s'allumer automatiquement quand il fait sombre
  ledController.enableThreshold("light");
  DEBUG_PRINTLN("[SETUP] ★ Light threshold ENABLED by default");
  DEBUG_PRINT("[SETUP] ★ LED will turn ON when light < ");
  DEBUG_PRINTLN(DEFAULT_LIGHT_THRESHOLD);
  
  // Appliquer le seuil immédiatement
  ledController.checkAndApplyThreshold(sensorManager.getLightLevel(), "light");
  
  // Connexion WiFi
  tft.println("WiFi...");
  DEBUG_PRINTLN("[SETUP] Connecting to WiFi...");
  
  if (wifiManager.begin(WIFI_SSID, WIFI_PASSWORD)) {
    DEBUG_PRINTLN("✓ WiFi connected!");
    DEBUG_PRINT("✓ IP Address: ");
    DEBUG_PRINTLN(wifiManager.getIPAddress());
    
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("WiFi OK!");
    tft.setTextSize(1);
    tft.print("IP: ");
    tft.println(wifiManager.getIPAddress());
  } else {
    DEBUG_PRINTLN("⚠ WiFi connection failed");
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.println("WiFi FAILED!");
  }
  
  // Démarrer le serveur API
  DEBUG_PRINTLN("[SETUP] Starting API server...");
  apiServer.begin(&sensorManager, &ledController);
  
  DEBUG_PRINT("✓ API Server at: http://");
  DEBUG_PRINT(wifiManager.getIPAddress());
  DEBUG_PRINTLN("/api/status");
  
  DEBUG_PRINTLN("\n╔═══════════════════════════════════════════════╗");
  DEBUG_PRINTLN("║          System Ready! 🚀                     ║");
  DEBUG_PRINTLN("║  LED auto-contrôlée par seuil lumière        ║");
  DEBUG_PRINTLN("╚═══════════════════════════════════════════════╝\n");
  
  delay(2000);
}

// ============================================================================
// LOOP - BOUCLE PRINCIPALE
// ============================================================================

void loop() {
  // 1. Gérer la connexion WiFi
  wifiManager.handleConnection();
  
  // 2. Traiter les requêtes HTTP
  apiServer.handleClient();
  
  // 3. Gérer les boutons
  handleButtons();
  
  // 4. Lire les capteurs périodiquement
  if (sensorManager.shouldRead()) {
    sensorManager.readSensors();
    
    // 5. Appliquer les seuils automatiques SI activés
    // Pour température
    ledController.checkAndApplyThreshold(
      sensorManager.getTemperature(), 
      "temperature"
    );
    
    // Pour lumière (avec logique inversée)
    // Si lumière < 2500 (sombre) → LED ON
    // Si lumière > 2500 (lumineux) → LED OFF
    ledController.checkAndApplyThreshold(
      sensorManager.getLightLevel(),
      "light"
    );
  }
  
  // 6. Mettre à jour l'affichage écran
  updateDisplay();
  
  // 7. Petite pause
  delay(100);
}
