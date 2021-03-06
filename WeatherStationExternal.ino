#include "myESP8266.h"

#include <Wire.h>
#include <core_build_options.h>

#include <LowPower.h>

//Libreria Secondo sensore di temperatura
#include <OneWire.h>

//Libreria DHT11 Umidità e temperatura
#include <dht11.h>

// Sensore di pressione
#include <Adafruit_Sensor.h>
#define ADDRESS_SENSOR 0x77                 // Sensor address

/* Variabili per BMP180 */
int16_t  ac1, ac2, ac3, b1, b2, mb, mc, md; // Store sensor PROM values from BMP180
uint16_t ac4, ac5, ac6; // Store sensor PROM values from BMP180

/* Costanti per gesire la precisione e il risparmio energetico del sensore di umidità */
// Ultra Low Power       OSS = 0, OSD =  5ms
// Standard              OSS = 1, OSD =  8ms
// High                  OSS = 2, OSD = 14ms
// Ultra High Resolution OSS = 3, OSD = 26ms
const uint8_t oss = 3;                      // Set oversampling setting
const uint8_t osd = 26;                     // with corresponding oversampling delay 

float P;                                 // Set global variables for temperature and pressure 

typedef struct
{
  char chrTemp[10];
  String strTemp;
  float temp;
} DS18S20;

typedef struct
{
  float fvolt;
} VOLTAGE;

typedef struct
{
  String mbar;
  String mmHg;
} BMP180;

DS18S20 ds18s20;
VOLTAGE voltage;
BMP180 bmp180;

long previousMillis = 0;

long timeToSend = 10000;
long timeToReceive = 9000;

dht11 DHT_m;
#define DHT11_PIN 4

// WiFi Const
//Marco's home (it)
//#define SSID "Maxwell" // insert your SSID
//#define PASS "thundertr" // insert your password
//Marco's home (uk)
//#define SSID "BTHub5-CKRP" // insert your SSID
//#define PASS "5498d7a573" // insert your password
//Marta's home (uk)
//#define SSID "PLUSNET-8QGTP8" // insert your SSID
//#define PASS "c63ea3bca6" // insert your password
//Our home (uk)
//#define SSID "TALKTALK2311E1" // insert your SSID
//#define PASS "FQKNUFF6" // insert your password
#define SSID "Maxwell" // insert your SSID
#define PASS "thundertr" // insert your password
//android phone
//#define SSID "AndroidAP" // insert your SSID
//#define PASS "thundertr" // insert your password


#define RST 9
#define ESP8266LowPower 10
#define LED 13
#define CONNECT_ATTEMPTS 100
#define DEFINED_IP "192.168.0.15"
boolean connected = false;

// Testare
#define DS18S20_PIN 12
OneWire ds(DS18S20_PIN);

// costanti per NTP
unsigned long epoch;
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

//ESP8266 esp8266(Serial1);


// Example 2 - Receive with an end-marker
const byte numChars = 48;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

// DEBUG
#define _DEBUG_ 0

// ESP8266 instance
myESP8266 esp8266(Serial1, Serial);

