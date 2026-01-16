// IoT Sensor API Server


extern SensorManager sensorManager;
extern LEDController ledController;
extern WiFiManager wifiManager;
extern APIServer apiServer;
extern ButtonManager buttonManager;

TFT_eSPI tft = TFT_eSPI();

 



// Affichage scan WiFi

void displayWiFiScan() {
  static unsigned long lastScan = 0;
  static int numberOfNetworks = 0;
  static String networks[20]; // Stocker jusqu'à 20 réseaux
  static int rssi[20];
  static bool encrypted[20];
  
  // Scanner tous les 10 secondes
  if (millis() - lastScan > 10000 || lastScan == 0) {
    lastScan = millis();
    buttonManager.resetScrollOffset(); // Reset du défilement lors d'un nouveau scan
    
    DEBUG_PRINTLN("[Display] Scanning WiFi networks...");
    
    // S'assurer que le WiFi est en mode station pour pouvoir scanner
    // Le scan fonctionne même si l'ESP32 est déjà connecté à un réseau
    WiFi.mode(WIFI_STA);
    
    numberOfNetworks = WiFi.scanNetworks();
    
    if (numberOfNetworks > 20) numberOfNetworks = 20;
    
    for (int i = 0; i < numberOfNetworks; i++) {
      networks[i] = WiFi.SSID(i);
      rssi[i] = WiFi.RSSI(i);
      encrypted[i] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
    }
    
    DEBUG_PRINT("[Display] Found ");
    DEBUG_PRINT(numberOfNetworks);
    DEBUG_PRINTLN(" networks");
  }
  
  // Récupérer l'offset de défilement
  int scrollOffset = buttonManager.getScrollOffset();
  
  // Calculer les indices de début et fin pour l'affichage
  const int ITEMS_PER_PAGE = 8; // Nombre de réseaux par page
  int startIndex = scrollOffset * ITEMS_PER_PAGE;
  int endIndex = min(startIndex + ITEMS_PER_PAGE, numberOfNetworks);
  
  // Limiter le scroll si  on est à la fin
  if (startIndex >= numberOfNetworks && numberOfNetworks > 0) {
    buttonManager.resetScrollOffset();
    startIndex = 0;
    endIndex = min(ITEMS_PER_PAGE, numberOfNetworks);
  }
  
  // Afficher les résultats
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  
  // En-tête
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.println("=== RESEAUX WiFi ===");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.printf("Trouves: %d reseaux\n", numberOfNetworks);
  
  // Indicateur de page
  if (numberOfNetworks > ITEMS_PER_PAGE) {
    int totalPages = (numberOfNetworks + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
    int currentPage = scrollOffset + 1;
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.printf("Page %d/%d\n", currentPage, totalPages);
  }
  
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.println("--------------------");
  
  if (numberOfNetworks == 0) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.println("Aucun reseau WiFi");
    tft.println("disponible");
  } else {
    // Afficher les réseaux de la page actuelle
    for (int i = startIndex; i < endIndex; i++) {
      // Numéro du réseau (numérotation globale)
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      tft.printf("%d. ", i + 1);
      
      // Nom du réseau WiFi (SSID) - affichage complet
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      String ssid = networks[i];
      
      // Afficher le SSID (40 caractères max par ligne en taille 1)
      if (ssid.length() > 36) {
        tft.println(ssid.substring(0, 36));
        tft.println("   " + ssid.substring(36));
      } else {
        tft.println(ssid);
      }
    }
  }
  

}

// Update display

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

// Setup

void setup() {
  Serial.begin(115200);
  delay(5000);
  run_all_unit_tests();
  delay(2000);
  
  DEBUG_PRINTLN("\n--- IoT Sensor API Server ---");
  DEBUG_PRINTLN("Starting...");
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("Booting...");
  delay(500);
  
  DEBUG_PRINTLN("Init buttons...");
  buttonManager.begin();
  
  WiFi.mode(WIFI_STA);
  DEBUG_PRINTLN("WiFi mode: STATION");
  
  DEBUG_PRINTLN("Init sensors...");
  sensorManager.begin();
  
  DEBUG_PRINTLN("Init LED...");
  ledController.begin();
  
  sensorManager.readSensors();
  
  ledController.setThreshold("light", DEFAULT_LIGHT_THRESHOLD);
  ledController.enableThreshold("light");
  DEBUG_PRINT("Light threshold set to: ");
  DEBUG_PRINTLN(DEFAULT_LIGHT_THRESHOLD);
  ledController.checkAndApplyThreshold(sensorManager.getLightLevel(), "light");
  
  tft.println("WiFi...");
  DEBUG_PRINTLN("Connecting to WiFi...");
  
  if (wifiManager.begin(WIFI_SSID, WIFI_PASSWORD)) {
    DEBUG_PRINTLN("WiFi OK");
    DEBUG_PRINT("IP: ");
    DEBUG_PRINTLN(wifiManager.getIPAddress());
    
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("WiFi OK!");
    tft.setTextSize(2);
    tft.print("IP: ");
    tft.println(wifiManager.getIPAddress());
  } else {
    DEBUG_PRINTLN("WiFi connection failed");
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.println("WiFi FAILED!");
  }
  
  DEBUG_PRINTLN("Starting API server...");
  apiServer.begin(&sensorManager, &ledController);
  
  DEBUG_PRINT("API at: http://");
  DEBUG_PRINT(wifiManager.getIPAddress());
  DEBUG_PRINTLN("/api/status");
  
  DEBUG_PRINTLN("\nSystem ready!");
  delay(2000);
}

// Loop principal

void loop() {

  //  Gérer les boutons
  buttonManager.handleButtons();
  
  //  Gérer la connexion WiFi
  wifiManager.handleConnection();
  
  // Traiter les requêtes HTTP
  apiServer.handleClient();
  
  
  // Lire les capteurs périodiquement
  if (sensorManager.shouldRead()) {
    sensorManager.readSensors();
  // appliquer le seuil
    ledController.checkAndApplyThreshold(sensorManager.getTemperature(), "temperature");
    ledController.checkAndApplyThreshold(sensorManager.getLightLevel(), "light");
  }
  
  //Mettre à jour l'affichage selon le mode
  DisplayMode mode = buttonManager.getDisplayMode();
  if (mode == DISPLAY_WIFI_SCAN) {
    displayWiFiScan();
  } else {
    updateDisplay();
  }
  
  delay(100);
}
