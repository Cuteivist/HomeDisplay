#include "NetworkManager.h"

#include "BoardController.h"
#include <HTTPClient.h>
#include <WiFi.h>

#include "credentials.h"

NetworkManager::NetworkManager()
{

}

NetworkManager::~NetworkManager()
{
    stopWiFi();
}

bool NetworkManager::sendRequest(WiFiClient &client) 
{
    // close connection before sending a new request
    client.stop(); 
    HTTPClient http;
    http.begin(client, serverAddress, serverPort, "/");
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        BoardController::debug("Received data:");
        BoardController::debug(payload);
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

bool NetworkManager::requestDataToDraw() 
{
    int8_t Attempts = 0;
    bool GotData = false;
    WiFiClient client;
    BoardController::debug("Trying to send request");
    while (GotData == false && Attempts <= 2) {
        GotData = sendRequest(client);
        Attempts++;
    }
}

uint8_t NetworkManager::connectToWifi() 
{
    mSignalStrength = -1000;
    BoardController::debug("Connecting to: " + String(wifiSsid));
    IPAddress dns(8, 8, 8, 8); // Google DNS
    WiFi.disconnect();
    // switch off AP
    WiFi.mode(WIFI_STA); 
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(wifiSsid, wifiPassword);
    unsigned long start = millis();
    uint8_t connectionStatus;
    bool AttemptConnection = true;
    while (AttemptConnection) {
        connectionStatus = WiFi.status();
        // Wait 15-secs maximum
        if (millis() > start + 15000) { 
            AttemptConnection = false;
        }
        if (connectionStatus == WL_CONNECTED || connectionStatus == WL_CONNECT_FAILED) {
            AttemptConnection = false;
        }
        delay(50);
    }
    if (connectionStatus == WL_CONNECTED) {
        // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
        mSignalStrength = WiFi.RSSI(); 
        BoardController::debug("WiFi connected at: " + WiFi.localIP().toString());
    } else
        BoardController::debug("WiFi connection *** FAILED ***");
    return connectionStatus;
}

void NetworkManager::stopWiFi() 
{
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
}

int16_t NetworkManager::signalStrength() const
{
    return mSignalStrength;
}