//--------------
void setup()
{
  // Serial output per debug
  Serial.begin(9600);
  // Serial output per trasmissione WiFi
  Serial1.begin(9600);
  
  LowPower.powerExtStandby(SLEEP_30MS, ADC_OFF, BOD_OFF, TIMER2_ON);
  //LowPower.powerSave(SLEEP_FOREVER, ADC_OFF, BOD_OFF, TIMER2_ON);
  //LowPower.adcNoiseReduction(SLEEP_FOREVER, ADC_OFF, BOD_OFF, TIMER2_ON);

  globalSettings();
  
  // delay necessario per far si che la wifi sia pronta davvero
  delay(5000);
  
  /*while(Serial1.available())
  {
    Serial.write(Serial1.read());
  }*/
  
  //flush whatever can be in the buffer of the WiFi module
  esp8266.flushESP8266();
  Serial.println("flushing...");
  
  delay(5000);

  while(!esp8266.testESP8266())
  {
    Serial.println("test done, not passed");
    // ON 1 sec, OFF 5 sec, restart
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
    delay(5000);
  }
  
  Serial.println("test done, passed");
  
  while(!esp8266.setWifiMode("1"))
  {
    Serial.println("set wifi, not passed");
    delay(2000);
  }
  
  Serial.println("set wifi, passed");
  
  delay(5000);
  
//  for (int i = 0; i < CONNECT_ATTEMPTS; i++) 
//  {
//    if (connectWiFi())
//    {
//      connected = true;
//      Serial.println("connected!");
//      for (int j = 0; j<10; j++)
//      {
//        digitalWrite(LED, HIGH);
//        delay(50);
//        digitalWrite(LED, LOW);
//        delay(50);
//      }
//      // Serial output per trasmissione WiFi
//      break;
//    }
//    else
//    {
//      Serial.println("not connected!");
//    }
//    delay(2000);
//  }

  esp8266.flushESP8266();
  while(!esp8266.setMux("1"))
  {
    delay(2000);
  }
  Serial.println("setMulti ok");
  delay(2000);
  
  esp8266.flushESP8266();
  while(!esp8266.connectWiFi(SSID, PASS))
  {
    Serial.println("failed...");
    delay(5000);
  }

  connected = true;
  Serial.println("connected!");
  for (int j = 0; j<10; j++)
  {
    digitalWrite(LED, HIGH);
    delay(50);
    digitalWrite(LED, LOW);
    delay(50);
  }
}


//bool setWifiMode(String mode)
//{
//  Serial.println("setting wifi");
//  Serial.println("AT+CWMODE="+mode);
//  Serial1.println("AT+CWMODE="+mode);
//  
//  char* statuses[] = {"OK","no change"};
//  
//  int numOfStatuses = sizeof(statuses)/sizeof(statuses[0]);
//  Serial.println(numOfStatuses);
//  delay(1000);
//  
//  if (evaluateSerialRiderLite(statuses))
//  {
//    return true;
//  }
//  else
//  {
//    return false;
//  }
//}
//bool evaluateSerialRiderLite(char* strStatuses[])
//{
//  bool CAA = true; //Comando Arlecchino Avviato
//  bool foundIt = false;
//  
//  int numOfStatuses = sizeof(strStatuses)/sizeof(strStatuses[0]);
//  
//  while(CAA)
//  {
//    char* response=serialRiderLite();
//    for(int i=0; i<=numOfStatuses; i++)
//    {
//      if (strcmp(strStatuses[i], response)==0)
//      {
//        foundIt = true;
//        CAA=false;
//      }
//      if (strcmp(strStatuses[i], "ERROR")==0 || strcmp(strStatuses[i], "FAIL")==0)
//      {
//        CAA=false;
//      }
//    }
//  }
//  return foundIt;
//}
//char* serialRiderLite()
//{
//  bool newData = false;
//  
//  int ndx = 0;
//  char rc;
//  char carriageReturn = '\r';
//  char newLine = '\n';
//  int numChars = 48;
//  char receivedChars[numChars];   // an array to store the received data
//  
//  memset(receivedChars, 0, numChars);
//  
//  while (Serial1.available() > 0 && newData == false) 
//  {
//    rc = Serial1.read();
//    
//    if (rc != carriageReturn && rc != newLine)
//    {
//        receivedChars[ndx] = rc;
//        ndx++;
//        if (ndx >= numChars) 
//        {
//            ndx = numChars - 1;
//        }
//    }
//    else if (rc == newLine)
//    {
//        receivedChars[ndx] = '\0'; // terminate the string
//        ndx = 0;
//        newData = true;
//    }
//  }
//  return receivedChars;
//}

                                        
//boolean setBaudRate()
//{
//  String cmd;
//  cmd = "AT+CIOBAUD=9600";
//  Serial1.println(cmd);
//  Serial.write(Serial1.read());
//  if(!Serial1.find("OK"))
//  {
//    Serial.println("baudrate ko...");
//    return false;
//  }
//  Serial.println("baudrate ok...");
//  return true;
//}

