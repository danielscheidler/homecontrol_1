/* 
 homecontrol - v1.0
 
 Arduino Sketch für homecontrol 
 Copyright (c) 2013 Daniel Scheidler All right reserved.
 
 homecontrol ist Freie Software: Sie können es unter den Bedingungen
 der GNU General Public License, wie von der Free Software Foundation,
 Version 3 der Lizenz oder (nach Ihrer Option) jeder späteren
 veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 
 homecontrol wird in der Hoffnung, dass es nützlich sein wird, aber
 OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 Siehe die GNU General Public License für weitere Details.
 
 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */


#include <SPI.h>
#include <Ethernet.h>
#include <MemoryFree.h>
#include <RCSwitch.h>

int lastMem = freeMemory();


// Transmitter is connected to Arduino Pin #7  
#define TRANSMITTER_PIN 7
//Receiver is on Interrupt 0 - Arduino Pin #2 
#define RECEIVER_PIN 2

// Variablen für Netzwerkdienste
EthernetServer HttpServer(80); 
EthernetClient interfaceClient;


// Variablen für Webseiten/Parameter
char*      rawCmdAnschluss          = (char*)malloc(sizeof(char)*20);

const int  MAX_BUFFER_LEN           = 80; // max characters in page name/parameter 
char       buffer[MAX_BUFFER_LEN+1]; // additional character for terminating null


// Instance of RCSwitch
RCSwitch mySwitch = RCSwitch();

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

const __FlashStringHelper * htmlHeader;
const __FlashStringHelper * htmlHead;
const __FlashStringHelper * htmlFooter;

// ------------------ Reset stuff --------------------------
void(* resetFunc) (void) = 0;
unsigned long resetMillis;
boolean resetSytem = false;
// --------------- END - Reset stuff -----------------------



/**
 * SETUP
 *
 * Grundeinrichtung der HomeControl
 * - Serielle Ausgabe aktivieren
 * - TFT initialisieren
 * - Netzwerk initialisieren
 * - Webserver starten
 * - IN-/OUT- Pins definieren
 * - Waage initialisieren (Tara)
 */
void setup() {
  unsigned char mac[]  = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED  };
  unsigned char ip[]   = { 192, 168, 1, 12 };
  unsigned char dns[]  = { 192, 168, 1, 1  };
  unsigned char gate[] = { 192, 168, 1, 1  };
  unsigned char mask[] = { 255, 255, 255, 0  };

  // Serial initialisieren
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println(F("HomeControl v1"));
  Serial.println();
  
  mySwitch.enableTransmit(TRANSMITTER_PIN);
  mySwitch.enableReceive(RECEIVER_PIN);
 
  // Optional set pulse length.
 //mySwitch.setPulseLength(350);  



  // Netzwerk initialisieren
  Ethernet.begin(mac, ip, dns, gate, mask);
  HttpServer.begin();

  Serial.print( F("IP: ") );
  Serial.println(Ethernet.localIP());

  initStrings();
}

/**
 * LOOP
 * 
 * Standard-Loop-Methode des Arduino Sketch
 * Diese Methode wird nach dem Setup endlos wiederholt.
 *
 * - Gewicht aktualisieren
 * - Webserver: 
 *    * auf Client warten
 *      * Falls Client verbunden ist entsprechende Webseite ausgeben
 *        und Verbindung beenden.
 */
void loop() {
  EthernetClient client = HttpServer.available();
  if (client) {
    while (client.connected()) {
      if(client.available()){        
        Serial.println(F("Website anzeigen"));
        showWebsite(client);
        
        Serial.print(F("> Freier Speicher: "));
        Serial.println(freeMemory());
        delay(100);
        client.stop();
      }
    }
  }
  delay(100);
  // Gecachte URL-Parameter leeren
  memset(rawCmdAnschluss,0, sizeof(rawCmdAnschluss)); 
}

// ---------------------------------------
//     RcSwitch Hilfsmethoden
// ---------------------------------------

