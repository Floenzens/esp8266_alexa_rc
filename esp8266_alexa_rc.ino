#include <ESP8266WiFi.h>                //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>                //https://github.com/tzapu/WiFiManager

#include <RCSwitch.h>                   // Send commands to RCSwitch
#include "fauxmoESP.h"                  // https://bitbucket.org/xoseperez/fauxmoesp Alexa Support for ESP8266


//for LED status
#include <Ticker.h>
Ticker ticker;

fauxmoESP alexawifi;                    // alexa Support by fauxmoESP
RCSwitch mySwitch = RCSwitch();         // initialise RC Support

//+=============================================================================
// Toggle state
//
void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED); // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);    // set pin to the opposite state
}

//+=============================================================================
// Turn off the Led after timeout
//
void disableLed()
{
  Serial.println("Turning off the LED to save power.");
  digitalWrite(BUILTIN_LED, HIGH);      // Shut down the LED
  ticker.detach();                      // Stopping the ticker
}

//+=============================================================================
// Gets called when WiFiManager enters configuration mode
//
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
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
// Send RC commands to devices
//
void requestRcSwitch(uint8_t device_id, const char * device_name, bool state) {
  Serial.print("Gerät: "); Serial.println(device_name);
  Serial.print("Status: ");

  if (state) {

    if(device_id == 0) {
    Serial.println("AN");

//    mySwitch.send(12152610, 24);
    // Sending RC command to more than one device
    mySwitch.switchOn("11111", "10000"); // Schalter A
    delay(50);
    mySwitch.switchOn("11111", "01000"); // Schalter B
    delay(50);
    mySwitch.switchOn("11111", "00100"); // Schalter C
    delay(50);
    mySwitch.switchOn("11111", "00010"); // Schalter D
    delay(50);
    }


  //  if(device_id == 1) {
  //  Serial.println("AN");

  //  mySwitch.switchOn("11111", "00001");
  //  delay(50);
  //  }

  }

  else {

    if(device_id == 0) {
    Serial.println("AUS");

    //mySwitch.send(11855607, 24);
    mySwitch.switchOff("11111", "10000"); // Schalter A
    delay(50);
    mySwitch.switchOff("11111", "01000"); // Schalter B
    delay(50);
    mySwitch.switchOff("11111", "00100"); // Schalter C
    delay(50);
    mySwitch.switchOff("11111", "00010"); // Schalter D
    delay(50);
    }

//    if(device_id == 1) {
//    Serial.println("OFF");

//    mySwitch.switchOff("11111", "00001");
//    delay(50);
//    }
  ticker.attach(0.5, disableLed);
  }

}

//+=============================================================================
// Setup web server and RCSwitch
//
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("");
  Serial.println("ESP8266 IR Controller");
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  // Reset device if lost wifi Connection
  WiFi.onStationModeDisconnected(&lostWifiCallback);

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP().toString());
  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);

  // below RC specific

  mySwitch.enableTransmit(5);  //D1 am NodeMCU

  // Optional set protocol (default is 1, will work for most outlets)
  mySwitch.setProtocol(1);

  // Optional set pulse length.
  mySwitch.setPulseLength(315);           // good for my switch

  // Optional set number of transmission repetitions.
  //mySwitch.setRepeatTransmit(15);

// Add some devices
  alexawifi.addDevice("Licht"); //ID 0
  alexawifi.onMessage(requestRcSwitch);

  ticker.attach(0.5, disableLed);
}

void loop() {
  // put your main code here, to run repeatedly:
  alexawifi.handle();
  //delay(200);
}