//void loop()
//{
//  memset(packetBuffer, 0, NTP_PACKET_SIZE);
//  
//  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
//  packetBuffer[1] = 0;     // Stratum, or type of clock
//  packetBuffer[2] = 6;     // Polling Interval
//  packetBuffer[3] = 0xEC;  // Peer Clock Precision
//  // 8 bytes of zero for Root Delay & Root Dispersion
//  packetBuffer[4] = 0x31; 
//  packetBuffer[5] = 0x4E; 
//  packetBuffer[6] = 0x31;
//  packetBuffer[7] = 0x34;
//  packetBuffer[12]  = 49; 
//  packetBuffer[13]  = 0x4E;
//  packetBuffer[14]  = 49;
//  packetBuffer[15]  = 52;
//  
//  String cmd = "AT+CIPSTART=1,\"";
//  cmd += "UDP";
//  cmd += "\",\"";
//  //cmd += "193.204.114.233";
//  //cmd += "193.204.114.232";
//  //cmd += "46.101.52.119";
//  cmd += "216.239.35.8";
//  cmd += "\",\"";
//  cmd += "123";  
//  cmd += "\"";
//  
//  Serial1.println(cmd);
//  Serial.println(cmd);
//  if(!serialRider("OK","","",""))
//  {
//    Serial.println("Unable to connect to NTP server");
//    Serial1.println("AT+CIPCLOSE=1");
//    serialRider("OK","","","");
//    return;
//  }
//  
//  Serial.println("AT+CIPSEND=1,48");
//  Serial1.println("AT+CIPSEND=1,48");
//  if(!serialRider(">","","",""))
//  {
//    Serial.println("Prompt missed.");
//    Serial1.println("AT+CIPCLOSE=1");
//    serialRider("OK","","","");
//    return;
//  }
//  //delay(1000);
//  Serial.println("Sending package...");
//  //Serial1.write(packetBuffer,NTP_PACKET_SIZE);
//  //Serial1.println("aaaaaaaa");
//  //Serial1.write("");
//    for (int i = 0; i < NTP_PACKET_SIZE; i++)
//    {
//      Serial.print(i);
//      Serial.print(" ");
//      Serial.print(packetBuffer[i]);
//      Serial.print(" ");
//      Serial.println((uint8_t)packetBuffer[i]);
//      Serial1.write(packetBuffer[i]);
//      //Serial1.print(packetBuffer[i]);
//      delay(5);
//    }
////    while(Serial1.available())
////    {
////      Serial.write(Serial1.read());
////    }
//  if (!waitforLine("+IPD", 3000)) 
//  {
//    Serial.println("NTP Server did not respond1");
//  }
//  
//  delay(2000);
//  Serial.println("AT+CIPCLOSE=1");
//  Serial1.println("AT+CIPCLOSE=1");
//  serialRider("OK","","","");
//  
//  
//  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
//  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
//  // combine the four bytes (two words) into a long integer
//  // this is NTP time (seconds since Jan 1 1900):
//  unsigned long secsSince1900 = highWord << 16 | lowWord;
//  //Serial.print("Seconds since Jan 1 1900 = " );
//  //Serial.println(secsSince1900);
//
//  // now convert NTP time into everyday time:
//  //Serial.print("Unix time = ");
//  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
//  const unsigned long seventyYears = 2208988800UL;
//  // subtract seventy years:
//  epoch = secsSince1900 - seventyYears;
//  // print Unix time:
//  //Serial.println(epoch);
//  
//  Serial.println(epoch);
//  delay(10000);
//}


void loop()
{
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis < 0) // check for millis() rollover
  {
    // set correct previousMillis if millis() rolls over
    previousMillis = 4294967295 - previousMillis + currentMillis;
  }
  
  // check for engines to start
  if (currentMillis - previousMillis >= timeToSend)
  {
    previousMillis = currentMillis;
    
    ntpSync();
    
    TempEngine();
    
    HumTempEngine();
    
    checkPressure();
    
    checkBatteryLevel();
    
    ds18s20.strTemp = dtostrf(ds18s20.temp, 7, 3, ds18s20.chrTemp);

    Serial.print("Temperatura misurata DS18S20	: " + ds18s20.strTemp + " ");
    Serial.write(176);
    Serial.println("C");
    
    Serial.print("Pressure: ");
    Serial.print(bmp180.mbar);
    Serial.println(" mbar");
    
    Serial.print(DHT_m.humidity);
    Serial.print(",\t");
    Serial.println(DHT_m.temperature);
    Serial.println(voltage.fvolt);
    Serial.println(epoch);
  }
}