void switchWirelessOutlet(int number){
  mySwitch.disableReceive();
  Serial.print("Schalte: ");
  Serial.println(number);
  delay(10);
  
  int numberStk = number % 5;  
  if (numberStk == 0) numberStk = 5;
  if (numberStk < 0) numberStk = numberStk*(-1);
  
  int numberStkIT = (number-50) % 4;
  if (numberStkIT == 0) numberStkIT = 4;
  if (numberStkIT < 0) numberStkIT = numberStkIT*(-1);

  if (number > 0){ 
    //Intertechno Logik
    if (number > 306  && number <= 322){
      mySwitch.switchOn('p',((((number-306)-1)/4)+1), numberStkIT);  
    } else if (number > 290){
      mySwitch.switchOn('o',((((number-290)-1)/4)+1), numberStkIT);  
    } else if (number > 258){
      mySwitch.switchOn('n',((((number-258)-1)/4)+1), numberStkIT);  
    } else if (number > 242){
      mySwitch.switchOn('m',((((number-242)-1)/4)+1), numberStkIT);  
    } else if (number > 226){
      mySwitch.switchOn('l',((((number-226)-1)/4)+1), numberStkIT);  
    } else if (number > 210){
      mySwitch.switchOn('k',((((number-210)-1)/4)+1), numberStkIT);  
    } else if (number > 194){
      mySwitch.switchOn('j',((((number-194)-1)/4)+1), numberStkIT);  
    } else if (number > 178){
      mySwitch.switchOn('i',((((number-178)-1)/4)+1), numberStkIT);  
    } else if (number > 162){
      mySwitch.switchOn('h',((((number-162)-1)/4)+1), numberStkIT);  
    } else if (number > 146){
      mySwitch.switchOn('g',((((number-146)-1)/4)+1), numberStkIT);  
    } else if (number > 130){
      mySwitch.switchOn('f',((((number-130)-1)/4)+1), numberStkIT);  
    } else if (number > 114){
      mySwitch.switchOn('e',((((number-114)-1)/4)+1), numberStkIT);  
    } else if (number > 98){
      mySwitch.switchOn('d',((((number-98)-1)/4)+1), numberStkIT);  
    } else if (number > 82){
      mySwitch.switchOn('c',((((number-82)-1)/4)+1), numberStkIT);  
    } else if (number > 66){
      mySwitch.switchOn('b',((((number-66)-1)/4)+1), numberStkIT);  
    } else if (number > 50){
      Serial.print("RC Switch: ");
      Serial.print(((((number-50)-1)/4)+1));
      Serial.print(" - ");
      Serial.println(numberStkIT);

      mySwitch.switchOn('a',((((number-50)-1)/4)+1), numberStkIT);  
    }else{
      //Standard Logik
      mySwitch.switchOn(int2bin(((number-1)/5)+1), numberStk);
    }
  } else {
    number = number * (-1);
    //Intertechno Logik
    if (number > 306  && number <= 322){
      mySwitch.switchOff('p',((((number-306)-1)/4)+1), numberStkIT);  
    } else if (number > 290){
      mySwitch.switchOff('o',((((number-290)-1)/4)+1), numberStkIT);  
    } else if (number > 258){
      mySwitch.switchOff('n',((((number-258)-1)/4)+1), numberStkIT);  
    } else if (number > 242){
      mySwitch.switchOff('m',((((number-242)-1)/4)+1), numberStkIT);  
    } else if (number > 226){
      mySwitch.switchOff('l',((((number-226)-1)/4)+1), numberStkIT);  
    } else if (number > 210){
      mySwitch.switchOff('k',((((number-210)-1)/4)+1), numberStkIT);  
    } else if (number > 194){
      mySwitch.switchOff('j',((((number-194)-1)/4)+1), numberStkIT);  
    } else if (number > 178){
      mySwitch.switchOff('i',((((number-178)-1)/4)+1), numberStkIT);  
    } else if (number > 162){
      mySwitch.switchOff('h',((((number-162)-1)/4)+1), numberStkIT);  
    } else if (number > 146){
      mySwitch.switchOff('g',((((number-146)-1)/4)+1), numberStkIT);  
    } else if (number > 130){
      mySwitch.switchOff('f',((((number-130)-1)/4)+1), numberStkIT);  
    } else if (number > 114){
      mySwitch.switchOff('e',((((number-114)-1)/4)+1), numberStkIT);  
    } else if (number > 98){
      mySwitch.switchOff('d',((((number-98)-1)/4)+1), numberStkIT);  
    } else if (number > 82){
      mySwitch.switchOff('c',((((number-82)-1)/4)+1), numberStkIT);  
    } else if (number > 66){
      mySwitch.switchOff('b',((((number-66)-1)/4)+1), numberStkIT);  
    } else if (number > 50){
      Serial.print("RC Switch: ");
      Serial.print(((((number-50)-1)/4)+1));
      Serial.print(" - ");
      Serial.println(numberStkIT);

      mySwitch.switchOff('a',((((number-50)-1)/4)+1), numberStkIT);  
    } else {
      //Standard Logik
      mySwitch.switchOff(int2bin(((number-1)/5)+1), numberStk);
    } 
  }
  
  delay(10);
  mySwitch.enableReceive(0);
}


