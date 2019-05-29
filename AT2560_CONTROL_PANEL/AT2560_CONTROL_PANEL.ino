//-----LIBRARIES-----
#include <EEPROM.h>
#include <OneWire.h>                //Temperature sensor
#include <DallasTemperature.h>      //Temperature sensor
#include <Encoder.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

//-----CONSTANTS-----
#define DEBOUNCE_DELAY 300          //Delay for on-board buttons
#define GATE2_DELAY 15000          //Time to wait for gate2 to open/close
#define MAX_LIGHT_VAL 255          //Maximum light brightness (to prevent LEDs from dying)
#define POT_MIN 10                  //Min potvalue to compensate for imperfect pots
#define POT_MAX 1013               //Max pot value to compensate for imperfect pots
#define TEMP_UPDATE_INTERVAL 30000    //Temperature update interval
#define COOL_KICK_MAX_TIME 2500    //Maximum time to wait for kick to success
#define BACKLIGHT_DURATION 5000
#define EEPROM_GATE2_ADDR 0
#define T_UPDATE_INTERVAL 300000
#define POT_TOLERANCE 15
#define SERIAL_TIMEOUT 1000
#define BELL_BTN_TIME_MIN 65
#define BELL_BTN_TIME_MAX 75

//-----PIN DEFINITIONS-----
//Rotary encoder
#define TOPIC_RSW "panel/dominik/rsw"
#define PIN_RSW_1   45              //Input - Rotary switch mode 1
#define PIN_RSW_2   53              //Input - Rotary switch mode 2
#define PIN_RSW_3   51              //Input - Rotary switch mode 3
#define PIN_RSW_4   49              //Input - Rotary switch mode 4
#define PIN_RSW_5   47              //Input - Rotary switch mode 5
#define PIN_RSW_6   43              //Input - Rotary switch mode 6
//Toggle switches
#define TOPIC_SW_1 "panel/dominik/sw/1"
#define TOPIC_SW_2 "panel/dominik/sw/2"
#define TOPIC_SW_3 "panel/dominik/sw/3"
#define TOPIC_SW_4 "panel/dominik/sw/4"
#define TOPIC_SW_5 "panel/dominik/sw/5"
#define SW_ON_MSG "on"
#define SW_OFF_MSG "off"
#define PIN_SW_1 46                 //Input - Unassigned
#define PIN_SW_2 48                 //Input - Unassigned
#define PIN_SW_3 50                 //Input - RGB enable
#define PIN_SW_4 52                 //Input - Unassigned
#define PIN_SW_5 40                 //Input - RGB panel override
//Buttons
#define TOPIC_BTN_1 "panel/dominik/btn/1"
#define TOPIC_BTN_2 "panel/dominik/btn/2"
#define TOPIC_BTN_3 "panel/dominik/btn/3"
#define TOPIC_BTN_4 "panel/dominik/btn/4"
#define TOPIC_BTN_5 "panel/dominik/btn/5"
#define TOPIC_BTN_6 "panel/dominik/btn/6"
#define TOPIC_BTN_7 "panel/dominik/btn/7"
#define BTN_MSG "pressed"
#define PIN_BTN_1 37                //Input - Turn light on/off
#define PIN_BTN_2 39                //Input - Open gate1
#define PIN_BTN_3 41                //Input - Open/close gate2
#define PIN_BTN_4 35                //Input - Ring a bell
#define PIN_BTN_5 31                //Input - Enable RGB override
#define PIN_BTN_6 42                //Input - Reset Arduino
#define PIN_BTN_7 44                //Input - Unassigned
//Rotary Encoder
#define TOPIC_RE "panel/dominik/re"
#define TOPIC_BTN_RE "panel/dominik/btn/re"
#define PIN_RE_1 2                  //Input/Interrupt - Rotary encoder pin1
#define PIN_RE_2 A1                 //Input - Rotary encoder pin2
#define PIN_BTN_RE A13              //Input - Rotary encoder button
//Potentiometers (Analog Pins)
#define TOPIC_POT_1 "panel/dominik/pot/1"
#define TOPIC_POT_2 "panel/dominik/pot/2"
#define TOPIC_POT_3 "panel/dominik/pot/3"
#define TOPIC_POT_4 "panel/dominik/pot/4"
#define TOPIC_POT_5 "panel/dominik/pot/5"
#define TOPIC_POT_6 "panel/dominik/pot/6"
#define PIN_POT_1 8                 //Input/Analog - RGB brightness
#define PIN_POT_2 9                 //Input/Analog - RGB speed
#define PIN_POT_3 12                //Input/Analog - RGB red
#define PIN_POT_4 11                //Input/Analog - RGB green
#define PIN_POT_5 10                //Input/Analog - RGB blue
#define PIN_POT_6 A6                //Input/Analog - master light brightness pot
//LED pins
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
#define LED_ON_MSG "on"
#define LED_OFF_MSG "off"
#define PIN_LED_L1 13                //Output - L1 [POWER]
#define PIN_LED_R1 27                //Output - R1 [LIGHT]
#define PIN_LED_L2 23                //Output - L2 Unassigned
#define PIN_LED_R2 25                //Output - R2 Unassigned
#define PIN_LED_L3 28                //Output - L3 Unassigned
#define PIN_LED_R3 29                //Output - R3 Unassigned
#define PIN_LED_L4 30                //Output - L4 Unassigned
#define PIN_LED_R4 32                //Output - R4 Unassigned
#define PIN_LED_R5 34                //Output - R5 Unassigned
#define PIN_LED_L5 36                //Output - L5 [RGB_ENABLED]
#define TOPIC_TEMPERATURE "panel/dominik/temperature"
#define ONE_WIRE_BUS A15             //Input/Output - OneWire bus for temperature sensor
//External control
#define TOPIC_RGB_RED "panel/dominik/rgb/red"
#define TOPIC_RGB_GREEN "panel/dominik/rgb/green"
#define TOPIC_RGB_BLUE "panel/dominik/rgb/blue"
#define PIN_RGB_BLUE  9              //Output/PWM - RGB Blue
#define PIN_RGB_GREEN  10            //Output/PWM - RGB Green
#define PIN_RGB_RED  8               //Output/PWM - RGB Red 
#define TOPIC_GATE_1 "panel/dominik/gate/1"
#define TOPIC_GATE_2 "panel/dominik/gate/2"
#define GATE_2_CLOSE_MSG "close"
#define GATE_2_OPEN_MSG "open"
#define GATE_2_TOGGLE_MSG "toggle"
#define GATE_2_FORCE_MSG "force"
#define PIN_GATE_1 22                 //Output - gate1 control
#define PIN_GATE_2 24                 //Output - gate2 control
#define TOPIC_BTN_BELL "panel/dominik/btn/bell"
#define TOPIC_BELL_SPEAKER "panel/dominik/bell"
#define TOPIC_LIGHT "lights/dominik/1"
#define TOPIC_DOOR "lock/door/dominik"
#define DOOR_LOCK_MSG "lock"
#define DOOR_UNLOCK_MSG "unlock"
#define TOPIC_SERVO "panel/dominik/coolkick"
#define PIN_BTN_BELL 26              //Input - Bell outside
#define PIN_BELL_SPEAKER A14         //Output - Bell control
#define PIN_LIGHT 11                 //Output/PWM - Room light control
#define PIN_DOOR_UNLOCK 6            //Output - Signal to unlock room door
#define PIN_DOOR_LOCK 5              //Output - Signal to close room door
#define PIN_SERVO 7                  //Output/PWM - Servo

