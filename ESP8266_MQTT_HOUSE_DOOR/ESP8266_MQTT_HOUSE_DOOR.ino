#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <Servo.h>
#define CLIENT_NAME "ESP8266_HOUSE_DOOR"


#define TOPIC_DOOR_LOCK "lock/door/house"
#define TOPIC_DOOR_SWITCH "switch/door/house"
#define BELL_MSG "bell"
#define BUTTON_MSG "button"
#define PANEL_MSG "panel"
#define LOCK_MSG "lock"
#define UNLOCK_MSG "unlock"
#define TOGGLE_MSG "toggle"

#define SERVO_PIN 3
#define BUTTON_PIN 5
#define BELL_PIN 4
#define PANEL_PIN 13

#define LOCK_VALUE 0
#define UNLOCK_VALUE 180
#define LOCKING_TIME 1850
#define BELL_DEBOUNCE 60





#define PUBLISH_DELAY 50
#define DEBOUNCE_TIME 300
#define DEBUG false


const char* ssid = "PoliNET";
const char* password = "12345678";
const char* mqtt_server = "192.168.1.30";

Servo servo;
WiFiClient espClient;
PubSubClient client(espClient);

char msg[50];
unsigned long lastButtonTime=0;
unsigned long lastPanelTime=0;
unsigned long lastBellTime=0;
int currentangle=0;
boolean lastButton=LOW;
boolean lastPanel=LOW;
boolean lastBell=LOW;

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
    if(payload==LOCK_MSG)
      lockDoor();
    else if(payload==UNLOCK_MSG)
      unlockDoor();

    
  }


}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    dPrint("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CLIENT_NAME,"dominik","dominik325")) {
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
        updateButtonsOffline();
        ArduinoOTA.handle(); 
      }
    }
  }
  digitalWrite(LED_BUILTIN,HIGH);
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
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(BUTTON_PIN,INPUT_PULLUP);
  pinMode(PANEL_PIN,INPUT_PULLUP);
  pinMode(BELL_PIN,INPUT_PULLUP);
  if(DEBUG) Serial.begin(115200);
  setup_wifi();
  startOTAServer();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  //Don't do this, since it shakes the actual motor real quick on reset, whappen a lot...
  //servo.attach(SERVO_PIN); //3(RX)
  //servo.write(90);
  //servo.detach();
}



void loop() {
  //debugIt();
  if (!client.connected()) {
    reconnect();
  }

  updateButtons();
  
  
  
  client.loop();  
  ArduinoOTA.handle();    
}


void updateButtons(){
  boolean currentButton=digitalRead(BUTTON_PIN);
  boolean currentPanel=digitalRead(PANEL_PIN);
  boolean currentBell=digitalRead(BELL_PIN);
  unsigned long currentTime=millis();
  
  if(currentButton==HIGH&&lastButton==LOW&&currentTime>lastButtonTime+DEBOUNCE_TIME){
    lastButtonTime=currentTime;
    lastButton=HIGH;
  }  
  if(currentButton==LOW&&lastButton==HIGH&&currentTime>lastButtonTime+DEBOUNCE_TIME){
    lastButtonTime=currentTime;
    lastButton=LOW;
    delay(BELL_DEBOUNCE);
    if(digitalRead(BUTTON_PIN)==LOW)
      client.publish(TOPIC_DOOR_SWITCH,BUTTON_MSG);      
  }

  if(currentPanel==HIGH&&lastPanel==LOW&&currentTime>lastPanelTime+DEBOUNCE_TIME){
    lastPanelTime=currentTime;
    lastPanel=HIGH;
  }  
  if(currentPanel==LOW&&lastPanel==HIGH&&currentTime>lastPanelTime+DEBOUNCE_TIME){
    lastPanelTime=currentTime;
    lastPanel=LOW;
    delay(BELL_DEBOUNCE);
      if(digitalRead(PANEL_PIN)==LOW)
      client.publish(TOPIC_DOOR_SWITCH,PANEL_MSG);       
  }

  if(currentBell==HIGH&&lastBell==LOW&&currentTime>lastBellTime+DEBOUNCE_TIME){
    lastBellTime=currentTime;
    lastBell=HIGH;
  }  
  if(currentBell==LOW&&lastBell==HIGH&&currentTime>lastBellTime+DEBOUNCE_TIME){
    lastBellTime=currentTime;
    lastBell=LOW;
    delay(BELL_DEBOUNCE);
      if(digitalRead(BELL_PIN)==LOW)
      client.publish(TOPIC_DOOR_SWITCH,BELL_MSG);       
  }

  
}


void updateButtonsOffline(){
  boolean currentButton=digitalRead(BUTTON_PIN);
  boolean currentPanel=digitalRead(PANEL_PIN);
  boolean currentBell=digitalRead(BELL_PIN);
  unsigned long currentTime=millis();
  
  if(currentButton==HIGH&&lastButton==LOW&&currentTime>lastButtonTime+DEBOUNCE_TIME){
    lastButtonTime=currentTime;
    lastButton=HIGH;
  }  
  if(currentButton==LOW&&lastButton==HIGH&&currentTime>lastButtonTime+DEBOUNCE_TIME){
    lastButtonTime=currentTime;
    lastButton=LOW;
      unlockDoor();      
  }

  if(currentPanel==HIGH&&lastPanel==LOW&&currentTime>lastPanelTime+DEBOUNCE_TIME){
    lastPanelTime=currentTime;
    lastPanel=HIGH;
  }  
  if(currentPanel==LOW&&lastPanel==HIGH&&currentTime>lastPanelTime+DEBOUNCE_TIME){
    lastPanelTime=currentTime;
    lastPanel=LOW;
      unlockDoor();      
  }

  
}



void lockDoor(){
    servo.attach(SERVO_PIN);
    servo.write(LOCK_VALUE);
    delay(LOCKING_TIME);
    servo.detach();     
}

void unlockDoor(){
    servo.attach(SERVO_PIN);
    servo.write(UNLOCK_VALUE);
    delay(LOCKING_TIME);
    servo.detach();     
}



void debugIt(){
  //boolean temp1=digitalRead(SW1);
  //boolean temp2=digitalRead(SW2);
  //long currentTime=millis();
  //client.publish(TOPIC_SW1,ON_MSG);
  
}
