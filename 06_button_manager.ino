// Gestionnaire de bouttons

class ButtonManager {
  private:
    unsigned long lastButton1Press;
    unsigned long lastButton2Press;
    DisplayMode currentDisplayMode;
    int scrollOffset;

  public:
    ButtonManager() 
      : lastButton1Press(0), 
        lastButton2Press(0), 
        currentDisplayMode(DISPLAY_NORMAL),
        scrollOffset(0) {}

    // Initialisation des boutons
     
    void begin() {
      pinMode(BUTTON_1, INPUT_PULLUP);
      pinMode(BUTTON_2, INPUT_PULLUP);
      
      DEBUG_PRINTLN("[ButtonManager] Buttons initialized");
      DEBUG_PRINT("[ButtonManager] Button 1 (WiFi scan) on pin: ");
      DEBUG_PRINTLN(BUTTON_1);
      DEBUG_PRINT("[ButtonManager] Button 2 (Normal display) on pin: ");
      DEBUG_PRINTLN(BUTTON_2);
    }

    // Vérifier l'état des boutons et gérer les actions
     
    void handleButtons() {
      unsigned long currentTime = millis();
      
      // Bouton 1 - Afficher/Défiler les réseaux WiFi
      if (digitalRead(BUTTON_1) == LOW && 
          (currentTime - lastButton1Press) > BUTTON_DEBOUNCE_DELAY) {
        lastButton1Press = currentTime;
        
        if (currentDisplayMode == DISPLAY_WIFI_SCAN) {
          // Déjà en mode scan : défiler vers le bas
          scrollOffset++;
          DEBUG_PRINT("[ButtonManager] Button 1 - Scrolling to offset: ");
          DEBUG_PRINTLN(scrollOffset);
        } else {
          // Passer en mode scan
          currentDisplayMode = DISPLAY_WIFI_SCAN;
          scrollOffset = 0;
          DEBUG_PRINTLN("[ButtonManager] Button 1 pressed - Switching to WiFi scan mode");
        }
        delay(200); // Anti-rebond supplémentaire
      }
      
      // Bouton 2 - Affichage normal
      if (digitalRead(BUTTON_2) == LOW && 
          (currentTime - lastButton2Press) > BUTTON_DEBOUNCE_DELAY) {
        lastButton2Press = currentTime;
        currentDisplayMode = DISPLAY_NORMAL;
        scrollOffset = 0; // Reset du défilement
        DEBUG_PRINTLN("[ButtonManager] Button 2 pressed - Switching to normal mode");
        delay(200); // Anti-rebond supplémentaire
      }
    }

    // Obtenir le mode d'affichage actuel
     
    DisplayMode getDisplayMode() {
      return currentDisplayMode;
    }

    // Définir le mode d'affichage
     
    void setDisplayMode(DisplayMode mode) {
      currentDisplayMode = mode;
    }

    // Obtenir l'offset de défilement actuel
     
    int getScrollOffset() {
      return scrollOffset;
    }

    // Réinitialiser l'offset de défilement
     
    void resetScrollOffset() {
      scrollOffset = 0;
    }
};

// Instance globale
ButtonManager buttonManager;