#define TOPIC_LCD "panel/dominik/lcd"
#define LCD_CLEAR "clear"
//-----Library intitializations-----
LiquidCrystal_I2C lcd(0x27, 16, 2);           //LCD initialization, 16*2 chars
Servo coolServo;                              //Servo initialization
Encoder myEnc(PIN_RE_1, PIN_RE_2);  //Encoder initialization
OneWire oneWire(ONE_WIRE_BUS);                //OneWire Initialization
DallasTemperature sensors(&oneWire);          //Temperature sensor initialization

//-----Status values-----
int backlightEnabled = 1;         //LCD backlight status
int rgbOverride = 0;              //RGB panel override status
int lastRotarySwitchStatus = 0;   //Last rotary switch mode
float currTempC = -1;             //Current temperature
byte gate2Open = 0;               //Gate2 open/closed status
long rotaryEncoderVal = 0;         //Rotary encoder selected option
long lastRotaryEncoderVal = 0;     //Rotary encoder last option

//-----Timer values-----
unsigned long lastGate2Time = 0;           //Timer to check if gate2 stopped moving
unsigned long lastTemp = 0;                //Time of thelast temperature update
unsigned long lastBacklight = 0;

//All other values
int lastRSW = 0;
unsigned long lastTimeSW[5] = {0, 0, 0, 0, 0};
boolean lastSW[5] = {HIGH, HIGH, HIGH, HIGH, HIGH};
unsigned long lastTimeBTN[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};;
boolean lastBTN[9] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
int lastPOT[6] = {0, 0, 0, 0, 0, 0};
unsigned long lastRSWTime = 0;

