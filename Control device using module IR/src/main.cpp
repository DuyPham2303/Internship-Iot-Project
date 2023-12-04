#include<Arduino.h>
#include<IRremote.h>
#include <EEPROMex.h>
#define EEPROMSize8k 8192
#define button1 4
#define button2 5
#define button3 6
#define ADDRESS_BUTTON1 0
#define ADDRESS_BUTTON2 4
#define ADDRESS_BUTTON3 8
IRsend irsend; // Pin 3 is constant define as IRsend 
uint32_t* address = 0; // address need to be registered
decode_results results;
const int RECV_PIN = 9;
uint32_t Hex_code[] ={0x2FD48B7,0x2FDD827,0x2FD7807};
uint32_t Read_Hex[3];
void Write_EEPROM();
void Control_Television();
void setup(){
    Serial.begin(9600);
    pinMode(button1,INPUT);
    pinMode(button2,INPUT);
    pinMode(button3,INPUT);
    digitalWrite(button1,HIGH);
    digitalWrite(button2,HIGH);
    digitalWrite(button3,HIGH);
    IrReceiver.begin(RECV_PIN, DISABLE_LED_FEEDBACK); 
    Write_EEPROM();
}
void loop(){
    if(IrReceiver.decode()){
        Serial.print("HEX code:");
        Serial.println(IrReceiver.decodedIRData.decodedRawData,HEX);
        delay(500);
        IrReceiver.resume();
    }
}
void Write_EEPROM(){
    for(int HEX_index = 0 ; HEX_index < 3 ; HEX_index++){
        EEPROM.writeLong(address, Hex_code[HEX_index]);
        address++;
    }
}
void Control_Television(){
    int val1 = digitalRead(button1); 
    int val2 = digitalRead(button2); 
    int val3 = digitalRead(button3);
    if(val1 == LOW)
    {
        irsend.sendNEC(EEPROM.readLong(ADDRESS_BUTTON1), 32); 
        Serial.println("TURN ON THE TV ");
    }
    else if(val2 == LOW)
    {
        irsend.sendNEC(EEPROM.readLong(ADDRESS_BUTTON2), 32); 
        Serial.println("CHANNEL UP");
    } 
    else if(val3 == LOW)
    {
        irsend.sendNEC(EEPROM.readLong(ADDRESS_BUTTON3), 32);
        Serial.println("CHANNEL DOWN");
    }
    else Serial.println("None");
}
void dump(decode_results *results) {
    // Dumps out the decode_results structure.
    // Call this after IRrecv::decode()
    int count = results->rawlen;
    if (results->decode_type == UNKNOWN)            Serial.print("Unknown encoding: ");
    else if (results->decode_type == SONY)          Serial.print("Decoded SONY: ");
    Serial.print(results->value, HEX);
    Serial.print(" (");
    Serial.print(results->bits, DEC);
    Serial.println(" bits)");
    Serial.print("Raw (");
    Serial.print(count, DEC);
    Serial.print("): ");
    for (int i = 1; i < count; i++) {
        if (i & 1) {
             Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
        }
        else {
            Serial.write('-');
            Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
        }
        Serial.print(" ");
    }
    Serial.println();
}