#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <cstdint>
#include <WString.h>

class WiFiClient;

class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager();

    bool requestDataToDraw();
    uint8_t connectToWifi();
    void stopWiFi();
    int16_t signalStrength() const;
    String response() const;

private:
    int16_t mSignalStrength = -1000;
    String mResponse;

    bool sendRequest(WiFiClient &client);
};

#endif // NETWORKMANAGER_H