void setup() {
  //-----Initialize everything-----
  //Init LCD
  lcd.init();
  lcd.backlight();
  lcdPrint("Booting up...   ", 0); //Display message on LCD
  lcdPrint("                ", 1);
  //Init Servo
  coolServo.attach(PIN_SERVO);
  coolServo.write(0);
  coolServo.detach();
  //Init Temp Sensor
  sensors.begin();
  //Init ESP8266 Serial
  Serial3.begin(115200);
  //Init EEPROM values
  gate2Open = EEPROM.read(EEPROM_GATE2_ADDR);

  //-----Setting pin modes-----
  pinMode(PIN_RGB_BLUE, OUTPUT);
  pinMode(PIN_RGB_GREEN, OUTPUT);
  pinMode(PIN_RGB_RED, OUTPUT);
  pinMode(PIN_GATE_1, OUTPUT);
  pinMode(PIN_GATE_2, OUTPUT);
  pinMode(PIN_BELL_SPEAKER, OUTPUT);
  pinMode(PIN_BTN_BELL, INPUT_PULLUP);
  pinMode(PIN_LIGHT, OUTPUT);
  pinMode(PIN_BTN_1, INPUT_PULLUP);
  pinMode(PIN_BTN_2, INPUT_PULLUP);
  pinMode(PIN_BTN_3, INPUT_PULLUP);
  pinMode(PIN_BTN_4, INPUT_PULLUP);
  pinMode(PIN_BTN_5, INPUT_PULLUP);
  pinMode(PIN_BTN_6, INPUT_PULLUP);
  pinMode(PIN_BTN_7, INPUT_PULLUP);
  pinMode(PIN_RSW_1, INPUT_PULLUP);
  pinMode(PIN_RSW_2, INPUT_PULLUP);
  pinMode(PIN_RSW_3, INPUT_PULLUP);
  pinMode(PIN_RSW_4, INPUT_PULLUP);
  pinMode(PIN_RSW_5, INPUT_PULLUP);
  pinMode(PIN_RSW_6, INPUT_PULLUP);
  pinMode(PIN_SW_1, INPUT_PULLUP);
  pinMode(PIN_SW_2, INPUT_PULLUP);
  pinMode(PIN_SW_3, INPUT_PULLUP);
  pinMode(PIN_SW_4, INPUT_PULLUP);
  pinMode(PIN_SW_5, INPUT_PULLUP);
  pinMode(PIN_DOOR_LOCK, OUTPUT);
  pinMode(PIN_DOOR_UNLOCK, OUTPUT);

  pinMode(PIN_LED_L1, OUTPUT);
  pinMode(PIN_LED_R1, OUTPUT);
  pinMode(PIN_LED_L2, OUTPUT);
  pinMode(PIN_LED_R2, OUTPUT);
  pinMode(PIN_LED_L3, OUTPUT);
  pinMode(PIN_LED_R3, OUTPUT);
  pinMode(PIN_LED_L4, OUTPUT);
  pinMode(PIN_LED_R4, OUTPUT);
  pinMode(PIN_LED_R5, OUTPUT);
  pinMode(PIN_LED_L5, OUTPUT);

  pinMode(PIN_RE_1, INPUT_PULLUP);
  pinMode(PIN_RE_2, INPUT_PULLUP);
  pinMode(PIN_BTN_RE, INPUT_PULLUP);

  //Set initial states
  digitalWrite(PIN_BELL_SPEAKER, HIGH);
  digitalWrite(PIN_LED_L1, LOW);
  digitalWrite(PIN_LED_R1, LOW);
  digitalWrite(PIN_LED_L2, LOW);
  digitalWrite(PIN_LED_R2, LOW);
  digitalWrite(PIN_LED_L3, LOW);
  digitalWrite(PIN_LED_R3, LOW);
  digitalWrite(PIN_LED_L4, LOW);
  digitalWrite(PIN_LED_R4, LOW);
  digitalWrite(PIN_LED_R5, LOW);
  digitalWrite(PIN_LED_L5, LOW);
  digitalWrite(PIN_DOOR_LOCK, LOW);
  digitalWrite(PIN_DOOR_UNLOCK, LOW);

  //Finish booting up
  lcd.clear();
  lcdPrint("Setup finished!", 0); //Display success message on LCD

}

