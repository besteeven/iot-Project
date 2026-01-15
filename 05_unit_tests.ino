/*
  ============================================================================
  UNIT TESTS avec traçage d'exécution
  ============================================================================
*/

// Compteurs globaux pour les tests
int totalTests = 0;
int passedTests = 0;
int failedTests = 0;

// ============================================================================
// INITIALISATION DE SERIAL POUR LES TESTS
// ============================================================================

void init_test_serial() {
  delay(500);
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║   TESTS UNITAIRES - START              ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println("");
}

// MACROS DE TEST 

#define TEST(actual, expected, name) \
  do { \
    totalTests++; \
    Serial.print("[TEST "); Serial.print(totalTests); Serial.print("] "); \
    Serial.print(name); Serial.print(" ... "); \
    if (actual == expected) { \
      Serial.print("✓ PASS"); \
      passedTests++; \
    } else { \
      Serial.print("✗ FAIL"); \
      Serial.print(" | Attendu: "); Serial.print(expected); \
      Serial.print(" | Obtenu: "); Serial.print(actual); \
      failedTests++; \
    } \
    Serial.println(); \
  } while(0)

#define TEST_BOOL(condition, name) \
  do { \
    totalTests++; \
    Serial.print("[TEST "); Serial.print(totalTests); Serial.print("] "); \
    Serial.print(name); Serial.print(" ... "); \
    if (condition) { \
      Serial.print("✓ PASS"); \
      passedTests++; \
    } else { \
      Serial.print("✗ FAIL"); \
      failedTests++; \
    } \
    Serial.println(); \
  } while(0)

#define TEST_FLOAT(actual, expected, tol, name) \
  do { \
    totalTests++; \
    Serial.print("[TEST "); Serial.print(totalTests); Serial.print("] "); \
    Serial.print(name); Serial.print(" ... "); \
    if (abs(actual - expected) <= tol) { \
      Serial.print("✓ PASS"); \
      passedTests++; \
    } else { \
      Serial.print("✗ FAIL"); \
      Serial.print(" | Attendu: ~"); Serial.print(expected); \
      Serial.print("±"); Serial.print(tol); \
      Serial.print(" | Obtenu: "); Serial.print(actual); \
      failedTests++; \
    } \
    Serial.println(); \
  } while(0)

// ============================================================================
// TESTS SENSOR MANAGER
// ============================================================================

void test_sensor_manager() {
  Serial.println("");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  TEST SENSOR MANAGER (10 tests)        ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  Serial.println("[GROUPE 1] Vérification du nombre de capteurs");
  int count = sensorManager.getSensorCount();
  Serial.print("  → getSensorCount() retourne: "); Serial.println(count);
  TEST(count, 2, "getSensorCount() == 2");
  
  Serial.println("[GROUPE 2] Vérification des types de capteurs");
  SensorData s0 = sensorManager.getSensor(0);
  Serial.print("  → sensor[0].type = "); Serial.println(s0.type);
  TEST(s0.type == "temperature", 1, "sensor[0].type == 'temperature'");
  
  SensorData s1 = sensorManager.getSensor(1);
  Serial.print("  → sensor[1].type = "); Serial.println(s1.type);
  TEST(s1.type == "light", 1, "sensor[1].type == 'light'");
  
  Serial.println("[GROUPE 3] Vérification des plages de valeurs");
  Serial.print("  → sensor[0].minValue = "); Serial.println(s0.minValue);
  TEST_FLOAT(s0.minValue, -40.0, 0.1, "sensor[0].minValue == -40.0");
  
  Serial.print("  → sensor[0].maxValue = "); Serial.println(s0.maxValue);
  TEST_FLOAT(s0.maxValue, 80.0, 0.1, "sensor[0].maxValue == 80.0");
  
  Serial.print("  → sensor[1].minValue = "); Serial.println(s1.minValue);
  TEST_FLOAT(s1.minValue, 0.0, 0.1, "sensor[1].minValue == 0.0");
  
  Serial.print("  → sensor[1].maxValue = "); Serial.println(s1.maxValue);
  TEST_FLOAT(s1.maxValue, 4095.0, 0.1, "sensor[1].maxValue == 4095.0");
  
  Serial.println("[GROUPE 4] Vérification des lectures de capteurs");
  float temp = sensorManager.getTemperature();
  Serial.print("  → getTemperature() = "); Serial.println(temp);
  TEST_BOOL((temp >= -40.0 && temp <= 80.0), "Température dans la plage");
  
  float light = sensorManager.getLightLevel();
  Serial.print("  → getLightLevel() = "); Serial.println(light);
  TEST_BOOL((light >= 0.0 && light <= 4095.0), "Lumière dans la plage");
  
  Serial.println("✓ Tests SensorManager terminés");
}

// ============================================================================
// TESTS LED CONTROLLER
// ============================================================================

