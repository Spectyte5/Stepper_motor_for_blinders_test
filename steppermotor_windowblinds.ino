#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <Stepper.h>

// -------------------------------------------------------------------------------------------------------------
const char* SSID = "xxxxxxxx";
const char* PASSWORD = "xxxxxxxx";

const char* MQTT_SERVER = "xxxxxxxx";
const char* MQTT_CLIENT = "esp32";
const char* MQTT_TOPIC_TEMP = "esp32/esp_response";
const char* MQTT_TOPIC_CMD = "esp32/webserver";
const int  MQTT_PORT = 1883;  

const char* MQTT_USERNAME = "esp32";
const char* MQTT_PASSWORD = "test";

const int CONN_ATTEMPT_LIMIT = 100;
char* COMMAND_MSG;
unsigned COMMAND_LEN = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
// -------------------------------------------------------------------------------------------------------------

void callback(char* topic, byte* message, unsigned int msgLength)
{
  // Here check for message
  if (String(topic) == MQTT_TOPIC_CMD){

    Serial.println("<CMD>");
    Serial.println(message[0]);
    Serial.println("</CMD>");
    
    COMMAND_MSG = (char*)message;
    COMMAND_LEN = msgLength;
  }
}
// -------------------------------------------------------------------------------------------------------------
void reconnect()
{
  Serial.print("Starting connection process...\n");
  while (!client.connected()) 
  {
    delay(5000);
    Serial.print("Attempting a connection [5s]\n");
    if (client.connect(MQTT_CLIENT , MQTT_USERNAME, MQTT_PASSWORD)) {
      client.subscribe(MQTT_TOPIC_TEMP);
      client.subscribe(MQTT_TOPIC_CMD);
    }
    else {
      Serial.println(client.state());
    }
    
  }
  Serial.print("Connection established sucessfully \n");
}


// -------------------------------------------------------------------------------------------------------------
void initWifi()
{
  int attempt = 0;
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    if (attempt > CONN_ATTEMPT_LIMIT) {
      Serial.print("ERROR: CONNECTION ATTEMPT LIMIT EXCEEDED\n");
      break; 
    }
    delay(1000);
    attempt++;
  }
  Serial.print("CONNECTION SUCCESSFULL\n");
  
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);  
}



// -------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------

// Define number of steps per rotation:
int stepsperrev = 2048;
// Max levels for blinder (reaching 1 for delay of 10 ms and speed of 10rpms takes 6s) which means limit of 1 allows for 6 full revolutions
// we start at closed blinder:
const int upper_max = 2;
const int upper_min = 0;
// up, down, stop modes:
const int up_code=(int)'u'+(int)'p';
const int down_code=(int)'d'+(int)'n';
const int stop_code=(int)'s'+(int)'t';
int code=stop_code;
int steps=0;

// Wiring:
#define IN1 13 // Pin 13 to IN1 on the ULN2003 driver
#define IN2 12 // Pin 12 to IN2 on the ULN2003 driver
#define IN3 14 // Pin 14 to IN3 on the ULN2003 driver
#define IN4 27 // Pin 27 to IN4 on the ULN2003 driver

// Create stepper object called 'stepper_m', note the pin order:
Stepper stepper_m = Stepper(stepsperrev, IN1, IN2, IN3, IN4);

// -------------------------------------------------------------------------------------------------------------

void setup() {
  // Begin Serial communication at a baud rate of 115200:
  Serial.begin(115200);
   Serial.println("Init Begin");

  initWifi();

  Serial.println("Init Done");
}

void loop() {
   if (!client.connected()) {
    reconnect();
    }
    client.loop();

  
  //do not move at the start of the program
   stepsperrev = 0;
  
//read command:
String command(COMMAND_MSG);

Serial.print(command);

if (command == "UP"){
code = up_code;
}
if (command == "DOWN"){
code = down_code;
}
if (command == "STOP"){
code = stop_code;
}

//stop when max is reached
if (steps >= upper_max && code!=down_code){
    code = stop_code;
  }
if (steps <= upper_min && code!=up_code){
    code = stop_code;
  }

  switch (code){
   case up_code:
      steps++;
      stepsperrev = 2048;
     break;
     
     case down_code:
      steps--;
      stepsperrev = -2048;
      break;
        
     case stop_code:
     stepsperrev = 0;
     break;
     
  default:
  break;
 }
 stepper_m.step(stepsperrev);
    // Set the speed to 5 rpm:
  stepper_m.setSpeed(5);
} 
