#include "myESP8266.h"
#include <Stream.h>

myESP8266::myESP8266(Stream &ESP8266Serial)
{
  _thisSerial=&ESP8266Serial;
  //_thisSerial->println("ciao2");
}

void myESP8266::flushESP8266()
{
  while(_thisSerial->available())
  {
    _thisSerial->read();
  }
}

bool myESP8266::testESP8266()
{
  _thisSerial->println("AT");
  if(serialRider("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::resetESP8266()
{
  _thisSerial->println("AT+RST");
  if(serialRider("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::setWifiMode(String mode)
{
  _thisSerial->println("AT+CWMODE="+mode);
  
  if(serialRider("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::serialRider(char *string) //suggested by Marta, formerly serialReader
{
  bool foundIt = false;
  //static byte ndx = 0;
  int ndx = 0;
  char carriageReturn = '\r';
  char newLine = '\n';
  char rc;
  bool CAA = true; //Comando Arlecchino Avviato
  bool newData = false;
  int numChars = 48;
  char receivedChars[numChars];   // an array to store the received data
  
  memset(receivedChars, 0, numChars);
    
  while(CAA)
  {
    while (_thisSerial->available() > 0 && newData == false) 
    {
      rc = _thisSerial->read();
      
      if (rc != carriageReturn && rc != newLine)
      {
          receivedChars[ndx] = rc;
          ndx++;
          if (ndx >= numChars) 
          {
              ndx = numChars - 1;
          }
      }
      else if (rc == newLine)
      {
          receivedChars[ndx] = '\0'; // terminate the string
          ndx = 0;
          newData = true;
      }
      if (newData == true)
      {
        if (strcmp(receivedChars, string)==0 || strcmp(receivedChars, "no change")==0)
        {
          foundIt = true;
          CAA=false;
        }
        if (strcmp(receivedChars, "ERROR")==0 || strcmp(receivedChars, "FAIL")==0)
        {
          CAA=false;
        }
      }
      //#if defined(_DEBUG_)==1
      //showNewData();
      //#endif
      newData = false;
    }
  }
  return foundIt;
}