// ---------------------------------------
//     Webserver Hilfsmethoden
// ---------------------------------------

/**
 *  URL auswerten und entsprechende Seite aufrufen
 */
void showWebsite(EthernetClient client){
  char * HttpFrame =  readFromClient(client);
  
  delay(200);
  boolean pageFound = false;
  
  char *ptr = strstr(HttpFrame, "favicon.ico");
  if(ptr){
    pageFound = true;
  }
  ptr = strstr(HttpFrame, "index.html");
  if (!pageFound && ptr) {
    runIndexWebpage(client);
    pageFound = true;
  } 
  ptr = strstr(HttpFrame, "rawCmd");
  if(!pageFound && ptr){
    runRawCmdWebpage(client, HttpFrame);
    pageFound = true;
  } 

  delay(300);

  ptr=NULL;
  HttpFrame=NULL;

 if(!pageFound){
    runIndexWebpage(client);
  }
  delay(20);
}




// ---------------------------------------
//     Webseiten
// ---------------------------------------

/**
 * Startseite anzeigen
 */
void  runIndexWebpage(EthernetClient client){
  showHead(client);

  client.print(F("<h4>Navigation</h4><br/>"
    "<a href='/rawCmd'>Manuelle Schaltung</a><br>"));

  showFooter(client);
}


/**
 * rawCmd anzeigen
 */
void  runRawCmdWebpage(EthernetClient client, char* HttpFrame){
  if (atoi(rawCmdAnschluss)!=0 ) {
    postRawCmd(client, rawCmdAnschluss);
    return;
  }


  showHead(client);
  
  client.println(F(  "<h4>Manuelle Schaltung</h4><br/>"
                     "<form action='/rawCmd'>"));

  client.println(F( "<b>Anschluss: </b>" 
                    "<select name=\"schalte\" size=\"1\" > "));
  
  client.println(F( "  <option value=\"1\">Anschluss 1</option>"
                    "  <option value=\"2\">Anschluss 2</option>"
                    "  <option value=\"3\">Anschluss 3</option>"
                    "  <option value=\"4\">Anschluss 4</option>"
                    "  <option value=\"5\">Anschluss 5</option>"
                    "  <option value=\"6\">Anschluss 6</option>"
                    "  <option value=\"7\">Anschluss 7</option>"
                    "  <option value=\"8\">Anschluss 8</option>"
                    "  <option value=\"9\">Anschluss 9</option>"
                    "  <option value=\"10\">Anschluss 10</option>"
                    "  <option value=\"11\">Anschluss 11</option>"
                    "  <option value=\"12\">Anschluss 12</option>"
                    "  <option value=\"13\">Anschluss 13</option>"
                    "  <option value=\"14\">Anschluss 14</option>"
                    "  <option value=\"15\">Anschluss 15</option>"
                    "  <option value=\"16\">Anschluss 16</option>"));
                    
  client.println(F( "</select>" ));

  client.println(F("<input type='submit' value='Abschicken'/>"
                   "</form>"));

  showFooter(client);
}


