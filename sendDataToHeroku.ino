#include <avr/sleep.h>
#include <avr/wdt.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

uint8_t GPSoff[] = {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x4D, 0x3B};
uint8_t GPSon[] = {0xB5, 0x62, 0x02, 0x41, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x4C, 0x37};

static const int RXPin = 5, TXPin = 4;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS dev8ice
SoftwareSerial ss(RXPin, TXPin);


char URL[] = "drais.herokuapp.com";
//char URL[] = "gpsworkana.herokuapp.com";
char CODE[] = "2795";
char KEY[] = "TEST2795";


char APN[] = "datos.personal.com";
char APNUSER[] = "datos";
char APNPASS[] = "datos";

/*
char APN[] = "internet.itelcel.com";
char APNUSER[] = "webgprs";
char APNPASS[] = "webgprs2002";
*/
char replybuffer[500]    = "\0";

int descanso = 2;
boolean apagar = false;
int tiempo = 0;
const byte SLEEP_SIM = 10;

// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect

void setup() {
    Serial.begin(9600);

    ss.begin(GPSBaud);

    pinMode(SLEEP_SIM, OUTPUT);
    digitalWrite(SLEEP_SIM, LOW);
    delay(300);

    sendUBX(GPSon, sizeof(GPSon)/sizeof(uint8_t));
    
    initSim();
    
    activarConexionInternet();
    delay(300);
}

void loop() {
    if(apagar) {
        apagarConexionInternet();

        Serial.print("AT+CFUN=0\r\n");
        readData(50);
        
        Serial.print("AT+CSCLK=1\r\n");
        delay(100);
        digitalWrite(SLEEP_SIM, HIGH);

        sendUBX(GPSoff, sizeof(GPSoff)/sizeof(uint8_t));
        
        tiempo--;
        tiempo *= 60;
        for(int i = 0; i < tiempo; i++) {
            delay(1000);    
        }
        /*int limite = tiempo / 8;
        for(int i = 0; i < limite; i++) {
            // disable ADC
            ADCSRA = 0;
            
            // clear various "reset" flags
            MCUSR = 0;     
            // allow changes, disable reset
            WDTCSR = bit (WDCE) | bit (WDE);
            // set interrupt mode and an interval 
            WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
            wdt_reset();  // pat the dog
              
            set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
            noInterrupts ();           // timed sequence follows
            sleep_enable();
             
            // turn off brown-out enable in software
            MCUCR = bit (BODS) | bit (BODSE);
            MCUCR = bit (BODS); 
            interrupts ();             // guarantees next instruction executed
            sleep_cpu ();  
              
            // cancel sleep as a precaution
            sleep_disable();
        }*/

        sendUBX(GPSon, sizeof(GPSon)/sizeof(uint8_t));
        
        digitalWrite(SLEEP_SIM, LOW);
        delay(100);
        Serial.print("AT\r\n");
        readData(100);
        Serial.print("AT+CSCLK=0\r\n");
        readData(100);
        initSim();
        activarConexionInternet();
        apagar = false;
    }
    smartDelay(1000);

    enviarCoordenadas();
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}



void enviarCoordenadas() {
    
    Serial.print(F("AT+CIPSTART=TCP,"));
    Serial.print(URL);
    Serial.print(F(",80\r\n")); // Attach or Detach from GPRS Service      
    if(strstr(readData(1000),"OK")) {
        Serial.print(F("AT+CIPSEND\r\n"));
        if(strstr(readData(500), ">")) {
            
            Serial.print(F("GET /api/device/"));
            Serial.print(CODE);
            Serial.print(F("/"));
            Serial.print(KEY);
            Serial.print(F("/lat/"));
            if(gps.location.isValid()) {
                Serial.print(gps.location.lat(), 6);
            } else {
                Serial.print("***********");
            }
            Serial.print(F("/lon/"));
            if(gps.location.isValid()) {
                Serial.print(gps.location.lng(), 6);
            } else {
                Serial.print("***********");
            }
            
            
            /*if(gps.location.isValid()) {
                Serial.print(F("GET /index.php?lat="));
                Serial.print(gps.location.lat(), 6);
                Serial.print(F("&lon="));
                Serial.print(gps.location.lng(), 6);
            } else {
                Serial.print(F("GET /index.php?lat=*****&lon=******"));
            }*/
            
            Serial.print(F(" HTTP/1.1\r\n"));
            
            Serial.print(F("Host: "));
            Serial.print(URL);
            Serial.print(F("\r\n\r\n"));
            Serial.write(0x1A);
            
            char *apagarDispositivo = strstr(readData(2000), "apagar"); 
            if(apagarDispositivo != NULL) {
                apagarDispositivo += 7;
                tiempo = (*apagarDispositivo - '0');
                apagarDispositivo++;
                tiempo *= 10;
                tiempo += (*apagarDispositivo - '0');
                apagar = true;
            }
        }
    }
    Serial.print(F("AT+CIPCLOSE\r\n"));
    readData(50);
}


