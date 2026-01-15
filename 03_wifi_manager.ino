// Gestionnaire de wifi


class WiFiManager {
  private:
    bool isConnected;
    unsigned long lastConnectionAttempt;
    static const int MAX_CONNECTION_ATTEMPTS = 20;
    static const unsigned long CONNECTION_TIMEOUT = 10000;


  public:
    WiFiManager() : isConnected(false), lastConnectionAttempt(0) {}

    bool begin(const char* ssid, const char* password) {
      DEBUG_PRINTLN("[WiFiManager] Starting WiFi connection...");
      DEBUG_PRINT("[WiFiManager] SSID: ");
      DEBUG_PRINTLN(ssid);
      
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      
      int attempts = 0;
      unsigned long startTime = millis();
      
      while (WiFi.status() != WL_CONNECTED && 
             attempts < MAX_CONNECTION_ATTEMPTS &&
             (millis() - startTime) < CONNECTION_TIMEOUT) {
        delay(500);
        DEBUG_PRINT(".");
        attempts++;
      }
      DEBUG_PRINTLN();
      
      if (WiFi.status() == WL_CONNECTED) {
        isConnected = true;
        DEBUG_PRINTLN("[WiFiManager] WiFi connected!");
        DEBUG_PRINT("[WiFiManager] IP Address: ");
        DEBUG_PRINTLN(WiFi.localIP());
        return true;
      } else {
        isConnected = false;
        DEBUG_PRINTLN("[WiFiManager] Failed to connect to WiFi");
        return false;
      }
    }

    bool isWiFiConnected() {
      return WiFi.status() == WL_CONNECTED;
    }

    String getIPAddress() {
      return WiFi.localIP().toString();
    }

    void handleConnection() {
      if (!isWiFiConnected()) {
        unsigned long currentTime = millis();
        if (currentTime - lastConnectionAttempt > 10000) {
          DEBUG_PRINTLN("[WiFiManager] Attempting to reconnect...");
          WiFi.begin();
          lastConnectionAttempt = currentTime;
          
        }
      } else {
        isConnected = true;
      }
    }

    void disconnect() {
      WiFi.disconnect(true);
      isConnected = false;
      DEBUG_PRINTLN("[WiFiManager] WiFi disconnected");
    }
};

// Instance globale
WiFiManager wifiManager;