void globalSettings()
{
  // set Sensors
  ac1 = read_2_bytes(0xAA);
  ac2 = read_2_bytes(0xAC);
  ac3 = read_2_bytes(0xAE);
  ac4 = read_2_bytes(0xB0);
  ac5 = read_2_bytes(0xB2);
  ac6 = read_2_bytes(0xB4);
  b1  = read_2_bytes(0xB6);
  b2  = read_2_bytes(0xB8);
  mb  = read_2_bytes(0xBA);
  mc  = read_2_bytes(0xBC);
  md  = read_2_bytes(0xBE);

  /*
  Serial.println("");
  Serial.println("Sensor calibration data:");
  Serial.print(F("AC1 = ")); Serial.println(ac1);
  Serial.print(F("AC2 = ")); Serial.println(ac2);
  Serial.print(F("AC3 = ")); Serial.println(ac3);
  Serial.print(F("AC4 = ")); Serial.println(ac4);
  Serial.print(F("AC5 = ")); Serial.println(ac5);
  Serial.print(F("AC6 = ")); Serial.println(ac6);
  Serial.print(F("B1 = "));  Serial.println(b1);
  Serial.print(F("B2 = "));  Serial.println(b2);
  Serial.print(F("MB = "));  Serial.println(mb);
  Serial.print(F("MC = "));  Serial.println(mc);
  Serial.print(F("MD = "));  Serial.println(md);
  Serial.println("");
  */
  
  pinMode(RST, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(ESP8266LowPower, OUTPUT);
  
  //
  digitalWrite(LED, HIGH);
  digitalWrite(RST, LOW);
  digitalWrite(ESP8266LowPower, LOW);
  delay(100);
  digitalWrite(RST, HIGH);
  delay(100);
  digitalWrite(ESP8266LowPower, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
}

//boolean testESP8266()
//{
//  Serial.println("At");
//  Serial1.println("AT");
//  
//  if(serialRider("OK"))
//  {
//    return true;
//  }
//  else
//  {
//    return false;
//  }
//}

//boolean resetESP8266()
//{ 
//  delay(5000);
//  Serial.println("AT+RST");
//  Serial1.println("AT+RST");
//  
//  if(serialRider("ready"))
//  {
//    return true;
//  }
//  else
//  {
//    return false;
//  }
//}

//boolean setWifiMode(String mode)
//{
//  Serial1.println("AT+CWMODE="+mode);
//  Serial.println("AT+CWMODE="+mode);
//  
//  if(serialRider("OK","no change","",""))
//  {
//    return true;
//  }
//  else
//  {
//    return false;
//  }
//}

//boolean connectWiFi() 
//{ 
//  String cmd = "AT+CWJAP=\"";
//  cmd += SSID;
//  cmd += "\",\"";
//  cmd += PASS;
//  cmd += "\"";
//  Serial1.println(cmd);
//  Serial.println(cmd);
//  
//  if(serialRider("OK"))
//  {
//    Serial.print("true");
//    return true;
//  }
//  else
//  {
//    Serial.print("false");
//    return false;
//  }
//}

//boolean setMultiConnection(String mode)
//{
//  Serial1.println("AT+CIPMUX="+mode);
//  Serial.println("AT+CIPMUX="+mode);
//  if(serialRider("OK"))
//  {
//    return true;
//  }
//  else
//  {
//    return false;
//  }
//}

//boolean setConnection()
//{
//  String cmd;
//  cmd = "AT+CIPSTART=\"UDP\",\"";
//  cmd += DEFINED_IP;
//  cmd += "\",9999";
//  Serial.println(cmd);
//  Serial1.println(cmd);
//
//  if(serialRider("OK"))
//  {
//    return true;
//  }
//  else
//  {
//    return false;
//  }
//}

//boolean closeConnection()
//{
//  String cmd;
//  cmd = "AT+CIPCLOSE";
//  Serial.println(cmd);
//  Serial1.println(cmd);
//  
//  if (!Serial1.find("OK")) 
//  {
//    Serial.println("chiusura connessione ko");
//    return false;
//  }
//  
//  /*while(!Serial1.find("OK") or !Serial1.find("Unlink"))
//  {
//  }*/
//  Serial.println("connessione chiusa");
//  return true;
//}

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


/*******************************
/*
/*  Temperatura
/*
/******************************/

void TempEngine()
{
  ds18s20.temp = getTemp();
}

// Read temperature from the DS18S20 in DEG Celsius or return error
float getTemp(){
  //returns the temperature from one DS18B20 in DEG Celsius
 
  byte data[12];
  byte addr[8];
 
  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -2000;
  }
 
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -3000;
  }
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
 
  byte present = ds.reset();
  ds.select(addr);   
  ds.write(0xBE); // Read Scratchpad
 
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
 
  ds.reset_search();
 
  byte MSB = data[1];
  byte LSB = data[0];
 
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
 
  return TemperatureSum;
}



