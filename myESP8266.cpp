#include "myESP8266.h"
#include <Stream.h>


myESP8266::myESP8266(Stream &ESP8266Serial, Stream &Debug)
{
  _thisSerial=&ESP8266Serial;
  _thisSerialDebug=&Debug;  
  //_thisSerial->println("ciao2");
  //const int NTP_PACKET_SIZE= 48;
  //byte* _packetBuffer[];
}

void myESP8266::setNTP_PACKET_SIZE(int NTP_PACKET_SIZE)
{
  this->NTP_PACKET_SIZE = NTP_PACKET_SIZE;
}
void myESP8266::setPacketBuffer(byte* packetBuffer)
{
  _packetBuffer = packetBuffer;
}
byte* myESP8266::getPacketBuffer()
{
  return _packetBuffer;
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
  
  char* statuses[] = {"OK","no change"};
  
  int numOfStatuses = sizeof(statuses)/sizeof(statuses[0]);

  if (evaluateSerialRiderLite(statuses, numOfStatuses))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::connectWiFi(String SSID, String PASS) 
{ 
  String cmd = "AT+CWJAP=\"";
  cmd += SSID;
  cmd += "\",\"";
  cmd += PASS;
  cmd += "\"\r\n";
  
  _thisSerial->println(cmd);
  
  if(serialRider("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::setMux(String mode)
{
  _thisSerial->println("AT+CIPMUX="+mode);
  //_thisSerial->print("AT+CIPMUX=");
  //_thisSerial->println(mode);
  
  if(serialRider("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::setBaudRate(String Baudrate)
{
  String cmd;
  cmd = "AT+CIOBAUD="+Baudrate;
  _thisSerial->println(cmd);
  
  if(serialRider("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::setConnection(String connType, String ipDest, String port)
{
  String cmd;
  cmd = "AT+CIPSTART=\"";
  cmd += connType;
  cmd += "\",\"";
  cmd += ipDest;
  cmd += "\",";
  cmd += port;
  
  _thisSerial->println(cmd);

  if(serialRider("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::closeConnection()
{
  String cmd;
  cmd = "AT+CIPCLOSE";
  
  _thisSerial->println(cmd);
  
  if (serialRider("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::setMultiConnection(String Chan, String connType, String ipDest, String port)
{
  //String cmd = "AT+CIPSTART=\"UDP\",\"193.204.114.232\",123"; //it works
  String cmd = "AT+CIPSTART=1,\"UDP\",\"193.204.114.232\",123"; //it works
//  String cmd;
//  cmd = "AT+CIPSTART=";
//  cmd += Chan;
//  cmd += ",\"";
//  cmd += connType;
//  cmd += "\",\"";
//  cmd += ipDest;
//  cmd += "\",\"";
//  cmd += port;
//  cmd += "\"";
//  String cmd = "AT+CIPSTART=\"1\",\"";
//  cmd += "UDP";
//  cmd += "\",\"";
//  cmd += "193.204.114.232";
//  cmd += "\",";
//  cmd += "\"123\"";  

  _thisSerial->println("AT+CIPSTART=1,\"UDP\",\"193.204.114.232\",123\r\n");
  _thisSerialDebug->println(cmd);

  if(serialRider("OK"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::closeMultiConnection(String Chan)
{
  String cmd;
  cmd = "AT+CIPCLOSE="+Chan;
  
  _thisSerial->println(cmd);
  _thisSerialDebug->println(cmd);
  
  if (serialRider1("Unlink"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

//boolean setServer()
//{
//  String cmd;
//  cmd += "AT+CIPSERVER=1,9999";
//  Serial1.println(cmd);
//  if (!Serial1.find("OK")) 
//  {
//    return false;
//  }
//  return true;
//}
//
//boolean stopServer()
//{
//  String cmd;
//  cmd += "AT+CIPSERVER=0";
//  Serial1.println(cmd);
//  if (!Serial1.find("OK")) 
//  {
//    return false;
//  }
//  return true;
//}
//

bool myESP8266::sendData(char *message)
{
  String cmd;
  cmd += "AT+CIPSEND=";
  cmd += sizeof(message)/sizeof(char);//(String)string.length();
  _thisSerialDebug->println(cmd);
  _thisSerial->println(cmd);
 
  if(serialRider(">"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool myESP8266::sendDataWithChannel(char *chan, byte message[])
{
  String cmd;
  cmd += "AT+CIPSEND=";
  cmd += chan;
  cmd += ",";
  cmd += "48";
  _thisSerialDebug->println(cmd);
  _thisSerial->println(cmd);
  if(serialRider1(">"))
  {
    _thisSerialDebug->println("> found");
    _thisSerialDebug->write(_packetBuffer,48);
    _thisSerial->write(_packetBuffer,48);
    if(serialRider1("SEND OK"))
    {
      delay(1000);
      if (serialRider1("+IPD,1,48:"))
      {
        
        return true;
      }
      //while(_thisSerial->available())
      //{
        //_thisSerialDebug->print(_thisSerial->read());
      //}
      return false;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}
//boolean sendData(String data)
//{
//  String cmd;
//  cmd += "AT+CIPSEND=";
//  //data += "\r\n";
//  cmd += (String)data.length();
//  Serial.println(cmd);
//  Serial.println(data);
//  Serial1.println(cmd);
//  if (!Serial1.find(">")) 
//  {
//    Serial.println("non invio...");
//    return false;
//  }
//  Serial.println("invio...");
//  Serial1.println(data);
//  return true;
//}
//
//String receiveData()
//{
//  String cmd;
//  
//  if (Serial1.available())
//  {
//    if (Serial1.find("+IPD"))
//    {
//      cmd = (String)Serial1.read();
//      int startMessage = cmd.indexOf('>',0)+1;
//      cmd = cmd.substring(startMessage,cmd.length());
//    }
//  }
//  return cmd;
//}

bool myESP8266::evaluateSerialRiderLite(char* strStatuses[], int numOfStatuses)
{
  bool CAA = true; //Comando Arlecchino Avviato
  bool foundIt = false;
 
  while(CAA)
  {
    char* response=serialRiderLite();
    for(int i=0; i<=numOfStatuses; i++)
    {
      if (strcmp(strStatuses[i], response)==0)
      {
        foundIt = true;
        CAA=false;
      }
      if (strcmp(strStatuses[i], "ERROR")==0 || strcmp(strStatuses[i], "FAIL")==0)
      {
        CAA=false;
      }
    }
  }
  return foundIt;
}

char* myESP8266::serialRiderLite()
{
  bool newData = false;
  
  int ndx = 0;
  char rc;
  char carriageReturn = '\r';
  char newLine = '\n';
  int numChars = 48;
  char receivedChars[numChars];   // an array to store the received data
  
  memset(receivedChars, 0, numChars);
  
  while (_thisSerial->available() > 0 && newData == false) 
  {
    rc = _thisSerial->read();
    _thisSerialDebug->println(rc);
    
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
  }
  return receivedChars;
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






bool myESP8266::serialRider1(char *string) //suggested by Marta, formerly serialReader
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
  char storedChars[numChars];   // an array to store the received data

  memset(receivedChars, 0, numChars);
  memset(storedChars, 0, numChars);

  while(CAA)
  {
    while (_thisSerial->available() > 0 && newData == false) 
    {
      //_thisSerialDebug->println("in newdata");
      rc = _thisSerial->read();
      _thisSerialDebug->print(rc);
      
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
          strcpy(storedChars,receivedChars);
          memset(receivedChars, 0, numChars);
      }
      if (newData == true)
      {
        //_thisSerialDebug->println(storedChars);
        if (strcmp(storedChars, string)==0 || strcmp(storedChars, "no change")==0)
        {
          foundIt = true;
          CAA=false;
        }
        if (strcmp(storedChars, "ERROR")==0 || strcmp(storedChars, "FAIL")==0 || strcmp(storedChars, "Link typ ERROR")==0)
        {
          CAA=false;
        }
      }
      //#if defined(_DEBUG_)==1
      //showNewData();
      //#endif
      newData = false;
    }
//    _thisSerialDebug->println(_thisSerial->available());
//    _thisSerialDebug->println(newData);
//    _thisSerialDebug->println(strcmp(storedChars, string));
//    _thisSerialDebug->println(storedChars);
//    _thisSerialDebug->println();
//    _thisSerialDebug->println();
//    _thisSerialDebug->println();
    if (_thisSerial->available() == 0 && newData == false && strcmp(receivedChars, string)==0)
    {
//      _thisSerialDebug->println(strcmp(receivedChars, string));
//      _thisSerialDebug->println(receivedChars);
      foundIt = true;
      CAA = false; 
    }
  }
  //_thisSerialDebug->println(receivedChars);
  return foundIt;
}

