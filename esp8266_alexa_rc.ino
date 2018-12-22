#include <ESP8266WiFi.h>                //https://github.com/esp8266/Arduino

//needed for library WiFiManager
//#include <DNSServer.h>
//#include <ESP8266WebServer.h>

// TODO: WiFiManager wieder nutzen
//#include <WiFiManager.h>                // https://github.com/tzapu/WiFiManager

#include <RCSwitch.h>                   // Send commands to RCSwitch https://github.com/sui77/rc-switch
#include "fauxmoESP.h"                  // https://bitbucket.org/xoseperez/fauxmoesp Alexa Support for ESP8266

// Rename the credentials.sample.h file to credentials.h and 
// edit it according to your router configuration
#include "credentials.h"

//for LED status
#include <Ticker.h>
Ticker ticker;

fauxmoESP fauxmo;                       // alexa Support by fauxmoESP
RCSwitch mySwitch = RCSwitch();         // initialise RC Support



// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE     115200

#define ID_SOCKET_A           "Schalter A"
#define ID_SOCKET_B           "Schalter B"
#define ID_SOCKET_C           "Schalter C"
#define ID_SOCKET_D           "Schalter D"
#define ID_SOCKET_ALL         "Licht"



//+=============================================================================
// Toggle state
//
void tick() {
  
  //toggle state
  int state = digitalRead(LED_BUILTIN); // get the current state of GPIO1 pin
  digitalWrite(LED_BUILTIN, !state);    // set pin to the opposite state

}





//+=============================================================================
// Turn off the Led after timeout
//
void disableLed() {
  
  Serial.println("Turning off the LED to save power.");
  digitalWrite(LED_BUILTIN, HIGH);      // Shut down the LED
  ticker.detach();                      // Stopping the ticker

}



//+=============================================================================
// Send RC commands to devices
//

void requestRcSwitch(uint8_t device_id, const char * device_name, bool state) {
    Serial.print("Device: "); Serial.println(device_name);
    Serial.print("Status: ");
  
    if (state) {
    
        Serial.println("ON");
    
        if(device_id == 0) {            
            mySwitch.switchOn("11111", "10000"); // Socket A
        } else if (device_id == 1) {
            mySwitch.switchOn("11111", "01000"); // Socket B
        } else if (device_id == 2) {
            mySwitch.switchOn("11111", "00100"); // Socket C
        } else if (device_id == 3) {
            mySwitch.switchOn("11111", "00010"); // Socket D  

        // Sending RC command to more than one device at once
        } else if (device_id == 4) {
            mySwitch.switchOn("11111", "10000"); // Socket A
            delay(50);
            mySwitch.switchOn("11111", "01000"); // Socket B
            delay(50);
            mySwitch.switchOn("11111", "00100"); // Socket C
            delay(50);
            mySwitch.switchOn("11111", "00010"); // Socket D  
            delay(50);                
        }
    
    } else {
    
        Serial.println("OFF");

        if(device_id == 0) {            
            mySwitch.switchOff("11111", "10000"); // Socket A
        } else if (device_id == 1) {
            mySwitch.switchOff("11111", "01000"); // Socket B
        } else if (device_id == 2) {
            mySwitch.switchOff("11111", "00100"); // Socket C
        } else if (device_id == 3) {
            mySwitch.switchOff("11111", "00010"); // Socket D
            
        // Sending RC command to more than one device at once
        } else if (device_id == 4) {
            mySwitch.switchOff("11111", "10000"); // Socket A
            delay(50);
            mySwitch.switchOff("11111", "01000"); // Socket B
            delay(50);
            mySwitch.switchOff("11111", "00100"); // Socket C
            delay(50);
            mySwitch.switchOff("11111", "00010"); // Socket D  
            delay(50);                
        }

        
    }

    delay(50);

   // ticker.attach(0.5, disableLed);
    
}


// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {
  
    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}


//+=============================================================================
// Gets called when device loses connection to the accesspoint
//

void lostWifiCallback (const WiFiEventStationModeDisconnected& evt) {
    
    Serial.println("Lost Wifi");
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);

}


//+=============================================================================
// Setup web server and RCSwitch
//
void setup() {

    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println("ESP8266 IR Controller");
    
    //set led pin as output
    pinMode(LED_BUILTIN, OUTPUT);
    
    // start ticker with 0.5 because we start in AP mode and try to connect
    ticker.attach(0.6, tick);
  
    
    //keep LED on
    digitalWrite(LED_BUILTIN, LOW);


    wifiSetup();

 
    // Reset device if lost wifi Connection
    WiFi.onStationModeDisconnected(&lostWifiCallback);
  
    ticker.detach();

    
    // below RC specific
    mySwitch.enableTransmit(5);  //D1 on NodeMCU (=GPIO5)

    // Optional set protocol (default is 1, will work for most outlets)
    mySwitch.setProtocol(1);

    // Optional set pulse length.
    mySwitch.setPulseLength(315);           // good for the sockets I am using
  
    // Optional set number of transmission repetitions.
    //mySwitch.setRepeatTransmit(15);

    // Add some devices
    fauxmo.createServer(true); // set to false, if you want to use an external webserver instead of fauxmo build in one
    fauxmo.addDevice(ID_SOCKET_A); 
    fauxmo.addDevice(ID_SOCKET_B); 
    fauxmo.addDevice(ID_SOCKET_C); 
    fauxmo.addDevice(ID_SOCKET_D);
    fauxmo.addDevice(ID_SOCKET_ALL); 
    
  
    fauxmo.setPort(80); // required for gen3 devices
    fauxmo.enable(true);
    // fauxmo.onMessage(requestRcSwitch);


    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        // Callback when a command from Alexa is received. 
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
        // Otherwise comparing the device_name is safer.

        if (strcmp(device_name, ID_SOCKET_A)==0) {
            requestRcSwitch(0, ID_SOCKET_A, state);
        } else if (strcmp(device_name, ID_SOCKET_B)==0) {
            requestRcSwitch(1, ID_SOCKET_B, state);
        } else if (strcmp(device_name, ID_SOCKET_C)==0) {
            requestRcSwitch(2, ID_SOCKET_C, state);
        } else if (strcmp(device_name, ID_SOCKET_D)==0) {
            requestRcSwitch(3, ID_SOCKET_D, state);
        } else if (strcmp(device_name, ID_SOCKET_ALL)==0) {
            requestRcSwitch(4, ID_SOCKET_ALL, state);
        }

    });

    ticker.attach(0.5, disableLed);

}

void loop() {

  // put your main code here, to run repeatedly:
  fauxmo.handle();
  //delay(200);

}
