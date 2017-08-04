#ifndef myESP8266_h
#define myESP8266_h

// the #include statment and code go here...
#include <Stream.h>
#include <Arduino.h>

class myESP8266
{
  public:
    myESP8266(Stream &ESP8266Serial, Stream &Debug);
    int NTP_PACKET_SIZE;
    byte* _packetBuffer;
    void setNTP_PACKET_SIZE(int NTP_PACKET_SIZE);
    void setPacketBuffer(byte* packetBuffer);
    void flushESP8266();
    bool testESP8266();
    bool resetESP8266();
    bool setWifiMode(String mode);
    bool connectWiFi(String SSID, String PASS);
    bool setMux(String mode);
    bool setBaudRate(String Baudrate);
    bool setConnection(String connType, String ipDest, String port);
    bool closeConnection();
    bool setMultiConnection(String Chan, String connType, String ipDest, String port);
    bool closeMultiConnection(String Chan);
    bool sendData(char *message);
    bool sendDataWithChannel(char *Chan, byte message[]);
    
  private:
    Stream *_thisSerial;
    bool serialRider(char *string);
    bool evaluateSerialRiderLite(char* strStatuses[], int numOfStatuses);
    char* serialRiderLite();
    
    Stream *_thisSerialDebug;
    bool serialRider1(char *string);

};

#endif
