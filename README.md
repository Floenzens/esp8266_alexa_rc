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
![NodeMCU wiring](https://user-images.githubusercontent.com/1045651/34310934-91862f56-e75a-11e7-9a65-d8287ce97840.jpg)
![344 mhz module](https://user-images.githubusercontent.com/1045651/34310933-916863cc-e75a-11e7-8296-e150f3cb3199.jpg)

## Used libraries
* FauxmoESP https://bitbucket.org/xoseperez/fauxmoesp.git
* WiFiManager https://github.com/tzapu/WiFiManager
* RCSwitch https://github.com/sui77/rc-switch

## inspired by
https://youtu.be/48RW4JHMXUA
https://github.com/mdhiggins/ESP8266-HTTP-IR-Blaster
