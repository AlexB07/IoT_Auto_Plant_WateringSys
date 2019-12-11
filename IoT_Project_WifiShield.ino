#include <WiFiNINA.h>
#include <ThingSpeak.h>
#include <WiFi.h>
#include "dht.h"
#define RX 2
#define TX 3
#define dht_apin A0 // Analog Pin sensor is connected to
#define soilSensor A5
#define waterSensor A1
dht DHT;
const char* ssid = "FBIvan8";       // AP NAME
const char* password = "yorkstjohn"; // AP PASSWORD
char API[] = "8AZCIE8756M2UDEE"; // Write API KEY
const unsigned long channel_id = 921264;
boolean DEBUG = true;
boolean STOPPED = false;
WiFiClient client;

int relay = 7;
int STOP = 200;
  
void setup() {
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  setupWifi();
  ThingSpeak.begin(client);
}

void loop() {
  //Serial.println(getWaterLevel >= 200);
  if (getWaterLevel() >= STOP){ //Disables watering plant system when water has ran out (protects the water pump)
    if (STOPPED){
      Serial.println("Thank you for topping me up. Watering Unpaused!");
      }
  executeWater(); //Check to see if plant needs watering
  postData();    //Post data to Thingspeak
   //Delay between posting data (2.5 mins)
   STOPPED = false;
  }else { //Prints out message to user when IoT has stopped. (Prints only once)
    if (!STOPPED){
      Serial.println("Water Levels need topping up. Watering Paused!"); 
      } 
      STOPPED = true;
      postData();
    }
  delay(150000);
}

void executeWater(){
  //Serial.println("Checking soil moisture " + String(getSoilMoisture()));
  //soil moisture check, turns on water
  if (getSoilMoisture() <= 290){
      Serial.println("Watering...");
      digitalWrite(relay, HIGH);
      delay(500);
      digitalWrite(relay,LOW);
     
  }
  }

void setupWifi(){
  //Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

//Posting Data To ThingSpeak
void postData(){
  Serial.println("Posting Data To ThingSpeak...");
  ThingSpeak.setField(1, getTemperatureValue());
  Serial.println(" Temperature (C)= " + getTemperatureValue());
  ThingSpeak.setField(2, getHumidityValue());
  Serial.println(" Humidity in %= " + getHumidityValue());
  ThingSpeak.setField(3, String(getWaterLevel()));
  Serial.println(" Water Level= " + String(getWaterLevel()));
  ThingSpeak.setField(4, String(getSoilMoisture()));
  Serial.println(" Soil Moisture= " +String(getSoilMoisture()));
  ThingSpeak.writeFields(channel_id, API);
  
    
}
//Returns Temperature in String Format
String getTemperatureValue(){

   DHT.read11(dht_apin);
   //Serial.print(" Temperature(C)= ");
   int temp = DHT.temperature;
   //Serial.println(temp); 
   delay(50);
   return String(temp); 
  
}

//Returns Humidity in String Format
String getHumidityValue(){

   DHT.read11(dht_apin);
  //Serial.print(" Humidity in %= ");
   int humidity = DHT.humidity;
   //Serial.println(humidity);
   delay(50);
   return String(humidity); 
  
}
//Return Soil Moisture in Float Format
float getSoilMoisture(){
  float soil_sensor = analogRead(soilSensor);
  float soil_voltage = (soil_sensor * 5000)/ 1024;
  soil_voltage = soil_voltage - 500;
  float soil_moisture = soil_voltage / 10;
  return soil_moisture;
  }


//Returns Water Level in Integer Format
int getWaterLevel(){
  int water_level = analogRead(waterSensor);
  return water_level;
  }