/*******************************
/*
/*  Umidita'
/*
/******************************/ 
dht11 getHumTemp()
{
  int chk;
  //Serial.print("DHT11, \t");
  chk = DHT_m.read(DHT11_PIN);    // READ DATA
  switch (chk)
  {
    case DHTLIB_OK: 
                //Serial.print("OK,\t");
                break;
    case DHTLIB_ERROR_CHECKSUM:
                //Serial.print("Checksum error,\t");
                break;
    case DHTLIB_ERROR_TIMEOUT:
                //Serial.print("Time out error,\t");
                break;
    default:
                //Serial.print("Unknown error,\t");
                break;
  } 
  
  return DHT_m;
 }
 
 void HumTempEngine()
 {
   DHT_m = getHumTemp();
 }
 

/*******************************
/*
/*  Batteria
/*
/******************************/
void checkBatteryLevel() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  voltage.fvolt = result;
  //return result; // Vcc in millivolts
}


/*******************************
/*
/*  Pressione
/*
/******************************/
void checkPressure()
{
      int32_t b5;
      P = pressure(b5);
      //Serial.print("Pressure: ");
      //Serial.print(P, 2);
      //Serial.print(" mbar, ");
      //Serial.print(P * 0.0295299830714, 2);
      //Serial.println(" inHg");
      //Serial.println("");
      bmp180.mbar = (String)P;
      bmp180.mmHg = (String)(P*0.750062);
}

/**********************************************
  Calculate pressure readings
 **********************************************/
