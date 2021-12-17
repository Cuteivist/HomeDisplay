#include <HTTPClient.h>

#include "credentials.h"

int wifi_signal = 0;

// ########################### HTTP ###########################
bool sendRequest(WiFiClient &client) {
    client.stop(); // close connection before sending a new request
    HTTPClient http;
    http.begin(client, "pc.lan", 8881, "/"); // TODO change to pi address later when server moved to PI
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Received data:");
        Serial.println(payload);
        client.stop();
        http.end();
        return true;
    } else {
        Serial.printf("connection failed, error: %s\n", http.errorToString(httpCode).c_str());
        client.stop();
        http.end();
        return false;
    }
    http.end();
    return true;
}

bool requestDataToDraw() {
    int8_t Attempts = 0;
    bool GotData = false;
    WiFiClient client; // wifi client object
    Serial.write("Trying to send request");
    while (GotData == false && Attempts <= 2) {
        GotData = sendRequest(client);
        Attempts++;
    }
}
// ###########################  WIFI ###########################
uint8_t startWiFi() {
    Serial.print("\r\nConnecting to: ");
    Serial.println(String(ssid));
    IPAddress dns(8, 8, 8, 8); // Google DNS
    WiFi.disconnect();
    WiFi.mode(WIFI_STA); // switch off AP
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(ssid, password);
    unsigned long start = millis();
    uint8_t connectionStatus;
    bool AttemptConnection = true;
    while (AttemptConnection) {
        connectionStatus = WiFi.status();
        if (millis() > start + 15000) { // Wait 15-secs maximum
            AttemptConnection = false;
        }
        if (connectionStatus == WL_CONNECTED || connectionStatus == WL_CONNECT_FAILED) {
            AttemptConnection = false;
        }
        delay(50);
    }
    if (connectionStatus == WL_CONNECTED) {
        wifi_signal = WiFi.RSSI(); // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
        Serial.println("WiFi connected at: " + WiFi.localIP().toString());
    } else
        Serial.println("WiFi connection *** FAILED ***");
    return connectionStatus;
}

void stopWiFi() {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
}