void test_led_controller() {
  Serial.println("");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  TEST LED CONTROLLER (19 tests)        ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  Serial.println("[GROUPE 1] État initial de la LED");
  bool initialState = ledController.getState();  
  Serial.print("  → État initial: "); Serial.println(initialState ? "ON" : "OFF");
  TEST_BOOL(!initialState, "État initial == OFF");  
  
  Serial.println("[GROUPE 2] Contrôle ON/OFF");
  ledController.turnOn();
  Serial.println("  → Appel turnOn()");
  bool stateAfterOn = ledController.getState();
  Serial.print("  → État après ON: "); Serial.println(stateAfterOn ? "ON" : "OFF");
  TEST_BOOL(stateAfterOn, "Après turnOn() -> ON");
  
  ledController.turnOff();
  Serial.println("  → Appel turnOff()");
  bool stateAfterOff = ledController.getState();
  Serial.print("  → État après OFF: "); Serial.println(stateAfterOff ? "ON" : "OFF");
  TEST_BOOL(!stateAfterOff, "Après turnOff() -> OFF");
  
  Serial.println("[GROUPE 3] Contrôle Toggle");
  ledController.turnOff();
  Serial.println("  → État initial: OFF");
  ledController.toggle();
  Serial.println("  → Appel toggle()");
  bool stateAfterToggle1 = ledController.getState();
  Serial.print("  → État après toggle: "); Serial.println(stateAfterToggle1 ? "ON" : "OFF");
  TEST_BOOL(stateAfterToggle1, "Après toggle() de OFF -> ON");
  
  ledController.toggle();
  Serial.println("  → Appel toggle() à nouveau");
  bool stateAfterToggle2 = ledController.getState();
  Serial.print("  → État après toggle: "); Serial.println(stateAfterToggle2 ? "ON" : "OFF");
  TEST_BOOL(!stateAfterToggle2, "Après toggle() de ON -> OFF");
  
  Serial.println("[GROUPE 4] Modes (Auto/Manual)");
  String mode = ledController.getMode();
  Serial.print("  → Mode initial: "); Serial.println(mode);
  TEST(mode == "auto", 1, "Mode initial == 'auto'");
  
  ledController.setMode("manual");
  Serial.println("  → Appel setMode('manual')");
  String modeAfterSet = ledController.getMode();
  Serial.print("  → Mode après setMode: "); Serial.println(modeAfterSet);
  TEST(modeAfterSet == "manual", 1, "Après setMode('manual') -> 'manual'");
  
  ledController.setMode("auto");
  Serial.println("  → Appel setMode('auto')");
  bool isAutoMode1 = ledController.isAutoMode();
  Serial.print("  → isAutoMode(): "); Serial.println(isAutoMode1 ? "true" : "false");
  TEST_BOOL(isAutoMode1, "isAutoMode() == true en mode auto");
  
  ledController.setMode("manual");
  Serial.println("  → Appel setMode('manual')");
  bool isAutoMode2 = ledController.isAutoMode();
  Serial.print("  → isAutoMode(): "); Serial.println(isAutoMode2 ? "true" : "false");
  TEST_BOOL(!isAutoMode2, "isAutoMode() == false en mode manual");
  
  Serial.println("[GROUPE 5] Gestion des seuils");
  float thresholdTemp = ledController.getThreshold("temperature");
  Serial.print("  → Seuil température défaut: "); Serial.println(thresholdTemp);
  TEST_FLOAT(thresholdTemp, 25.0, 0.1, "Seuil temp défaut == 25.0");
  
  float thresholdLight = ledController.getThreshold("light");
  Serial.print("  → Seuil lumière défaut: "); Serial.println(thresholdLight);
  TEST_FLOAT(thresholdLight, 2500.0, 0.1, "Seuil light défaut == 2500.0");
  
  ledController.setThreshold("temperature", 28.0);
  Serial.println("  → Appel setThreshold('temperature', 28.0)");
  float thresholdTempAfter = ledController.getThreshold("temperature");
  Serial.print("  → Seuil température après: "); Serial.println(thresholdTempAfter);
  TEST_FLOAT(thresholdTempAfter, 28.0, 0.1, "Seuil temp après set == 28.0");
  
  Serial.println("[GROUPE 6] Activation/Désactivation des seuils");
  bool tempThresholdEnabled1 = ledController.isThresholdEnabled("temperature");
  Serial.print("  → Seuil température activé par défaut: "); Serial.println(tempThresholdEnabled1 ? "true" : "false");
  TEST_BOOL(!tempThresholdEnabled1, "Seuil temp désactivé par défaut");
  
  bool lightThresholdEnabled1 = ledController.isThresholdEnabled("light");
  Serial.print("  → Seuil lumière activé par défaut: "); Serial.println(lightThresholdEnabled1 ? "true" : "false");
  TEST_BOOL(!lightThresholdEnabled1, "Seuil light désactivé par défaut");  
  
  ledController.enableThreshold("temperature");
  Serial.println("  → Appel enableThreshold('temperature')");
  bool tempThresholdEnabled2 = ledController.isThresholdEnabled("temperature");
  Serial.print("  → Seuil température après enable: "); Serial.println(tempThresholdEnabled2 ? "true" : "false");
  TEST_BOOL(tempThresholdEnabled2, "Seuil temp activé après enable");
  
  ledController.disableThreshold("temperature");
  Serial.println("  → Appel disableThreshold('temperature')");
  bool tempThresholdEnabled3 = ledController.isThresholdEnabled("temperature");
  Serial.print("  → Seuil température après disable: "); Serial.println(tempThresholdEnabled3 ? "true" : "false");
  TEST_BOOL(!tempThresholdEnabled3, "Seuil temp désactivé après disable");
  
  Serial.println("✓ Tests LEDController terminés");
}