char* readData(uint16_t timeout) {
  uint16_t replyidx = 0;
  for(int i = 0; i < 500; i++) {
    replybuffer[i] = 0;  
  }
  while (timeout--) {
    if (replyidx >= 500) {
      break;
    }
    while (Serial.available()) {
      char c =  Serial.read();
      if (c == '\r') continue;
      if (c == 0xA) {
        if (replyidx == 0)   // the first 0x0A is ignored
          continue;
      }
      replybuffer[replyidx] = c;
      replyidx++;
    }
    if (timeout == 0) {
      break;
    }
    delay(1);
  }
  replybuffer[replyidx] = '\0';  // null term
  
  while (Serial.available()) {
    Serial.read();
  }
  return replybuffer;
}

void initSim() {
    Serial.print(F("AT\r\n")); // Attach or Detach from GPRS Service
    while(strstr(readData(500),"OK")==NULL) {
        Serial.print(F("AT\r\n")); // Attach or Detach from GPRS Service
    }

    Serial.print(F("AT+CFUN=1\r\n")); // Attach or Detach from GPRS Service
    while(strstr(readData(1000),"OK")==NULL) {
        Serial.print(F("AT+CFUN=1\r\n")); // Attach or Detach from GPRS Service
    }

    Serial.print(F("AT+CGATT=1\r\n")); // Attach or Detach from GPRS Service
    while(strstr(readData(1000),"OK")==NULL) {
        Serial.print(F("AT+CGATT=1\r\n")); // Attach or Detach from GPRS Service
    }
}

void apagarConexionInternet() {
    Serial.print(F("AT+CIPSHUT\r\n"));  // Apagamos la conneccion a internet
    readData(500);
}

void activarConexionInternet() {
    sendUBX(GPSon, sizeof(GPSon)/sizeof(uint8_t));
    apagarConexionInternet();
        
    Serial.print(F("AT+CSTT="));
    Serial.print(APN);
    Serial.print(F(","));
    Serial.print(APNUSER);
    Serial.print(F(","));
    Serial.print(APNPASS);
    Serial.print(F("\r\n")); // Realiza una conexion al APN
    while(strstr(readData(5000),"OK")==NULL) {
        Serial.print(F("AT+CSTT="));
        Serial.print(APN);
        Serial.print(F(","));
        Serial.print(APNUSER);
        Serial.print(F(","));
        Serial.print(APNPASS);
        Serial.print(F("\r\n")); // Realiza una conexion al APN
    }
    
    Serial.print(F("AT+CIICR\r\n")); // Activamos la conexión a internet
    while(strstr(readData(15000),"OK")==NULL) {
        Serial.print(F("AT+CIICR\r\n")); // Activamos la conexión a internet
    }
    
    
    Serial.print(F("AT+CIFSR\r\n")); // Obtencion de direccion ip
    while(strstr(readData(15000),"ERROR")!=NULL) {
        Serial.print(F("AT+CIFSR\r\n")); // Obtención de dirección ip
    }
}

void sendUBX(uint8_t MSG[], int tam) {
    for(int i = 0; i < tam; i++) {
        ss.write(MSG[i]);    
    }
}
