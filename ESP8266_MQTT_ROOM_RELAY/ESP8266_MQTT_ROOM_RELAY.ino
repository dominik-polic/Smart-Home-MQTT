#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <Servo.h>
#define CLIENT_NAME "ESP8266_PANEL_DOMINIK"
#define SERIAL_TIMEOUT 1000

#define TOPIC_LCD "panel/dominik/lcd"
#define TOPIC_RGB_RED "panel/dominik/rgb/red"
#define TOPIC_RGB_GREEN "panel/dominik/rgb/green"
#define TOPIC_RGB_BLUE "panel/dominik/rgb/blue"
#define TOPIC_GATE_1 "panel/dominik/gate/1"
#define TOPIC_GATE_2 "panel/dominik/gate/2"
#define TOPIC_BELL_SPEAKER "panel/dominik/bell"
#define TOPIC_LIGHT "lights/dominik/1"
#define TOPIC_DOOR "lock/door/dominik"
#define TOPIC_SERVO "panel/dominik/coolkick"
#define TOPIC_LED_L1 "panel/dominik/led/L1"
#define TOPIC_LED_L2 "panel/dominik/led/L2"
#define TOPIC_LED_L3 "panel/dominik/led/L3"
#define TOPIC_LED_L4 "panel/dominik/led/L4"
#define TOPIC_LED_L5 "panel/dominik/led/L5"
#define TOPIC_LED_R1 "panel/dominik/led/R1"
#define TOPIC_LED_R2 "panel/dominik/led/R2"
#define TOPIC_LED_R3 "panel/dominik/led/R3"
#define TOPIC_LED_R4 "panel/dominik/led/R4"
#define TOPIC_LED_R5 "panel/dominik/led/R5"

#define PUBLISH_DELAY 50
#define DEBUG false   //DON'T TOUCH, SERIAL USED FOR COMMUNICATION WITH AT2560


const char* ssid = "PoliNET";
const char* password = "12345678";
const char* mqtt_server = "192.168.1.30";

WiFiClient espClient;
PubSubClient client(espClient);

char msg[50];

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

  String message="%"+topic+"|"+payload+"&";
  Serial.print(message);

 


}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    dPrint("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CLIENT_NAME,"dominik","dominik325")) {
      dPrintln("connected");
      //Subscribe to topics
      client.subscribe(TOPIC_RGB_RED);
      client.subscribe(TOPIC_RGB_GREEN);
      client.subscribe(TOPIC_RGB_BLUE);
      client.subscribe(TOPIC_GATE_1);
      client.subscribe(TOPIC_GATE_2);
      client.subscribe(TOPIC_BELL_SPEAKER);
      client.subscribe(TOPIC_LIGHT);
      client.subscribe(TOPIC_DOOR);
      client.subscribe(TOPIC_SERVO);
      client.subscribe(TOPIC_LED_L1);
      client.subscribe(TOPIC_LED_L2);
      client.subscribe(TOPIC_LED_L3);
      client.subscribe(TOPIC_LED_L4);
      client.subscribe(TOPIC_LED_L5);
      client.subscribe(TOPIC_LED_R1);
      client.subscribe(TOPIC_LED_R2);
      client.subscribe(TOPIC_LED_R3);
      client.subscribe(TOPIC_LED_R4);
      client.subscribe(TOPIC_LED_R5);      
      client.subscribe(TOPIC_LCD);
      
    } else {
      dPrint("failed, rc=");
      dPrint((String)client.state());
      dPrintln(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      for(int i=0;i<50;i++){
        delay(100);
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
  //if(DEBUG) Serial.begin(115200);
  Serial.begin(115200);
  setup_wifi();
  startOTAServer();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  checkSerial();
  
  
  client.loop();  
  ArduinoOTA.handle();    
}


void checkSerial(){
  if(Serial.available()){
      unsigned long startTime=millis();
      char cmessage[100]="";
      char cpayload[100]="";
      char ctopic[100]="";
      char lastChar='.';
      int mlength=0; 
      int plength=0;
      int tlength=0;    
      boolean isPayload=false; 
      while(lastChar!='&'&&millis()<startTime+SERIAL_TIMEOUT){
        while(!Serial.available()&&millis()<startTime+SERIAL_TIMEOUT);
        if(millis()>startTime+SERIAL_TIMEOUT)
          break;
        lastChar=Serial.read();
        cmessage[mlength]=lastChar;
        mlength++;      
        if(lastChar=='|')
          isPayload=true;
        if(lastChar!='&'&&lastChar!='|'&&lastChar!='%'){
          if(isPayload){
            cpayload[plength]=lastChar;
            plength++;
          }else{
            ctopic[tlength]=lastChar;
            tlength++;
          }
        }
      }  
      ctopic[tlength]=0;
      cpayload[plength]=0;
      cmessage[mlength]=0;
      
      client.publish(ctopic,cpayload);
    
  }
}
