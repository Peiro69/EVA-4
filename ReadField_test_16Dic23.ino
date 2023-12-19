/*
  ReadField
  
  Description: Demonstates reading from a public channel which requires no API key and reading from a private channel which requires a read API key.
               The value read from the public channel is the current outside temperature at MathWorks headquaters in Natick, MA.  The value from the
               private channel is an example counter that increments every 10 seconds.
  
  Hardware: ESP8266 based boards
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires ESP8266WiFi library and ESP8622 board add-on. See https://github.com/esp8266/Arduino for details.
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2020, The MathWorks, Inc.
*/


// ELIMINAR EL DELAY Y USAR LA FUNCION MILLIS()

#include <ESP8266WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include <Servo.h>


char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

int servoPin = D7;
//pines del sensor
int TRIG = D5;
int ECHO = D3;
//variables que necesita el sensor
int duracion, distancia;
Servo miServo;


//channel details
unsigned long channelNumber = SECRET_CH_ID;
//actuador
const char * myReadAPIKey = SECRET_READ_APIKEY;
//sensor
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
// definir los campos
unsigned int campoServo = 1;


unsigned long tiempo1, tiempo2;
unsigned long tx = 0, ty = 0;



void setup() {
  //pin del servo
  miServo.attach(servoPin);
  //pines del sensor
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  Serial.begin(115200);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  int statusCode = 0;

  tiempo1 = millis();
  tiempo2 = millis();
  
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected");
  }


  //Desde TS al arduino, y del arduino al servo motor
  
  // Read in field 1 
  long valorServo = ThingSpeak.readLongField(channelNumber, campoServo, myReadAPIKey);  

  // Check the status of the read operation to see if it was successful
  statusCode = ThingSpeak.getLastReadStatus();
  if(statusCode == 200){
    //Serial.println("hola, si está entrando al if, y el valor del servo es: " + String(valorServo));
    if (valorServo == 1){
      //Serial.println("A este igual uwu, y si logra entrar acá es problema de TS: "+ String(valorServo));
      miServo.write(180);
    }else{
      //Serial.println("También entra a esta opción: " + String(valorServo));
      miServo.write(0);
    }
  }else{
    Serial.println("HOLA Actuador Problem reading channel. HTTP error code " + String(statusCode)); 
  }

    
 



    
    
   //Desde el sensor al arduino y del arduino a TS  

    
   digitalWrite(TRIG, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG, LOW);
   duracion = pulseIn(ECHO, HIGH);
   distancia = (duracion/2)/29;
   Serial.println(distancia);
  
   int x = ThingSpeak.writeField(channelNumber, 2, distancia, myWriteAPIKey);
   if(x == 200){
     Serial.println("HOLA Sensor Channel update successful." + String(x));
   }else{
     Serial.println("HOLA Sensor Problem updating channel. HTTP error code " + String(x));
   }
    
  
  
  




  
  
  delay(2000); // No need to read the counter too often.
  
}