void loop() {
  checkESP();
  updateRSW();
  updatePOTS();
  updateSwitches();
  updateButtons();
  updateLCD();
  updateTemperature();
  updateEncoder();

}

void checkESP() {
  if (Serial3.available()) {
    unsigned long startTime = millis();
    char cmessage[100] = "";
    char cpayload[100] = "";
    char ctopic[100] = "";
    char lastChar = '.';
    int mlength = 0;
    int plength = 0;
    int tlength = 0;
    boolean isPayload = false;
    while (lastChar != '&' && millis() < startTime + SERIAL_TIMEOUT) {
      while (!Serial3.available() && millis() < startTime + SERIAL_TIMEOUT);
      if (millis() > startTime + SERIAL_TIMEOUT)
        break;
      lastChar = Serial3.read();
      cmessage[mlength] = lastChar;
      mlength++;
      if (lastChar == '|')
        isPayload = true;
      if (lastChar != '&' && lastChar != '|' && lastChar != '%') {
        if (isPayload) {
          cpayload[plength] = lastChar;
          plength++;
        } else {
          ctopic[tlength] = lastChar;
          tlength++;
        }
      }
    }
    ctopic[tlength] = 0;
    cpayload[plength] = 0;
    cmessage[mlength] = 0;

    String message = cmessage;
    String topic = ctopic;
    String payload = cpayload;



    if (topic == TOPIC_RGB_RED) {
      analogWrite(PIN_RGB_RED, payload.toInt());
    } else if (topic == TOPIC_RGB_GREEN) {
      analogWrite(PIN_RGB_GREEN, payload.toInt());
    } else if (topic == TOPIC_RGB_BLUE) {
      analogWrite(PIN_RGB_BLUE, payload.toInt());
    } else if (topic == TOPIC_GATE_1) {
      gate1Open();
    } else if (topic == TOPIC_GATE_2) {
      if (payload == GATE_2_TOGGLE_MSG)
        action_gate2Open(true, true);
      else if (payload == GATE_2_OPEN_MSG)
        action_gate2Open(true, false);
      else if (payload == GATE_2_CLOSE_MSG)
        action_gate2Open(false, false);
      else if (payload == GATE_2_FORCE_MSG) {
        digitalWrite(PIN_GATE_2, HIGH);
        delay(200);
        digitalWrite(PIN_GATE_2, LOW);
      }
    } else if (topic == TOPIC_BELL_SPEAKER) {
      bellRing();
    } else if (topic == TOPIC_LIGHT) {
      analogWrite(PIN_LIGHT, payload.toInt());
    } else if (topic == TOPIC_DOOR) {
      if (payload == DOOR_LOCK_MSG)
        lockRoomDoor();
      else if (payload == DOOR_UNLOCK_MSG)
        unlockRoomDoor();
    } else if (topic == TOPIC_SERVO) {
      coolKick();
    } else if (topic == TOPIC_LED_L1)
      digitalWrite(PIN_LED_L1, (payload == LED_ON_MSG) ? HIGH : LOW);
    else if (topic == TOPIC_LED_L2)
      digitalWrite(PIN_LED_L2, (payload == LED_ON_MSG) ? HIGH : LOW);
    else if (topic == TOPIC_LED_L3)
      digitalWrite(PIN_LED_L3, (payload == LED_ON_MSG) ? HIGH : LOW);
    else if (topic == TOPIC_LED_L4)
      digitalWrite(PIN_LED_L4, (payload == LED_ON_MSG) ? HIGH : LOW);
    else if (topic == TOPIC_LED_L5)
      digitalWrite(PIN_LED_L5, (payload == LED_ON_MSG) ? HIGH : LOW);
    else if (topic == TOPIC_LED_R1)
      digitalWrite(PIN_LED_R1, (payload == LED_ON_MSG) ? HIGH : LOW);
    else if (topic == TOPIC_LED_R2)
      digitalWrite(PIN_LED_R2, (payload == LED_ON_MSG) ? HIGH : LOW);
    else if (topic == TOPIC_LED_R3)
      digitalWrite(PIN_LED_R3, (payload == LED_ON_MSG) ? HIGH : LOW);
    else if (topic == TOPIC_LED_R4)
      digitalWrite(PIN_LED_R4, (payload == LED_ON_MSG) ? HIGH : LOW);
    else if (topic == TOPIC_LED_R5)
      digitalWrite(PIN_LED_R5, (payload == LED_ON_MSG) ? HIGH : LOW);
    else if (topic == TOPIC_LCD){
      if(payload==LCD_CLEAR)
        lcd.clear();
      else
        lcdPrint(payload,0);
      
    }else{
      //Unknown topic
    }

    }

}





