#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson-v5.13.5.h>
#include <pins_arduino.h>
#define WIFI_AP "YOUR_WIFI_AP"
#define WIFI_PASSWORD "0972665872"
char Thingsboard_Server[] = "192.168.43.162";
#define User_name "DUY"
WiFiClient wifiClient;
int status = WL_IDLE_STATUS;
PubSubClient client(wifiClient);
boolean gpioStatus[] = {false,false,false};
void InitWiFi();
void Init_Thingsboard();
void reconnect();
void setup() {
 // put your setup code here, to run once:
    Serial.begin(115200);
    delay(10);
    pinMode(D0,OUTPUT);
    pinMode(D1,OUTPUT);
    pinMode(D2,OUTPUT);
    digitalWrite(D0,HIGH);
    digitalWrite(D1,HIGH);
    digitalWrite(D2,HIGH);
    InitWiFi();
    Init_Thingsboard();
}
void loop() {
 // put your main code here, to run repeatedly:
    if(!client.connected()){
         reconnect();
    }
    client.loop();
}
void callback(const char* topic, uint8_t*payload, unsigned int length){
    //Serial.println("On message");
    char json[length + 1];
    strncpy (json, (char*)payload, length);
    json[length] = '\0';
    Serial.println("Topic: ");
    Serial.println(topic);
    Serial.println("Message: ");
    Serial.println(json);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &data = jsonBuffer.parseObject((char*)json);
    if (!data.success()){
        Serial.println("parseObject() failed");
        return;
    }
    String methodName = String((const char*)data["method"]);
    if(methodName.equals("R1")){ 
        if(data["params"]==true){
            digitalWrite(D0,LOW);
            gpioStatus[0]=true;
        }
        else{
            digitalWrite(D0,HIGH);
            gpioStatus[0]=false;
        }
    }
    /**************************************************/
    if(methodName.equals("R2")){
        if(data["params"]==true){
            digitalWrite(D1,LOW);
            gpioStatus[1]=true;
        }
        else{
            digitalWrite(D1,HIGH);
            gpioStatus[1]=false;
        }
    }
    /**************************************************/
    if(methodName.equals("R3")){
        if(data["params"]==true){
            digitalWrite(D2,LOW);
            gpioStatus[2]=true;
        }
        else{
            digitalWrite(D2,HIGH);
            gpioStatus[2]=false;
        }
    }
    /**************************************************/
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
}
void reconnect(){
    // Loop until we're reconnected
    while (!client.connected()){
        status = WiFi.status();
        if( status != WL_CONNECTED){
            WiFi.begin(WIFI_AP, WIFI_PASSWORD);
            while (WiFi.status() != WL_CONNECTED){
                delay(500);
                Serial.print(".");
            }
        Serial.println("Connected to AP");
        }
        Serial.print("Connecting to Thingsboard node ...");
        // Attempt to connect (clientId, username, password)
        if(client.connect("ESP8266 Device",User_name,NULL)){
            Serial.println( "[DONE]" );
            client.subscribe("v1/devices/me/rpc/request/+");//dang ky nhan cac lenh rpc tu cloud
            client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
        }
        else{
            Serial.print( "[FAILED] [ rc = " );
            Serial.print( client.state());
            Serial.println(" : retrying in 5 seconds]");
            // Wait 5 seconds before retrying
            delay( 5000 );
        }
    }
}
String get_gpio_status(){
    //Prepare gpios JSON payload string
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& data = jsonBuffer.createObject();
    
    data["R1"]=gpioStatus[0] ? true : false;
    data["R2"]=gpioStatus[1] ? true : false;
    data["R3"]=gpioStatus[2] ? true : false;
    char payload[256];
    data.printTo(payload, sizeof(payload));
    String strPayload = String(payload);
    Serial.println("Get gpio status: ");
    Serial.println(strPayload);
    return strPayload;
}
void InitWiFi(){
    Serial.println("Connecting to AP ..."); // attempt to connect to WiFi network
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to AP")
}
void Init_Thingsboard(){
    client.setServer(Thingsboard_Server,1883); //init thingsboard address and mqtt port
    client.setCallback(callback);  //create a callback function to execute specific task whenever receive Json data from thingsboard server
}