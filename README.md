# esp8266_alexa_rc
Sending RC Commands with Alexa

Purpose of this project is to send voice controlled RC commands using different existing libraries, Amazon Echo and NodeMCU V2 (Amica)

## How it works
Flash the esp8266_alexa_rc.ino to your NodeMCU V2, connect your NodeMCU to your wifi using the WifiManager, discover new devices with your Alexa device and say "Alexa, Licht an" (or whatever you changed the command to).


## What you need
* NodeMCU V2 - I am using an Amica device
* Breadboard
* some male jumper cables
* 433 Mhz sender
* RC Sockets

## Wiring
* Connect 433 mhz modules VCC to NodeMCU VCC. As I am using NodeMCU V2 I am using VIN which should provide 5V as described here: https://github.com/opendata-stuttgart/meta/wiki/Pinouts-NodeMCU-v2,-v3
* Connect 433 mhz modules GND to NodeMCU GND
* Connect 433 mhz modules Data to NodeMCU GPIO Pin. I am GPIO5 which is D1 on NodeMCU

![NodeMCU wiring](https://user-images.githubusercontent.com/1045651/34310934-91862f56-e75a-11e7-9a65-d8287ce97840.jpg)
![344 mhz module](https://user-images.githubusercontent.com/1045651/34310933-916863cc-e75a-11e7-8296-e150f3cb3199.jpg)

## Set up everything
* change `alexawifi.addDevice("Licht"); //ID 0` to the desired Alexa command. Here I am using the german word `Licht`
* flash esp8266_alexa_rc.ino to NodeMCU (using Arduino IDE)
* configure `WifiManager` (see https://github.com/tzapu/WiFiManager for details)
  * connect to the wifi, your esp8266/NodeMCU newly established
  * provide your wifi credentials (will be stored in NodeMCUs filesystem)
  * connect to your "normal" wifi
* request Alexa to find new devices (e.g. by using the Alexa App)
* say "Alexa, Licht an" (if you use a german configured Alexa. Otherwise probably "Alexa, light on" or something comparable)
* have fun, switching your sockets on and off

## Used libraries
* FauxmoESP https://bitbucket.org/xoseperez/fauxmoesp.git
* WiFiManager https://github.com/tzapu/WiFiManager
* RCSwitch https://github.com/sui77/rc-switch

## inspired by
https://youtu.be/48RW4JHMXUA

https://github.com/mdhiggins/ESP8266-HTTP-IR-Blaster
