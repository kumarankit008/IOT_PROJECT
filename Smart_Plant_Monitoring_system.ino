
//include libraries
#include<ESP8266WiFi.h>
#include<WiFiClientSecure.h>
#include<MQTT.h>
#include<ArduinoJson.h>
#include<time.h>
#include"secrets.h"
#include"DHT.h"

int moisturePin = A0;
int ldrPin = D3;
#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 5
DHT dht(dht_dpin, DHTTYPE);


const int MQTT_PORT = 8883;
const char MQTT_SUB_TOPIC[] = "$aws/things/" THINGNAME "/shadow/update";
const char MQTT_PUB_TOPIC[] = "$aws/things/" THINGNAME "/shadow/update";

#ifdef USE_SUMMER_TIME_DST
uint8_t DST = 1;
#else
uint8_t DST = 0;
#endif

BearSSL::X509List cert(cacert);
BearSSL::X509List client_crt(client_cert);
BearSSL::PrivateKey key(privkey);

WiFiClientSecure net;
MQTTClient client;

//time
unsigned long lastMillis = 0;
time_t now;
time_t nowish = 1510592825;

void NTPConnect(void)
{
  Serial.print("Setting time using SNTP");
  configTime(TIME_ZONE * 3600, DST * 3600, "pool.ntp.org", "time.nist.gov");
  now = time(nullptr);
  while (now < nowish)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("done!");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
//  Serial.print("Current time: ");
//  Serial.print(asctime(&timeinfo));
}

void messageReceived(String &topic, String &payload)
{
  Serial.println("Recieved [" + topic + "]: " + payload);
     StaticJsonDocument<256> doc;
     deserializeJson(doc, payload);
     Serial.println("Received data = ");
     Serial.print(payload);
     Serial.println();
}



//mqtt connect
void connectToMQTT(){

   
   while(!client.connected()){
     Serial.println("MQTT Connecting ");
     if(client.connect(THINGNAME)){

        Serial.println("Connected to AWS");

        client.subscribe("inTopic");
     }
     else{
      Serial.print("Connection failed");
     }
   }
}

//connect to Wifi
void connectToWiFi(){

    WiFi.hostname(THINGNAME);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid,pass);
    Serial.println("Connecting to WiFi ");
    while(WiFi.status()!=WL_CONNECTED){
      Serial.print(".");
      delay(500);
    }

    Serial.println("Connected to Wifi");
  
}

void setup(){
  Serial.begin(115200);
  delay(5000);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(ldrPin,INPUT);
  
  connectToWiFi();
  NTPConnect();
  
  net.setTrustAnchors(&cert);
  net.setClientRSACert(&client_crt, &key);

  client.begin(MQTT_HOST, MQTT_PORT, net);
  client.onMessage(messageReceived); 
  connectToMQTT();
//  digitalWrite(LED_BUILTIN,1);

}

long lastMsg = 0;
char msg[50];
int value = 0;

void loop(){

    //read sensor data
    float t=dht.readTemperature();
    float h=dht.readHumidity();
    delay(1000);
    float soilMoisture = analogRead(moisturePin);
    delay(1000);
    int ldr = digitalRead(ldrPin);
    
    
  
  if(!client.connected()){
    connectToMQTT();
  }
  client.loop();
  
  if(isnan(t) || isnan(h)|| isnan(soilMoisture) || isnan(ldr)){
      Serial.println("Failed to read the sensors");
      return;
    }
  long now=millis();
  if(now-lastMsg>2000){
    lastMsg=now;
    StaticJsonDocument<200>doc;

     if(ldr ==0){
      doc["Light_Status"]="Night";
     }
    else if(ldr==1){
      
      doc["Light_Status"]="Daylight";
      if(soilMoisture > 350){
        doc["Soil_Status"]="DRY";
        digitalWrite(LED_BUILTIN,0);  //led on when dry indicating to water
        
      }
      else if(soilMoisture <=350){
        doc["Soil_Status"]="WET";
        digitalWrite(LED_BUILTIN,1);
      }
    }
    doc["Temperature"]=t;
    doc["Humidity"]=h;
    doc["Soil_Moisture_Content"]=soilMoisture;
    
    

    Serial.print("Data Published = ");                  
    serializeJson(doc,Serial);
    Serial.println();

    char shadow[measureJson(doc) + 1];
    
    serializeJson(doc, shadow, sizeof(shadow));
  

     client.publish("outTopic",shadow);
     
//     Serial.println(shadow);

     
  }
  
  delay(10000);
  
}
