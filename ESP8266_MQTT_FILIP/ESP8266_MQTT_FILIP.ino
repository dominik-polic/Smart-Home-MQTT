#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#define REL1 4
#define TOPIC_REL1 "filip/garaza"
#define CLIENT_NAME "FILIP_GARAZA"
#define TRIG_MSG "trigger"
#define PUBLISH_DELAY 50
#define ERROR_DELAY 10
#define HOLD_DELAY 500
#define LOOP_DELAY 100;

#define DEBUG false



const char* ssid = "A1 WLAN_3D2935";
const char* password = "CDC2ECDA3C";
const char* mqtt_server = "polichousecontrol.ddns.net";

WiFiClient espClient;
PubSubClient client(espClient);

char msg[50];
long lastTime1=0;
long lastPublish=0;
boolean lastActive1=HIGH;

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

  if(topic.equals(TOPIC_REL1)){
    if(payload.equals(TRIG_MSG)){
      digitalWrite(REL1,HIGH);
      delay(HOLD_DELAY);
      digitalWrite(REL1,LOW);
      dPrintln("REL1 on");
    }
  }
client.loop();  
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    dPrint("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CLIENT_NAME,"dominik","dominik325")) {
      dPrintln("connected");
      // Once connected, publish an announcement...

      client.subscribe(TOPIC_REL1,0);     
    } else {
      dPrint("failed, rc=");
      dPrint((String)client.state());
      dPrintln(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  //digitalWrite(LED_BUILTIN,LOW);
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
  //pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  //digitalWrite(LED_BUILTIN, HIGH);
  pinMode(REL1, OUTPUT);
  if(DEBUG) Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  startOTAServer();
}



void loop() {
  delay("LOOP_DELAY");

  //debugIt();
  if (!client.connected()) {
    reconnect();
  }
  
  
  client.loop();  
  ArduinoOTA.handle();    

}

void debugIt(){
  //boolean temp1=digitalRead(SW1);
  //boolean temp2=digitalRead(SW2);
  //long currentTime=millis();

  
}
