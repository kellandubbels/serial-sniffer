
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306();

#if defined(ESP8266)
  #define BUTTON_A 0
  #define BUTTON_B 16
  #define BUTTON_C 2
  #define LED      0
#else 
  #define BUTTON_A 9
  #define BUTTON_B 6
  #define BUTTON_C 5
  #define LED      13
#endif

#if (SSD1306_LCDHEIGHT != 32)
 #error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// variables
int last =84;
bool teletype = true;
char buf1[4096]; int i1 =0;
char buf2[10240]; int i2 =0;  //main storage buffer and pointer
char buf3[4096]; int i3 =0;

// ESP WiFi mode: uncomment line to select MODE_AP or MODE_STA
#define MODE_AP // phone connects directly to ESP
//#define MODE_STA // ESP connects to router

#ifdef MODE_AP
// For AP mode:
const char *ssid = "SerialSniffer";  // You will connect to this Access Point
const char *pw = "232"; // this is the password
IPAddress ip(192, 168, 0, 1);
IPAddress netmask(255, 255, 255, 0);
const int port = 23;
#endif

#ifdef MODE_STA
// For STATION mode:
const char *ssid = "NETGEAR37";  // The ROUTER SSID
const char *pw = "brightstreet607"; // The WiFi PASSWORD
const int port = 23;
#endif

WiFiServer server(port);
WiFiClient client;


void setup() {  

  //initialize UART as serial port at 9600 baud
  Serial.begin(9600);

  //initialize OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display(); // Display splash screen.
  display.clearDisplay(); // Clear the buffer.
  display.display(); // Commit display.
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.invertDisplay(true);
  display.setCursor(0,0);
  display.print(" v2 Serial Sniffer \n");
  display.println(" by Kellan Dubbels \n");
  display.display(); // actually display all of the above
  
  
  //initialize AP mode (connect directly to ESP SSID)  (no router)
  #ifdef MODE_AP 
  WiFi.softAPConfig(ip, ip, netmask); // configure ip address for softAP 
  WiFi.softAP(ssid, pw); // configure ssid and password for softAP
  #endif
  
  //initialize STATION mode (ESP connects to router SSID and gets an IP)
  #ifdef MODE_STA
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  while (WiFi.status() != WL_CONNECTED) {delay(100);}
  #endif

  // start TCP server:
  server.begin();   



} // End of setup

























//main loop
void loop() {


if(i2=0) {
  display.clearDisplay(); // Clear the buffer.
  display.invertDisplay(true);
  display.setCursor(0,0);
  display.print(" v2 Serial Sniffer \n");
  display.print(" by Kellan Dubbels \n");
  display.display(); // actually display all of the above
  delay(1000);
  display.clearDisplay(); // Clear the buffer.
  display.invertDisplay(true);
  display.setCursor(8,8);
  display.print(" v2 Serial Sniffer \n");
  display.setCursor(8,16);
  display.print(" by Kellan Dubbels \n");
  display.display(); // actually display all of the above
  delay(1000);
  display.clearDisplay(); // Clear the buffer.
  display.invertDisplay(true);
  display.setCursor(16,16);
  display.print(" v2 Serial Sniffer \n");
  display.setCursor(16,24);
  display.print(" by Kellan Dubbels \n");
  display.display(); // actually display all of the above
  delay(1000);
  display.clearDisplay(); // Clear the buffer. 
  display.invertDisplay(false);
  display.setCursor(0,0);
  display.print(" v2 Serial Sniffer \n");
  display.print(" by Kellan Dubbels \n");
  display.display(); // actually display all of the above
  delay(1000);  
  display.clearDisplay(); // Clear the buffer.
  display.invertDisplay(false);
  display.setCursor(8,8);
  display.print(" v2 Serial Sniffer \n");
  display.setCursor(8,16);
  display.print(" by Kellan Dubbels \n");
  display.display(); // actually display all of the above
  delay(1000);
  display.clearDisplay(); // Clear the buffer.
  display.invertDisplay(false);
  display.setCursor(16,16);
  display.print(" v2 Serial Sniffer \n");
  display.setCursor(16,24);
  display.print(" by Kellan Dubbels \n");
  display.display(); // actually display all of the above
  delay(1000);
  
}   // End of splash graphice










  // check for any new data from telnet client  
  if(client.available()) { 
    while(client.available()) {
       buf1[i1] = (char)client.read(); // read char from telnet client 
       buf2[i2] = buf1[i1]; 
       if(i1 < 4095) {i1++;};
       if(i2 < 10239) {i2++;} else {i2 = 0;}; 
    }
    // push telnet buffer to UART and OLED:
    Serial.write(buf1, i1);
    i1 = 0;
    displayRefresh(last);
  }
  

  // check for any new data in UART hardware buffer
  if(Serial.available()) { 
    while(Serial.available()) {
      buf2[i2] = (char)Serial.read(); // read char from UART
      buf3[i3] = buf2[i2]; 
      if(i2 < 10239) {i2++;} else {i2 = 0;}    
      if(i3 < 4095) i3++;
    }
    // push UART buffer to WiFi and OLED:
    client.write((char*)buf3, i3);
    i3 = 0;
    displayRefresh(last);
  }

  // scroll up with BUTTON A after checking for underflow
  if (! digitalRead(BUTTON_A)) {
    teletype = false; 
    display.invertDisplay(false);                           // un-invert display
    if (last>84) {                                         // check for underflow
      teletype = false;                                      // de-activate autoscrolling teletype
      displayRefresh(last -= 84);                            // decriment last character displayed pointer
      delay(200);                                            // quick debounce
     }
   }

  // scroll down with BUTTON B after checking for overflow
  if (! digitalRead(BUTTON_B)) {
    teletype = false; 
    display.invertDisplay(false);                           // un-invert display
    if (last<i2) {
      displayRefresh(last += 84); 
      delay(200);
    }
  }

  // re-enable auto-scrolling teletype mode with BUTTON C 
  if (! digitalRead(BUTTON_C)) {
    display.invertDisplay(true);
    teletype = true;
  }

  // the teletype function
  if (teletype) {
    while (last<i2) {
      last += 84;
    }
    displayRefresh(last);
  }
  

  // connect to a telnet client if none are connected
  if(!client.connected()) { 
    client = server.available(); // wait for it to connect
    return;
  }
  






    
} // end of main loop()



// refresh the display beginning from memory position specified via offset
void displayRefresh(int last) 
{
  
  display.clearDisplay();
  display.setCursor(0,0);
  for (int i=last-84 ; i<last ; i++) {
    if(buf2[i] != '\n') {
      display.print(buf2[i]);}
    else {
      display.write(' '); display.write(23);}
  }
  display.display();
} 






