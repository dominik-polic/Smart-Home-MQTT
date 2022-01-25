#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <Servo.h>
#define CLIENT_NAME "ESP8266_GARAGE_DOOR"


#define TOPIC_DOOR_LOCK "lock/door/garage"
#define TOPIC_DOOR_SWITCH "switch/door/garage"
#define TOPIC_DOOR_STATE "state/door/garage"
#define OPEN_MSG "open"
#define CLOSE_MSG "close"
#define TOGGLE_MSG "toggle"

#define DOOR_PIN 12 //D6
#define SENSOR_PIN 4 //D2

#define ACTIVE_TIME 20000

#define PUBLISH_DELAY 50
#define DEBUG true

#define OPEN_STATE LOW


const char* ssid = "Garaza WiFi";
const char* password = "12345678";
const char* mqtt_server = "192.168.1.17";

Servo servo;
WiFiClient espClient;
PubSubClient client(espClient);

char msg[50];
int currentangle=0;

boolean lastSensor = false;

unsigned long lastActivation = 0;


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  dPrintln("");
  dPrint("Connecting to ");
  dPrintln(ssid);

  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    dPrint(".");
  }

  dPrintln("");
  dPrintln("WiFi connected");
  dPrintln("IP address: ");
  dPrintln(WiFi.localIP().toString());
}

void callback(char* topic_c, byte* payload_c, unsigned int length) {
  dPrint("Message arrived [");
  dPrint(topic_c);
  dPrint("] ");
  String payload="";
  String topic=topic_c;
  for (int i = 0; i < length; i++) {
    payload+=(char)payload_c[i];
  }
  dPrintln("");
  dPrintln("Processed: " + payload + ", topic:"+topic);

  if(topic==TOPIC_DOOR_LOCK){
    if(payload==OPEN_MSG)
      openDoor();
    else if(payload==CLOSE_MSG)
      closeDoor();
    else if(payload == TOGGLE_MSG)
      toggleDoorOffline();

    
  }


}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    dPrint("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CLIENT_NAME)) {
      dPrintln("connected");
      //Subscribe to topics
      client.subscribe(TOPIC_DOOR_LOCK,0);       
      
    } else {
      dPrint("failed, rc=");
      dPrint((String)client.state());
      dPrintln(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      unsigned long startTime=millis();
      while(millis()<startTime+5000){
        ArduinoOTA.handle(); 
      }
    }
  }
}

void dPrint(String text){
  if(DEBUG) Serial.print(text);
}

void dPrintln(String text){
  if(DEBUG) Serial.println(text);
}

void startOTAServer(){
  ArduinoOTA.setHostname(CLIENT_NAME);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    dPrintln("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    dPrintln("END OTA!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    dPrintln("Progress: " + (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    dPrintln("Error[%u]: " + error);
    if (error == OTA_AUTH_ERROR) dPrintln("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) dPrintln("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) dPrintln("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) dPrintln("Receive Failed");
    else if (error == OTA_END_ERROR) dPrintln("End Failed");
  });
  ArduinoOTA.begin();
  
}


void setup() {
  pinMode(DOOR_PIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(DOOR_PIN, LOW);
  pinMode(SENSOR_PIN,INPUT_PULLUP);
  if(DEBUG) Serial.begin(115200);
  setup_wifi();
  startOTAServer();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  //Don't do this, since it shakes the actual motor real quick on reset, happen a lot...
  //servo.attach(SERVO_PIN); //3(RX)
  //servo.write(90);
  //servo.detach();
}



void loop() {
  //debugIt();
  if (!client.connected()) {
    reconnect();
  }

  if(digitalRead(SENSOR_PIN) != lastSensor){
    lastSensor = digitalRead(SENSOR_PIN);
    client.publish(TOPIC_DOOR_STATE,(lastSensor == OPEN_STATE)?OPEN_MSG:CLOSE_MSG);   
  }
  
  client.loop();  
  ArduinoOTA.handle();    
}


void openDoor(){  
  if(digitalRead(SENSOR_PIN) != OPEN_STATE){
    if(!performCheck()) return;
    digitalWrite(DOOR_PIN, HIGH);
    delay(500);
    digitalWrite(DOOR_PIN, LOW);
    //client.publish(TOPIC_DOOR_STATE,OPEN_MSG);   
  }  
}

void closeDoor(){  
  if(digitalRead(SENSOR_PIN) == OPEN_STATE){
    if(!performCheck()) return;
    digitalWrite(DOOR_PIN, HIGH);
    delay(500);
    digitalWrite(DOOR_PIN, LOW);
    //client.publish(TOPIC_DOOR_STATE,CLOSE_MSG);   
  }  
}


void toggleDoorOffline(){
  digitalWrite(DOOR_PIN, HIGH);
  delay(500);
  digitalWrite(DOOR_PIN, LOW);
}

boolean performCheck(){
  if(millis()<(lastActivation+ACTIVE_TIME)){
    return false;
  }else{
    lastActivation = millis();
    return true;
  }
}


void debugIt(){
  //boolean temp1=digitalRead(SW1);
  //boolean temp2=digitalRead(SW2);
  //long currentTime=millis();
  //client.publish(TOPIC_SW1,ON_MSG);
  
}
