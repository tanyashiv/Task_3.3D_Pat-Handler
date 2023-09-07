// Include necessary libraries
#include <ArduinoMqttClient.h>  //The Arduino MQTT client library is included for MQTT communication.
//Look for specified Arduino board types (SAMD MKR WiFi 1010, SAMD Nano 33 IoT, AVR Uno WiFi Rev2) and add the WiFiNINA library if any of these boards are found.
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
//Include WiFi101 library if MKR 1000 board is found.
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
//Include ESP8266WiFi library for ESP8266-based boards.
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
//Include WiFi library for ESP32-based boards.
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

// Specify Wi-Fi password and SSID 
char ssid[] = "tanya";
char pass[] = "hello12345";

int light = 2;  // Create a variable for the LED's connected pin.

WiFiClient wifiClient;  // Create a Wi-Fi client object
MqttClient mqttClient(wifiClient);  // Create an MQTT client object

// MQTT broker information
const char broker[] = "mqtt-dashboard.com"; // MQTT broker address
int port = 1883;   // MQTT broker port
const char topic[] = "tanya-wave";  // Subscribe to a MQTT topic
float arr[4];
int indo;
void setup() {

  Serial.begin(9600); // Initialize serial communication
  pinMode(light, OUTPUT);  //Make the LED-connected pin an output.
  while (!Serial) {
    ;  // wait for serial port to connect. 
  }


  Serial.print("Attempting to connect to WPA SSID: "); //Display the message on serial
  Serial.println(ssid); //display the ssid
  // Attempt to connect to Wi-Fi using the credentials provided.
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {

    Serial.print(".");  // Display dots while attempting to connect
    delay(5000);   // Wait 5 seconds before trying again.
  }

  Serial.println("You're connected to the network");   // Connection to Wi-Fi successful
  Serial.println(); //Print a line

  arr[0] = 0;
  arr[1] = 0;
  arr[2] = 0;

  Serial.print("Attempting to connect to the MQTT broker: "); //Display message on the screen
  Serial.println(broker); //Display broker address on the screen

    //connect to the MQTT broker using the mqttClient.
  if (!mqttClient.connect(broker, port)) {  //checks if the connection attempt was unsuccessful.
    Serial.print("MQTT connection failed! Error code = ");  //error message is displayed.
    Serial.println(mqttClient.connectError());   //error code is displayed

    while (1)
      ; //infinite loop
  }

 //code is executed when the MQTT connection is successful.
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

// Print success messages for MQTT connection and subscription to a topic
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();


  mqttClient.subscribe(topic);



  Serial.print("Waiting for messages on topic: ");
  Serial.println(topic);
  Serial.println();
  
}

void loop() {
  int messageSize = mqttClient.parseMessage();   // Check if a new MQTT message has arrived; non-zero messageSize indicates a message is present
  if (messageSize) {

    // Print details of the received MQTT message, including its topic and length
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

 

    String data;
    bool temp =false;
    while (mqttClient.available()) {
      char c = (char)mqttClient.read();

      Serial.print(c);

      if(c == '>')
      {
        temp = true;
      }
      else if(c == ',')  
      {
        temp = false;
      }
      if( temp )
      {
          data += c;
      }

    
    }

    
    arr[indo] = data.toFloat();
    indo++;

    if(arr[0] < 20 && arr[1] < 20 && arr[2] < 20 )
    {

    Serial.println();
    digitalWrite(light, HIGH);  // turn the LED on 
    delay(500);                 // wait 
    digitalWrite(light, LOW);   // turn the LED off 
    delay(500);
    
    }
    else if(arr[0] - arr[1] > 20 || arr[1] - arr[2]  > 20 )
    {
      Serial.println();
    digitalWrite(light, HIGH);  // turn the LED on 
    delay(1000);                 // wait 
    digitalWrite(light, LOW);   // turn the LED off
    delay(1000);
   
    }

    Serial.println();

    if( indo >4)
    {
      indo = 3;
      arr[0] = arr[1];
      arr[1] = arr[2];
    }
  }
}