//Subroutines
void publish(String topic, String payload) {
  String message = "%" + topic + "|" + payload + "&";
  Serial3.print(message);

}

void publish(String topic, double value) {
  publish(topic, (String)value);
}

void updateLCD() {
  //Turn backlight off after a certain amount of time
  if (millis() > lastBacklight + BACKLIGHT_DURATION && backlightEnabled == 1) {
    backlightEnabled = 0;
    lcd.noBacklight();
  }
}

void lcdWake() {   //Displays update on lcd instantly
  backlightEnabled = 1;
  lastBacklight = millis();
  lcd.backlight();
}

void lcdPrint(String text, int line) { //Prints a string onto LCD
  if (line != -1 && line != 2) lcdWake(); //Turn backlight on if needed
  if (line == -1) line = 0;
  if (line == 2) line = 1;
  
  
  lcd.setCursor(0, line);
  lcd.print(text + "                  ");
  if(text.length()>16){
    lcd.setCursor(0, line+1);
    text.remove(0,16);
    lcd.print(text + "                  ");
  }

}


void gate1Open() {	//Sends signal to open gate1
  //lcdPrint("Opening gate 1..",0);
  digitalWrite(PIN_GATE_1, HIGH);
  delay(200);
  digitalWrite(PIN_GATE_1, LOW);

}



void action_gate2Open(boolean state, boolean toggle) { //Opens or closes the gate
  if (toggle) {
    if (gate2Open == 0)
      publish(TOPIC_GATE_2, GATE_2_OPEN_MSG);
    else
      publish(TOPIC_GATE_2, GATE_2_CLOSE_MSG);
  }
  else if (millis() > lastGate2Time + GATE2_DELAY) {
    boolean change = false;
    if (state == true && gate2Open == 0) {
      gate2Open = 1;
      EEPROM.write(EEPROM_GATE2_ADDR, gate2Open);
      change = true;
      //lcdPrint("Opening gate 2   ",0);
    } else if (state == false && gate2Open == 1) {
      gate2Open = 0;
      EEPROM.write(EEPROM_GATE2_ADDR, gate2Open);
      change = true;
      //lcdPrint("Closing gate 2   ",0);
    } else {
      //lcdPrint("Gate 2 error!",0);
    }
    if (change == true) {
      lastGate2Time = millis();
      digitalWrite(PIN_GATE_2, HIGH);
      delay(200);
      digitalWrite(PIN_GATE_2, LOW);
    }
  }
}






