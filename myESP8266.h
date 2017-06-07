#ifndef myESP8266_h
#define myESP8266_h

// the #include statment and code go here...
#include <Stream.h>

class myESP8266
{
  public:
    myESP8266(Stream &ESP8266Serial);
    void flushESP8266();
    bool testESP8266();
    bool resetESP8266();
    bool setWifiMode(String mode);
    
  private:
    Stream *_thisSerial;
    bool serialRider(char *string);
};

#endif
