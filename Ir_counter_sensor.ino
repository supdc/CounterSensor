#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define LED_PIN D2
int irPin1=D2;
int irPin2=D3;
int count=0;
boolean state1 = true;
boolean state2 = true;
int countin=0;
int countout=0;
int i=1;
int total = 0;
int data[3];
int previousS1 = 0;
int previousS2 = 0;

const char* ssid = "Hh";
const char* password = "11223344";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_Client = "ir001";
const char* mqtt_username = "";
const char* mqtt_password = "";
WiFiClient espClient; 

PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;
char msg[100];
String DataString;

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
  if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) { //เชื่อมต่อกับ MQTT BROKER
    Serial.println("connected");
    client.subscribe("@msg/operator");
  }
  else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println("try again in 5 seconds");
    delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    message = message + char(payload[i]);
    
  }
  Serial.println(message);
  if(String(topic) == "@msg/operator") {
    if (message == "ON"){
      digitalWrite(LED_PIN,HIGH);
      //client.publish("@shadow/data/update", "{\"data\" : {\"led\" : \"on\"}}");
      Serial.println("LED ON"); }
    else if (message == "OFF") {
      digitalWrite(LED_PIN,LOW);
      //client.publish("@shadow/data/update", "{\"data\" : {\"led\" : \"off\"}}");
      Serial.println("LED OFF"); }
    } 
}
void setup() {
Serial.begin(115200);
pinMode(irPin1, INPUT);
pinMode(irPin2, INPUT);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); //เชื่อมต่อกับ WIFI
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); //เชื่อมต่อกับ WIFI สำเร็จ แสดง IP
  client.setServer(mqtt_server, mqtt_port); //กำหนด MQTT BROKER, PORT ที่ใช้
  client.setCallback(callback); //ตั้งค่าฟังก์ชันที่จะทำงานเมื่อมีข้อมูลเข้ามาผ่านการ Subscribe
  client.subscribe("@msg/operator");
}
String count1;
String count2;

void loop() {
  if (!digitalRead(irPin1) && i==1 && state1){
     
     delay(100);
     i++;
     state1 = false;
  }

   if (!digitalRead(irPin2) && i==2 &&   state2){
     Serial.println("Entering into room");
     
     delay(100);
     i = 1 ;
     countin++;
//     Serial.print("count in =");
     Serial.println(countin);
     count1 = String(countin);
     state2 = false;
     total = countin-countout;
//        Serial.print("total =");
       Serial.println(total);
  }

   if (!digitalRead(irPin2) && i==1 && state2 ){
     
     delay(100);
     i = 2 ;
     state2 = false;
  }

  if (!digitalRead(irPin1) && i==2 && state1 ){
     Serial.println("Exiting from room");
    
     delay(100);
     countout++;
       Serial.print("count out =");
       Serial.println(countout);
       count2=String(countout);
     i = 1;
     state1 = false;
     total = countin-countout;
//     Serial.print("total =");
       Serial.println(total);
     
  }  

    if (digitalRead(irPin1)){
     state1 = true;
    }

     if (digitalRead(irPin2)){
     state2 = true;
    }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
long now = millis();
int s1 = countin;
int s2 = countout;

if (s1 != previousS1 || s2 != previousS2) {  // check if either s1 or s2 has increased
  if (now - lastMsg > 1000) {  // send data every 1000 milliseconds (1 second)
    lastMsg = now;
    int total = s1 - s2;
    DataString = "{\"countin\":"+(String)s1+",\"countout\":"+(String)s2+",\"total\":"+(String)total+"}";
    DataString.toCharArray(msg, 100);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("ir001", msg);

    previousS1 = s1;  // update the previous values of s1 and s2
    previousS2 = s2;
  }
  delay(1);
}    

}