void updateRSW() {
  if (millis() > lastRSWTime + DEBOUNCE_DELAY) {
    if (digitalRead(PIN_RSW_1) == LOW && lastRSW != 1) {
      lastRSWTime = millis();
      lastRSW = 1;
      publish(TOPIC_RSW, "1");
    } else if (digitalRead(PIN_RSW_2) == LOW && lastRSW != 2) {
      lastRSWTime = millis();
      lastRSW = 2;
      publish(TOPIC_RSW, "2");
    } else if (digitalRead(PIN_RSW_3) == LOW && lastRSW != 3) {
      lastRSWTime = millis();
      lastRSW = 3;
      publish(TOPIC_RSW, "3");
    } else if (digitalRead(PIN_RSW_4) == LOW && lastRSW != 4) {
      lastRSWTime = millis();
      lastRSW = 4;
      publish(TOPIC_RSW, "4");
    } else if (digitalRead(PIN_RSW_5) == LOW && lastRSW != 5) {
      lastRSWTime = millis();
      lastRSW = 5;
      publish(TOPIC_RSW, "5");
    } else if (digitalRead(PIN_RSW_6) == LOW && lastRSW != 6) {
      lastRSWTime = millis();
      lastRSW = 6;
      publish(TOPIC_RSW, "6");
    }
  }
}

void updatePOTS() {
  int currentPOT1 = analogRead(PIN_POT_1);
  int currentPOT2 = analogRead(PIN_POT_2);
  int currentPOT3 = analogRead(PIN_POT_3);
  int currentPOT4 = analogRead(PIN_POT_4);
  int currentPOT5 = analogRead(PIN_POT_5);
  int currentPOT6 = analogRead(PIN_POT_6);

  if (currentPOT1 > lastPOT[1 - 1] + POT_TOLERANCE || currentPOT1 < lastPOT[1 - 1] - POT_TOLERANCE) {
    lastPOT[1 - 1] = currentPOT1;
    publish(TOPIC_POT_1, currentPOT1);
  }
  if (currentPOT2 > lastPOT[2 - 1] + POT_TOLERANCE || currentPOT2 < lastPOT[2 - 1] - POT_TOLERANCE) {
    lastPOT[2 - 1] = currentPOT2;
    publish(TOPIC_POT_2, currentPOT2);
  }
  if (currentPOT3 > lastPOT[3 - 1] + POT_TOLERANCE || currentPOT3 < lastPOT[3 - 1] - POT_TOLERANCE) {
    lastPOT[3 - 1] = currentPOT3;
    publish(TOPIC_POT_3, currentPOT3);
  }
  if (currentPOT4 > lastPOT[4 - 1] + POT_TOLERANCE || currentPOT4 < lastPOT[4 - 1] - POT_TOLERANCE) {
    lastPOT[4 - 1] = currentPOT4;
    publish(TOPIC_POT_4, currentPOT4);
  }
  if (currentPOT5 > lastPOT[5 - 1] + POT_TOLERANCE || currentPOT5 < lastPOT[5 - 1] - POT_TOLERANCE) {
    lastPOT[5 - 1] = currentPOT5;
    publish(TOPIC_POT_5, currentPOT5);
  }
  if (currentPOT6 > lastPOT[6 - 1] + POT_TOLERANCE || currentPOT6 < lastPOT[6 - 1] - POT_TOLERANCE) {
    lastPOT[6 - 1] = currentPOT6;
    publish(TOPIC_POT_6, currentPOT6);
  }

}


void updateSwitches() {	//Checks for toggle switch updates
  boolean currentSW1 = digitalRead(PIN_SW_1);
  boolean currentSW2 = digitalRead(PIN_SW_2);
  boolean currentSW3 = digitalRead(PIN_SW_3);
  boolean currentSW4 = digitalRead(PIN_SW_4);
  boolean currentSW5 = digitalRead(PIN_SW_5);
  unsigned long currentTime = millis();

  if (currentSW1 != lastSW[1 - 1] && currentTime > lastTimeSW[1 - 1] + DEBOUNCE_DELAY) {
    lastSW[1 - 1] = currentSW1;
    lastTimeSW[1 - 1] = currentTime;
    if (currentSW1 == LOW)
      publish(TOPIC_SW_1, SW_ON_MSG);
    else
      publish(TOPIC_SW_1, SW_OFF_MSG);
  }

  if (currentSW2 != lastSW[2 - 1] && currentTime > lastTimeSW[2 - 1] + DEBOUNCE_DELAY) {
    lastSW[2 - 1] = currentSW2;
    lastTimeSW[2 - 1] = currentTime;
    if (currentSW2 == LOW)
      publish(TOPIC_SW_2, SW_ON_MSG);
    else
      publish(TOPIC_SW_2, SW_OFF_MSG);
  }

  if (currentSW3 != lastSW[3 - 1] && currentTime > lastTimeSW[3 - 1] + DEBOUNCE_DELAY) {
    lastSW[3 - 1] = currentSW3;
    lastTimeSW[3 - 1] = currentTime;
    if (currentSW3 == LOW)
      publish(TOPIC_SW_3, SW_ON_MSG);
    else
      publish(TOPIC_SW_3, SW_OFF_MSG);
  }

  if (currentSW4 != lastSW[4 - 1] && currentTime > lastTimeSW[4 - 1] + DEBOUNCE_DELAY) {
    lastSW[4 - 1] = currentSW4;
    lastTimeSW[4 - 1] = currentTime;
    if (currentSW4 == LOW)
      publish(TOPIC_SW_4, SW_ON_MSG);
    else
      publish(TOPIC_SW_4, SW_OFF_MSG);
  }

  if (currentSW5 != lastSW[5 - 1] && currentTime > lastTimeSW[5 - 1] + DEBOUNCE_DELAY) {
    lastSW[5 - 1] = currentSW5;
    lastTimeSW[5 - 1] = currentTime;
    if (currentSW5 == LOW)
      publish(TOPIC_SW_5, SW_ON_MSG);
    else
      publish(TOPIC_SW_5, SW_OFF_MSG);
  }


}



