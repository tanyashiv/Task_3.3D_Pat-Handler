
// Include necessary libraries

#include <ArduinoMqttClient.h> //The Arduino MQTT client library is included for MQTT communication.
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

// Define ultrasonic sensor pins
const int trigPin = 2;  // Trigger pin for the ultrasonic sensor
const int echoPin = 3;  // Echo pin for the ultrasonic sensor

float duration, distance; // Variables used for storing sensor data

WiFiClient wifiClient;  // Create a Wi-Fi client object
MqttClient mqttClient(wifiClient); // Create an MQTT client object

// MQTT broker information
const char broker[] = "broker.mqttdashboard.com"; // MQTT broker address
int port = 1883;   // MQTT broker port
const char topic[] = "tanya-wave";  // MQTT topic to publish data

const long interval = 1000; // Sensor data transmission interval (milliseconds)
unsigned long previousMillis = 0;// Keep track of the last time data was sent.

int count = 0;  // A count variable that keeps track of iterations.

void setup() {

  Serial.begin(9600); // Initialize serial communication
  while (!Serial) {
    ; // Wait for the serial connection to be established.
  }

  Serial.print(" Trying to link up a connection with WPA SSID: "); //Print this message on the serial
  Serial.println(ssid); //Display the ssid

  // Try to connect to Wi-Fi using the given credentials.
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);  // Every 5 seconds, retry the Wi-Fi connection.
  }

  Serial.println("You're now linked to the network.");  //Print this message on the serial when connected



  Serial.print("Trying to establish a connection with the MQTT broker."); //Display this message on the serial
  Serial.println(broker); //Display the address of the broker

// Attempt to establish a connection with the MQTT broker
  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");  //Display this message on the serial
    Serial.println(mqttClient.connectError());  //Display the error code

    while (1)
      ;
  }

  Serial.println("You're connected to the MQTT broker!"); //Print this message on the serial when connected to broker

}

void loop() {

  mqttClient.poll();   // Check for MQTT messages


  unsigned long currentMillis = millis(); // Get the current time in milliseconds

  // Decide if it is time to transmit sensor data depending on the given interval.
  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;  // Update the previous time

  // Ultrasonic sensor logic for distance measurement
    digitalWrite(trigPin, LOW);   // Set the trigger pin low
    delayMicroseconds(2); //delay for 2 microseconds
    digitalWrite(trigPin, HIGH);    // Set the trigger pin high
    delayMicroseconds(10);  //wait for 10 microseconds
    digitalWrite(trigPin, LOW);   // Set the trigger pin low again

    duration = pulseIn(echoPin, HIGH);  // Determine the duration of the echo pulse
    distance = (duration * .0343) / 2;  // Calculate the distance 
    Serial.print("Distance: "); //Display this message on the serial
    Serial.println(distance); //Display the message on serial

    // Take a response if the distance is less than 12 .
    if (distance < 100) {
      mqttClient.beginMessage(topic); // Begin writing a MQTT message.
      mqttClient.print("Tanya Shiv : Wave is detected."); //Add message
      mqttClient.print("Distance=> "); //Include distance information
      mqttClient.print(distance); //Add the actual distance measurement.
      mqttClient.print(","); //Add the actual distance measurement.
      
      mqttClient.endMessage();  //Finish the MQTT message
      delay(1000);  //Wait
      Serial.println("Sent!!"); //Print message on serial
    }

    Serial.println(); //Print a line

    count++;  //Increment the count variable to keep track of iterations
  }
}