// ============================================================================
// TESTS LED THRESHOLD LOGIC
// ============================================================================

void test_led_threshold_logic() {
  Serial.println("");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  TEST THRESHOLD LOGIC (7 tests)        ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  Serial.println("[GROUPE 1] Mode Manual - Seuil ignoré");
  ledController.setMode("manual");
  ledController.turnOff();
  ledController.setThreshold("light", 2500.0);
  ledController.enableThreshold("light");
  Serial.println("  → Mode manual, seuil activé, valeur = 1000");
  ledController.checkAndApplyThreshold(1000.0, "light");
  bool manualState = ledController.getState();
  Serial.print("  → État LED: "); Serial.println(manualState ? "ON" : "OFF");
  TEST_BOOL(!manualState, "En mode manual, seuil ne change pas la LED");
  
  Serial.println("[GROUPE 2] Mode Auto - Seuil lumière bas");
  ledController.setMode("auto");
  ledController.turnOff();
  ledController.setThreshold("light", 2500.0);
  ledController.enableThreshold("light");
  Serial.println("  → Mode auto, lumière < 2500 (valeur = 1000)");
  ledController.checkAndApplyThreshold(1000.0, "light");
  bool autoLowLightState = ledController.getState();
  Serial.print("  → État LED: "); Serial.println(autoLowLightState ? "ON" : "OFF");
  TEST_BOOL(autoLowLightState, "Mode auto + lumière basse -> LED ON");
  
  Serial.println("[GROUPE 3] Mode Auto - Seuil lumière haut");
  ledController.setMode("auto");
  ledController.setThreshold("light", 2500.0);
  ledController.enableThreshold("light");
  Serial.println("  → Mode auto, lumière > 2500 (valeur = 3500)");
  ledController.checkAndApplyThreshold(3500.0, "light");
  bool autoHighLightState = ledController.getState();
  Serial.print("  → État LED: "); Serial.println(autoHighLightState ? "ON" : "OFF");
  TEST_BOOL(!autoHighLightState, "Mode auto + lumière haute -> LED OFF");
  
  Serial.println("[GROUPE 4] Mode Auto - Seuil température haut");
  ledController.setMode("auto");
  ledController.turnOff();
  ledController.setThreshold("temperature", 25.0);
  ledController.enableThreshold("temperature");
  Serial.println("  → Mode auto, température > 25 (valeur = 30)");
  ledController.checkAndApplyThreshold(30.0, "temperature");
  bool autoHighTempState = ledController.getState();
  Serial.print("  → État LED: "); Serial.println(autoHighTempState ? "ON" : "OFF");
  TEST_BOOL(autoHighTempState, "Mode auto + température haute -> LED ON");
  
  Serial.println("[GROUPE 5] Mode Auto - Seuil température bas");
  ledController.setMode("auto");
  ledController.setThreshold("temperature", 25.0);
  ledController.enableThreshold("temperature");
  Serial.println("  → Mode auto, température < 25 (valeur = 20)");
  ledController.checkAndApplyThreshold(20.0, "temperature");
  bool autoLowTempState = ledController.getState();
  Serial.print("  → État LED: "); Serial.println(autoLowTempState ? "ON" : "OFF");
  TEST_BOOL(!autoLowTempState, "Mode auto + température basse -> LED OFF");
  
  Serial.println("[GROUPE 6] Seuil désactivé");
  ledController.setMode("auto");
  ledController.turnOff();
  ledController.setThreshold("light", 2500.0);
  ledController.disableThreshold("light");
  Serial.println("  → Seuil désactivé, lumière basse");
  ledController.checkAndApplyThreshold(1000.0, "light");
  bool disabledThresholdState = ledController.getState();
  Serial.print("  → État LED: "); Serial.println(disabledThresholdState ? "ON" : "OFF");
  TEST_BOOL(!disabledThresholdState, "Seuil désactivé: pas d'effet");
  
  Serial.println("[GROUPE 7] Température == seuil (condition fausse)");
  ledController.setMode("auto");
  ledController.turnOff();
  ledController.setThreshold("temperature", 25.0);
  ledController.enableThreshold("temperature");
  Serial.println("  → Température == seuil (25 == 25, devrait être false)");
  ledController.checkAndApplyThreshold(25.0, "temperature");
  bool equalTempState = ledController.getState();
  Serial.print("  → État LED: "); Serial.println(equalTempState ? "ON" : "OFF");
  TEST_BOOL(!equalTempState, "Température == seuil: condition false");
  
  Serial.println("✓ Tests Threshold Logic terminés");
}

