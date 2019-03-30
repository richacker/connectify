
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//MQTT topic for Thingify
#define TOPIC  "team00"

int lasta = -32676;
unsigned long lastSend = 0;

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "54.152.151.227";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);


long now = millis();
long lastMeasure = 0;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() 
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  if(topic==TOPIC){
      Serial.print("Changing Relay 1 to ");

      //This one is for receiving the string from Thingify for Buzzer
      if(messageTemp == "sw1_on"){
        digitalWrite(D1, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "sw1_off"){
        digitalWrite(D1, LOW);
        Serial.print("Off");
      }
      
      //This one is for receiving the string from Thingify for Relay 1
      else if(messageTemp == "sw2_on"){
        digitalWrite(D2, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "sw2_off"){
        digitalWrite(D2, LOW);
        Serial.print("Off");
      }

      //This one is for receiving the string from Thingify for Relay 2
      else if(messageTemp == "sw3_on"){
        digitalWrite(D3, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "sw3_off"){
        digitalWrite(D3, LOW);
        Serial.print("Off");
      }
  }
  Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe(TOPIC);
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() 
{
  pinMode(D4, INPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
   client.connect("ESP8266Client","thingify","T#i_@i_2018");
   client.subscribe(TOPIC);

}
int di_1;
uint8_t current_di;
uint8_t last_di=10;
char publish_buffer[5];
// For this project, you don't need to change anything in the loop function. Basically it ensures that you ESP is connected to your broker
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client","thingify","T#i_@i_2018");
    di_1= digitalRead(D4);
    current_di=di_1;
    
  if (client.connected()) {
    if(last_di != current_di)
    {
      sprintf(publish_buffer,"%i",current_di);
      client.publish("Thingify", publish_buffer,strlen(publish_buffer));
    memset(publish_buffer,0,5);
    last_di=current_di;
    }
  }  

  
}