float pressure(int32_t b5)
{
  int32_t x1, x2, x3, b3, b6, p, UP;
  uint32_t b4, b7; 

  UP = read_pressure();                         // Read raw pressure

  b6 = b5 - 4000;
  x1 = (b2 * (b6 * b6 >> 12)) >> 11; 
  x2 = ac2 * b6 >> 11;
  x3 = x1 + x2;
  b3 = (((ac1 * 4 + x3) << oss) + 2) >> 2;
  x1 = ac3 * b6 >> 13;
  x2 = (b1 * (b6 * b6 >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4 * (uint32_t)(x3 + 32768)) >> 15;
  b7 = ((uint32_t)UP - b3) * (50000 >> oss);
  if(b7 < 0x80000000) { p = (b7 << 1) / b4; } else { p = (b7 / b4) << 1; } // or p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  return (p + ((x1 + x2 + 3791) >> 4)) / 100.0f; // Return pressure in mbar
}


// Al momento non serve....
/**********************************************
  Read 1 byte from the BMP sensor
 **********************************************/
uint8_t read_1_byte(uint8_t code)
{
  uint8_t value;
  Wire.beginTransmission(ADDRESS_SENSOR);         // Start transmission to device 
  Wire.write(code);                               // Sends register address to read from
  Wire.endTransmission();                         // End transmission
  Wire.requestFrom(ADDRESS_SENSOR, 1);            // Request data for 1 byte to be read
  if(Wire.available() >= 1)
  {
    value = Wire.read();                          // Get 1 byte of data
  }
  return value;                                   // Return value
}

/**********************************************
  Read 2 bytes from the BMP sensor
 **********************************************/
uint16_t read_2_bytes(uint8_t code)
{
  uint16_t value;
  Wire.beginTransmission(ADDRESS_SENSOR);         // Start transmission to device 
  Wire.write(code);                               // Sends register address to read from
  Wire.endTransmission();                         // End transmission
  Wire.requestFrom(ADDRESS_SENSOR, 2);            // Request 2 bytes from device
  if(Wire.available() >= 2)
  {
    value = (Wire.read() << 8) | Wire.read();     // Get 2 bytes of data
  }
  return value;                                   // Return value
}


/**********************************************
  Read uncompensated pressure value
 **********************************************/
int32_t read_pressure()
{
  int32_t value; 
  Wire.beginTransmission(ADDRESS_SENSOR);   // Start transmission to device 
  Wire.write(0xf4);                         // Sends register address to read from
  Wire.write(0x34 + (oss << 6));            // Write data
  Wire.endTransmission();                   // SEd transmission
  delay(osd);                               // Oversampling setting delay
  Wire.beginTransmission(ADDRESS_SENSOR);
  Wire.write(0xf6);                         // Register to read
  Wire.endTransmission();
  Wire.requestFrom(ADDRESS_SENSOR, 3);      // Request three bytes
  if(Wire.available() >= 3)
  {
    value = (((int32_t)Wire.read() << 16) | ((int32_t)Wire.read() << 8) | ((int32_t)Wire.read())) >> (8 - oss);
  }
  return value;                             // Return value
}

/*******************************
/*
/*  NTP
/*
/******************************/
void ntpSync()
{
  bool connectionEstablished = false;
  
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  
  packetBuffer[0] = 0b11100011; // LI=11, Version=100, Mode=011
  packetBuffer[1] = 0; // Stratum, or type of clock
  packetBuffer[2] = 6; // Polling Interval
  packetBuffer[3] = 0xEC; // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  esp8266.setPacketBuffer(packetBuffer);
    
  esp8266.flushESP8266();

  while(connectionEstablished==false)
  {
    if (esp8266.setMultiConnection("1", "UDP", "193.204.114.232", "123"))
    {
      Serial.println("Ntp connected!");
      connectionEstablished=true;
    }
    else
    {
      Serial.println("Unable to connect to NTP server");
      delay(1000);
    }
  }
  delay(5000);
  
  esp8266.flushESP8266();
 
  if(!esp8266.sendDataWithChannel("1", packetBuffer))
  {
    Serial.println("senddata failed");
  }
  else
  {
    Serial.println("senddata succeeded");
  }
  esp8266.flushESP8266();  
  while(!esp8266.closeMultiConnection("1"))
  {
    Serial.println("Connection not closed");
    delay(1000);
  }
  Serial.println("Connection closed");
  
  //packetBuffer = esp8266.getPacketBuffer();
  
  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
//  // combine the four bytes (two words) into a long integer
//  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;
//  //Serial.print("Seconds since Jan 1 1900 = " );
//  //Serial.println(secsSince1900);
//
//  // now convert NTP time into everyday time:
//  //Serial.print("Unix time = ");
//  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
//  // subtract seventy years:
  epoch = secsSince1900 - seventyYears;
  // print Unix time:
  Serial.println(epoch);
}

//  String cmd = "AT+CIPSTART=1,\"";
//  cmd += "UDP";
//  cmd += "\",\"";
//  cmd += "193.204.114.232";
//  cmd += "\",\"";
//  cmd += "123";  
//  cmd += "\"";
//  Serial1.println(cmd);
//  Serial.println(cmd);
//
//  if(!serialRider("OK"))
//  {
//    Serial.println("Unable to connect to NTP server");
//    Serial1.println("AT+CIPCLOSE=1");
//    serialRider("OK","","","");
//    return;
//  }
//  
//  Serial1.println("AT+CIPSEND=1,48");
//
//  if(!serialRider(">","","",""))
//  {
//    Serial.println("Prompt missed.");
//    Serial1.println("AT+CIPCLOSE=1");
//    serialRider("OK","","","");
//    return;
//  }
//  
//  Serial1.write(packetBuffer, NTP_PACKET_SIZE);
//  /*if (!waitforLine("SEND OK", 2000)) 
//  {
//    Serial.println("Package not sent.");
//    Serial1.println("AT+CIPCLOSE=1");
//    return;
//  }*/
//  
//  //delay(2000);
//  if (!waitforLine("+IPD,1,48:", 2000)) 
//  {
//    Serial.println("NTP Server did not respond");
//    Serial1.println("AT+CIPCLOSE=1");
//    return;
//  }
//  
//  Serial1.println("AT+CIPCLOSE=1");
//  while (Serial1.available())
//  {
//    Serial.println("pippo");
//    Serial.write(Serial1.read());
//  }
//
//  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
//  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
//  // combine the four bytes (two words) into a long integer
//  // this is NTP time (seconds since Jan 1 1900):
//  unsigned long secsSince1900 = highWord << 16 | lowWord;
//  //Serial.print("Seconds since Jan 1 1900 = " );
//  //Serial.println(secsSince1900);
//
//  // now convert NTP time into everyday time:
//  //Serial.print("Unix time = ");
//  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
//  const unsigned long seventyYears = 2208988800UL;
//  // subtract seventy years:
//  epoch = secsSince1900 - seventyYears;
//  // print Unix time:
//  Serial.println(epoch);
//}

void ntpSync1()
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  
//  packetBuffer[0] = 0b11100011; // LI=11, Version=100, Mode=011
//  packetBuffer[1] = 0; // Stratum, or type of clock
//  packetBuffer[2] = 6; // Polling Interval
//  packetBuffer[3] = 0xEC; // Peer Clock Precision
//  // 8 bytes of zero for Root Delay & Root Dispersion
//  packetBuffer[12]  = 49; 
//  packetBuffer[13]  = 0x4E;
//  packetBuffer[14]  = D49;
  packetBuffer[15]  = 52;
//print
//22706236000000004978495200000000000000000000000000000000

  String cmd = "AT+CIPSTART=1,\"";
  cmd += "UDP";
  cmd += "\",\"";
  //cmd += "193.204.114.232";
  cmd += "46.101.52.119";
  cmd += "\",\"";
  cmd += "123";  
  cmd += "\"";
  Serial1.println(cmd);

  //while (Serial1.available())
  //{
    if (!Serial1.find("OK")) 
    {
      Serial.println("Unable to connect to NTP server");
      Serial1.println("AT+CIPCLOSE=1");
      return;
    } 
  //} 

  //sendData("AT+CIPSTART=1,\"UDP\",\"193.204.114.232\",123",2000);
  
  //delay(5000);
  
  //Serial.println("Query to send...");
  Serial1.println("AT+CIPSEND=1,48");

//  if (!Serial1.find(">"))
//  {
//    Serial.println("Prompt missed");
//    delay(5000);
//    Serial1.println("AT+CIPCLOSE=1");
//    return;
//  }

    if (!Serial1.find(">"))
    {
      Serial.println("Prompt missed");
      //delay(5000);
      Serial1.println("AT+CIPCLOSE=1");
      Serial.write(Serial1.read());
      return;
    }
  
  Serial1.write(packetBuffer, 48);
  //delay (2000);
  //Serial.write(Serial1.readBytes(packetBuffer, 48));

//  {
    if (!waitforLine("+IPD,1,48:", 2000)) 
    {
      Serial.println("NTP Server did not respond");
      Serial1.println("AT+CIPCLOSE=1");
      return;
    }
//  } 
  
  Serial1.println("AT+CIPCLOSE=1");
  if (!Serial1.find("OK"))
  {
    Serial.println("Unable to close port");
    return;
  }
  
//  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]); //40 41
//  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]); // 42 43
//  // combine the four bytes (two words) into a long integer
//  // this is NTP time (seconds since Jan 1 1900):
//  unsigned long secsSince1900 = highWord << 16 | lowWord;
//  
//  // now convert NTP time into everyday time:
//  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
//  const unsigned long seventyYears = 2208988800UL;
//  // subtract seventy years:
//  epoch = secsSince1900 - seventyYears;
//  // print Unix time:
//  //Serial.println(epoch);
}