void updateButtons() {
  boolean currentButton1 = digitalRead(PIN_BTN_1);
  boolean currentButton2 = digitalRead(PIN_BTN_2);
  boolean currentButton3 = digitalRead(PIN_BTN_3);
  boolean currentButton4 = digitalRead(PIN_BTN_4);
  boolean currentButton5 = digitalRead(PIN_BTN_5);
  boolean currentButton6 = digitalRead(PIN_BTN_6);
  boolean currentButton7 = digitalRead(PIN_BTN_7);
  boolean currentButton8 = digitalRead(PIN_BTN_RE);
  boolean currentButton9 = digitalRead(PIN_BTN_BELL);
  unsigned long currentTime = millis();

  if (currentButton1 == HIGH && lastBTN[1 - 1] == LOW && currentTime > lastTimeBTN[1 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[1 - 1] = currentTime;
    lastBTN[1 - 1] = HIGH;
  }
  if (currentButton1 == LOW && lastBTN[1 - 1] == HIGH && currentTime > lastTimeBTN[1 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[1 - 1] = currentTime;
    lastBTN[1 - 1] = LOW;
    publish(TOPIC_BTN_1, BTN_MSG);
  }


  if (currentButton2 == HIGH && lastBTN[2 - 1] == LOW && currentTime > lastTimeBTN[2 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[2 - 1] = currentTime;
    lastBTN[2 - 1] = HIGH;
  }
  if (currentButton2 == LOW && lastBTN[2 - 1] == HIGH && currentTime > lastTimeBTN[2 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[2 - 1] = currentTime;
    lastBTN[2 - 1] = LOW;
    publish(TOPIC_BTN_2, BTN_MSG);
  }


  if (currentButton3 == HIGH && lastBTN[3 - 1] == LOW && currentTime > lastTimeBTN[3 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[3 - 1] = currentTime;
    lastBTN[3 - 1] = HIGH;
  }
  if (currentButton3 == LOW && lastBTN[3 - 1] == HIGH && currentTime > lastTimeBTN[3 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[3 - 1] = currentTime;
    lastBTN[3 - 1] = LOW;
    publish(TOPIC_BTN_3, BTN_MSG);
  }


  if (currentButton4 == HIGH && lastBTN[4 - 1] == LOW && currentTime > lastTimeBTN[4 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[4 - 1] = currentTime;
    lastBTN[4 - 1] = HIGH;
  }
  if (currentButton4 == LOW && lastBTN[4 - 1] == HIGH && currentTime > lastTimeBTN[4 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[4 - 1] = currentTime;
    lastBTN[4 - 1] = LOW;
    publish(TOPIC_BTN_4, BTN_MSG);
  }


  if (currentButton5 == HIGH && lastBTN[5 - 1] == LOW && currentTime > lastTimeBTN[5 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[5 - 1] = currentTime;
    lastBTN[5 - 1] = HIGH;
  }
  if (currentButton5 == LOW && lastBTN[5 - 1] == HIGH && currentTime > lastTimeBTN[5 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[5 - 1] = currentTime;
    lastBTN[5 - 1] = LOW;
    publish(TOPIC_BTN_5, BTN_MSG);
  }


  if (currentButton6 == HIGH && lastBTN[6 - 1] == LOW && currentTime > lastTimeBTN[6 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[6 - 1] = currentTime;
    lastBTN[6 - 1] = HIGH;
  }
  if (currentButton6 == LOW && lastBTN[6 - 1] == HIGH && currentTime > lastTimeBTN[6 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[6 - 1] = currentTime;
    lastBTN[6 - 1] = LOW;
    publish(TOPIC_BTN_6, BTN_MSG);
  }


  if (currentButton7 == HIGH && lastBTN[7 - 1] == LOW && currentTime > lastTimeBTN[7 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[7 - 1] = currentTime;
    lastBTN[7 - 1] = HIGH;
  }
  if (currentButton7 == LOW && lastBTN[7 - 1] == HIGH && currentTime > lastTimeBTN[7 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[7 - 1] = currentTime;
    lastBTN[7 - 1] = LOW;
    publish(TOPIC_BTN_7, BTN_MSG);
  }


  if (currentButton8 == HIGH && lastBTN[8 - 1] == LOW && currentTime > lastTimeBTN[8 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[8 - 1] = currentTime;
    lastBTN[8 - 1] = HIGH;
  }
  if (currentButton8 == LOW && lastBTN[8 - 1] == HIGH && currentTime > lastTimeBTN[8 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[8 - 1] = currentTime;
    lastBTN[8 - 1] = LOW;
    publish(TOPIC_BTN_RE, BTN_MSG);
  }



  if (currentButton9 == HIGH && lastBTN[9 - 1] == LOW && currentTime > lastTimeBTN[9 - 1] + DEBOUNCE_DELAY) {
    lastTimeBTN[9 - 1] = currentTime;
    lastBTN[9 - 1] = HIGH;
  }
  if (currentButton9 == LOW && lastBTN[9 - 1] == HIGH && currentTime > lastTimeBTN[9 - 1] + DEBOUNCE_DELAY*2) {
    lastTimeBTN[9 - 1] = currentTime;
    lastBTN[9 - 1] = LOW;      
    publish(TOPIC_BTN_BELL, BTN_MSG);
  }


}

void coolKick() {	//RGB Override
  if (digitalRead(PIN_SW_5) == LOW) {
    coolServo.attach(PIN_SERVO);
    coolServo.write(180);
    long temp_ms = millis();
    while (digitalRead(PIN_SW_5) == LOW && millis() < temp_ms + COOL_KICK_MAX_TIME);
    delay(80);
    coolServo.write(0);
    delay(200);
    coolServo.detach();
  }
}





void updateTemperature() {	//Gets update from the temp snesor
  if (millis() > (lastTemp + T_UPDATE_INTERVAL)) {
    lastTemp = millis();
    sensors.requestTemperatures();
    currTempC = sensors.getTempCByIndex(0);
    publish(TOPIC_TEMPERATURE , currTempC);

  }
}

void unlockRoomDoor() {	//Sends signal to unclock room door
  digitalWrite(PIN_DOOR_UNLOCK, HIGH);
  delay(1000);
  digitalWrite(PIN_DOOR_UNLOCK, LOW);
}

void bellRing() {	//Sends signal to ring a a bell
  digitalWrite(PIN_BELL_SPEAKER, LOW);
  delay(500);
  digitalWrite(PIN_BELL_SPEAKER, HIGH);
}

void lockRoomDoor() {	//Sends signal to lock room door
  digitalWrite(PIN_DOOR_LOCK, HIGH);
  delay(1000);
  digitalWrite(PIN_DOOR_LOCK, LOW);
}

void updateEncoder() {	//Checks if encoder status changed and process it
  long rotaryEncoderVal = myEnc.read() / 4;												//Update encoder value
  if (rotaryEncoderVal != lastRotaryEncoderVal) {									//If value has changed recently - update mode
    publish(TOPIC_RE, rotaryEncoderVal - lastRotaryEncoderVal);
    lastRotaryEncoderVal = rotaryEncoderVal;
  }
}
