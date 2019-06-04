# Smart-Home-MQTT

This is a new version of my smart home system, upgraded from my proprietary system to MQTT.


This is a personal project to connect devices around my house to MQTT server and make them accessible over the internet. 

## Basic concept
The basic concept is connecting various devies to an MQTT server over WiFi. MQTT devices are then arranged and programmed in NodeRED and connected to the remote Android APP and a web dashboard. Devices, also refered as nodes, are usually ESP8266 boards programmed in Arduino and embedded behind a light switch, inside a wall / power distribution box or inside a door lock. Each switch and end device are then subscribed (or publish to) a specific MQTT topic. NodeRed flows then proceess those signals and send them to end devices accordingly. You can also remotely see the current state of every device and update it according to your needs. 

### Basic diagram

![Image here](/docs/images/diagram-concept.jpg)





### Web interface

![Image here](/docs/images/nodered-dashboard.jpg)





### Example NodeRED flow

![Image here](/docs/images/nodered-flow.jpg)






### Example wiring

![Image here](/docs/images/electronics-1.jpg)





### Example in-house panel

![Image here](/docs/images/panel-1.jpg)




### A few APP screenshots

![Image here](/docs/images/app-4.jpg)



![Image here](/docs/images/app-5.jpg)



![Image here](/docs/images/app-1.jpg)