boolean waitforLine(char *string, int timeout)
{
  unsigned long endtime = millis() + timeout;
  const int MAXBUF = 256;
  static char buffer[MAXBUF];  

  while (millis() < endtime)
  {
    //checkStatus();
    if (readline(Serial1.read(), buffer, MAXBUF) > 0) 
    {
      Serial.print("string ");
      Serial.println(string);
      Serial.print("buffer ");
      Serial.println(buffer);
      
//    for (byte i = 0; i < NTP_PACKET_SIZE; i++)
//    {
//      Serial.print(i);
//      Serial.print(" ");
//      Serial.print(buffer[i]);
//      Serial.print(" ");
//      Serial.println((uint8_t)buffer[i]);
//      delay(5);
//    }
      
      String test = strstr(buffer, string);
      
      if (strstr(buffer, string) != NULL)
      {
        String substringBuffer = buffer;
        substringBuffer = substringBuffer.substring(substringBuffer.indexOf(':')+1);
        
        int pos = 16;
        while (pos < NTP_PACKET_SIZE) 
        {
//          packetBuffer[pos] = substringBuffer[pos-16];
          pos++;
        }
        return true;
      }
      if (strstr(buffer, "no change") != NULL)
        return true;
    }
  }
  return false;
}

int readline(int readch, char *buffer, int len)
{
  static int pos = 0;
  int rpos;

  if (readch > 0) {
    //Serial.println(readch);
    switch (readch) {
      case '\n':  // Ignore new lines
        break;
      case '\r': // Return on CR
        rpos = pos;
        pos = 0;  // Reset position for next input
        return rpos;
      default:
        if (pos < len-1) {
          buffer[pos++] = readch;
          buffer[pos] = '\0';
        }
    }
  }
  // No end of line found so return -1.
  return -1;
} 