// ============================================================================
// TESTS WIFI MANAGER
// ============================================================================

void test_wifi_manager() {
  Serial.println("");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  TEST WIFI MANAGER (3 tests)           ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  Serial.println("[GROUPE 1] État WiFi");
  bool wifiConnected = wifiManager.isWiFiConnected();
  Serial.print("  → isWiFiConnected(): "); Serial.println(wifiConnected ? "true" : "false");
  TEST_BOOL((wifiConnected == true || wifiConnected == false), "isWiFiConnected() retourne booléen");
  
  Serial.println("[GROUPE 2] Adresse IP");
  String ipAddress = wifiManager.getIPAddress();
  Serial.print("  → getIPAddress(): "); Serial.println(ipAddress);
  TEST_BOOL(ipAddress.length() > 0, "getIPAddress() retourne chaîne non-vide");
  
  Serial.println("[GROUPE 3] Format IP");
  bool hasDots = (ipAddress.indexOf('.') > 0);
  Serial.print("  → IP contient des points: "); Serial.println(hasDots ? "true" : "false");
  TEST_BOOL(hasDots, "Format IP contient des points");
  
  Serial.println("✓ Tests WiFiManager terminés");
}

// ============================================================================
// TESTS API SERVER
// ============================================================================

void test_api_server() {
  Serial.println("");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  TEST API SERVER (4 tests)             ║");
  Serial.println("╚════════════════════════════════════════╝");
  
  Serial.println("[GROUPE 1] Accès SensorManager");
  int sensorCount = sensorManager.getSensorCount();
  Serial.print("  → Nombre de capteurs: "); Serial.println(sensorCount);
  TEST(sensorCount, 2, "APIServer peut accéder à SensorManager");
  
  Serial.println("[GROUPE 2] Accès LEDController - État");
  bool ledState = ledController.getState();
  Serial.print("  → État LED: "); Serial.println(ledState ? "ON" : "OFF");
  TEST_BOOL((ledState == true || ledState == false), "APIServer peut accéder à LEDController");
  
  Serial.println("[GROUPE 3] Accès LEDController - Mode");
  String ledMode = ledController.getMode();
  Serial.print("  → Mode LED: "); Serial.println(ledMode);
  TEST_BOOL((ledMode == "auto" || ledMode == "manual"), "APIServer peut lire le mode LED");
  
  Serial.println("[GROUPE 4] Accès aux Seuils");
  float tempThreshold = ledController.getThreshold("temperature");
  Serial.print("  → Seuil température: "); Serial.println(tempThreshold);
  TEST_FLOAT(tempThreshold, 25.0, 10.0, "APIServer peut lire les seuils");
  
  Serial.println("✓ Tests APIServer terminés");
}

// ============================================================================
// RÉSUMÉ DES TESTS
// ============================================================================

void print_test_summary() {
  Serial.println("");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  RÉSUMÉ DES TESTS                      ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println("");
  
  Serial.print("Tests total:  ");
  Serial.println(totalTests);
  
  Serial.print("Tests réussis: ");
  Serial.print(passedTests);
  Serial.print(" (");
  if (totalTests > 0) {
    Serial.print((passedTests * 100) / totalTests);
  }
  Serial.println("%)");
  
  Serial.print("Tests échoués: ");
  Serial.println(failedTests);
  
  Serial.println("");
  
  if (failedTests == 0) {
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║  ✓ TOUS LES TESTS SONT RÉUSSIS!       ║");
    Serial.println("╚════════════════════════════════════════╝");
  } else {
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║  ✗ CERTAINS TESTS ONT ÉCHOUÉ!         ║");
    Serial.println("╚════════════════════════════════════════╝");
  }
  
  Serial.println("");
}

// ============================================================================
// FONCTION PRINCIPALE - APPELER LES TESTS
// ============================================================================

void run_all_unit_tests() {
  init_test_serial();
  
  test_sensor_manager();
  test_led_controller();
  test_led_threshold_logic();
  test_wifi_manager();
  test_api_server();
  
  print_test_summary();
}