void postRawCmd(EthernetClient client, char* anschluss){
  showHead(client);
  
  switchWirelessOutlet(atoi(anschluss));
  
  showFooter(client);
}





// ---------------------------------------
//     HTML-Hilfsmethoden
// ---------------------------------------

void showHead(EthernetClient client){
  client.println(htmlHeader);

  client.println(htmlHead);
}


void showFooter(EthernetClient client){
  client.println(F("<div  style=\"position: absolute;left: 30px; bottom: 40px; text-align:left;horizontal-align:left;\" width=200>"));
  client.print(F("<b>Freier Speicher:</b> "));                              
  client.println(freeMemory()); 
  client.println(F("</div>"));
  client.print(htmlFooter);
}


void initStrings(){
  htmlHeader = F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");
  
  htmlHead = F("<html><head>"
    "<title>HomeControl</title>"
    "<style type=\"text/css\">"
    "body{font-family:sans-serif}"
    "*{font-size:14pt}"
    "a{color:#abfb9c;}"
    "</style>"
    "</head><body text=\"white\" bgcolor=\"#494949\">"
    "<center>"
    "<hr><h2>HomeControl</h2><hr>") ;
    
    htmlFooter = F( "</center>"
    "<a  style=\"position: absolute;left: 30px; bottom: 20px; \"  href=\"/\">Zur&uuml;ck zum Hauptmen&uuml;</a>"
    "<span style=\"position: absolute;right: 30px; bottom: 20px; \">Entwickelt von Daniel Scheidler und Julian Theis</span>"
    "</body></html>");
    
}


// ---------------------------------------
//     Ethernet - Hilfsmethoden
// ---------------------------------------
/**
 * Zum auswerten der URL des übergebenen Clients
 * (implementiert um angeforderte URL am lokalen Webserver zu parsen)
 */
char* readFromClient(EthernetClient client){
  char paramName[20];
  char paramValue[20];
  char pageName[20];
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        memset(buffer,0, sizeof(buffer)); // clear the buffer

        client.find("/");
        if(byte bytesReceived = client.readBytesUntil(' ', buffer, sizeof(buffer))){ 
          buffer[bytesReceived] = '\0';

          Serial.print(F("URL: "));
          Serial.println(buffer);
          
          if(strcmp(buffer, "favicon.ico\0")){
            char* paramsTmp = strtok(buffer, " ?=&/\r\n");
            int cnt = 0;
            while (paramsTmp) {
              //Serial.print(F("Read: "));
              //Serial.println(paramsTmp);
              
              switch (cnt) {
                case 0:
                  strcpy(pageName, paramsTmp);
                  Serial.print(F("Domain: "));
                  Serial.println(buffer);
                  break;
                case 1:
                  strcpy(paramName, paramsTmp);
                  Serial.print(F("Parameter: "));
                  Serial.print(paramName);
                  break;
                case 2:
                  strcpy(paramValue, paramsTmp);
                  Serial.print(F(" = "));
                  Serial.println(paramValue);
                  pruefeURLParameter(paramName, paramValue);
                  break;
              }
              
              paramsTmp = strtok(NULL, " ?=&/\r\n");
              cnt=cnt==0?1:cnt==1?2:1;
            }
          }
    
        }
      }// end if Client available
      break;
    }// end while Client connected
  } 

  return buffer;
}


void pruefeURLParameter(char* tmpName, char* value){
  if(strcmp(tmpName, "schalte")==0 && strcmp(value, "")!=0){
    strcpy(rawCmdAnschluss, value);
    
    Serial.print(F("OK Anschluss: "));
    Serial.println(rawCmdAnschluss);    
  }  
}

// ---------------------------------------
//     Allgemeine Hilfsmethoden
// ---------------------------------------
char* int2bin(unsigned int x)
{
  static char buffer[6];
  for (int i=0; i<5; i++) buffer[4-i] = '0' + ((x & (1 << i)) > 0);
  buffer[68] ='\0';
  return buffer;
}