void sendData(String command, const int timeout)
{
  String response = "";
  
  Serial1.print(command); // send the read character to the esp8266
  
  long int time = millis();
  
  while( (time+timeout) > millis())
  {
    while(Serial1.available())
    {
      // The esp has data so display its output to the serial window
      char c = Serial1.read(); // read the next character.
      response+=c;
    }
  }
  
  Serial.println("Response: " + response);
}
//AT+CIPMUX=1
//AT+CWJAP="BTHub5-CKRP","5498d7a573"
//AT+CIPSTART=1,"UDP","193.204.114.232",123
//                                                
//AT+CIPCLOSE=1



boolean serialRider(char *string) //suggested by Marta, formerly serialReader
{
  memset(receivedChars, 0, numChars);
  boolean foundIt = false;
  static byte ndx = 0;
  char carriageReturn = '\r';
  char newLine = '\n';
  char rc;
  boolean CAA = true; //Comando Arlecchino Avviato

  while(CAA)
  {
    while (Serial1.available() > 0 && newData == false) 
    {
      rc = Serial1.read();
      
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
      //Serial.println(newData);
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
      #if defined(_DEBUG_)==1
      showNewData();
      #endif
      newData = false;
    }
  }
  return foundIt;
}

boolean serialRider(char *string0, char *string1, char *string2, char *string3) //suggested by Marta, formerly serialReader
{
  if(string1 == "")
  {
    string1="OK";
  }
  
  if(string2 == "")
  {
    string2="OK";
  }
  
  if(string3 == "")
  {
    string3="OK";
  }
  
  memset(receivedChars, 0, numChars);
  boolean foundIt = false;
  static byte ndx = 0;
  char carriageReturn = '\r';
  char newLine = '\n';
  char rc;
  boolean CAA = true; //Comando Arlecchino Avviato

  while(CAA)
  {
    while (Serial1.available() > 0 && newData == false) 
    {
      rc = Serial1.read();
      
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
      
      if (receivedChars[0] == '>')
      {
          receivedChars[ndx] = '\0';
          ndx = 0;
          newData = true;
      }
      if (newData == true && (strcmp(receivedChars, string0)==0 || strcmp(receivedChars, string1)==0 || strcmp(receivedChars, string2)==0 || strcmp(receivedChars, string3)==0) )
      {
        foundIt = true;
        CAA=false;
      }
      if (strcmp(receivedChars, "ERROR")==0 || strcmp(receivedChars, "FAIL")==0)
      {
        CAA=false;
      }
      #if defined(_DEBUG_)==1
      showNewData();
      #endif
      newData = false;
    }
  }
  return foundIt;
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
    }
}
