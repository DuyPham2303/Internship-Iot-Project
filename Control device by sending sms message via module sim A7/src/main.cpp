#include "Arduino.h"
#include "String.h"
int led = 5;
String message;
void Gsm_Init();
void Gsm_MakeSMS(String phone,String content); // Calling message function
void serialEvent_GSM();
void setup()
{
    Serial.begin(9600); 
    delay(100);
    GSM_Init();
    pinMode(led,OUTPUT); 
    digitalWrite(led,HIGH);
}
void loop()
{
    if(Serial.available()>0){ // Reading and naviagte data whenever module sim receives a message
        serialEvent_GSM(); 
    }
}
void serialEvent_GSM(){
    if (Serial.available()>0){
        message = Serial.readString();
    }
    if(message.indexOf("ON") > -1){
        Serial.println("LED ON");
        digitalWrite(led,HIGH);
        delay(1000);
        GSM_MakeSMS("0972665872", "DEN DA DUOC BAT");
    }
    else if(message.indexOf("OFF") > -1){
        Serial.println("LED OFF");
        digitalWrite(led,LOW);
        delay(1000);
        GSM_MakeSMS("0972665872", "DEN DA DUOC TAT");
    }
    else if(message.indexOf("KIEMTRA") > -1){
        if(digitalRead(led)) GSM_MakeSMS("0972665872", "den dang bat"); //Light is current ON
        else                 GSM_MakeSMS("0972665872", "den dang tat"); //Light is current OF
    }
 }
void GSM_MakeSMS(String phone, String content){
    Serial.println("AT+CMGS=\"" + phone + "\""); // sends message
    delay(5000); // wait for character '>' respond
    Serial.print(content); // sends content
    Serial.print((char)26); // press Ctrl+Z or 26 to end typing message and send it
    delay(5000); // delay for 5s
}
void GSM_Init(){
    Serial.println("ATE0"); // turn of response mode
    delay(3000);
    Serial.println("AT+IPR=9600"); // set speed of data transfer 9600 bps
    delay(3000);
    Serial.println("AT+CMGF=1"); // Chose TEXT Mode
    delay(3000);
    Serial.println("AT+CLIP=1"); // Presents incoming caller information
    delay(3000);
    Serial.println("AT+CNMI=2,2"); // directly presents message's contents
    delay(3000);
}