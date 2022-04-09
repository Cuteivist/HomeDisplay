#include "NetworkManager.h"

#include "Logger.h"
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
    mResponse.clear();
    // close connection before sending a new request
    client.stop(); 
    HTTPClient http;
    http.begin(client, serverAddress, serverPort, "/");
    DEBUG("Trying to send request: " + String(serverAddress) + ":" + String(serverPort));
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        mResponse = http.getString();
        DEBUG("Received data:");
        DEBUG(mResponse);
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
    int8_t attemptCount = 0;
    bool gotData = false;
    WiFiClient client;
    while (gotData == false && attemptCount <= 2) {
        gotData = sendRequest(client);
        attemptCount++;
    }
}

uint8_t NetworkManager::connectToWifi() 
{
    mSignalStrength = -1000;
    DEBUG("Connecting to: " + String(wifiSsid));
    IPAddress dns(8, 8, 8, 8); // Google DNS
    WiFi.disconnect();
    // switch off AP
    WiFi.mode(WIFI_STA); 
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.begin(wifiSsid, wifiPassword);
    const unsigned long startTime = millis();
    uint8_t connectionStatus;
    bool attemptConnection = true;
    while (attemptConnection) {
        connectionStatus = WiFi.status();
        // Wait 15-secs maximum
        if (millis() > startTime + 15000) { 
            attemptConnection = false;
        }
        if (connectionStatus == WL_CONNECTED || connectionStatus == WL_CONNECT_FAILED) {
            attemptConnection = false;
        }
        delay(50);
    }
    if (connectionStatus == WL_CONNECTED) {
        // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
        mSignalStrength = WiFi.RSSI(); 
        DEBUG("WiFi connected with IP: " + WiFi.localIP().toString());
    } else {
        DEBUG("WiFi connection *** FAILED ***");
    }
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

String NetworkManager::response() const
{
    return mResponse;
}