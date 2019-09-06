/**
 * Created by André - Gala - Nicolas - Youssef
 * Wenesday 11/04/2018
 * IoT - ESP32 Project
 */
#include "DHT.h"
#include <WiFi.h>

#define DHTPIN 26
#define DHTTYPE DHT11

const int trigPin = 34;
const int echoPin = 27;
long timeTravel;
int distance;

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);


WiFiClient client;


String temperaturekey = "5acdb749c03f9736579ca8ef";
String humiditykey = "5acdb74fc03f9736579ca8f3";
String distancekey = "5acdbcfac03f97406a02a234";



//The API token, we need it to connect to ubidots
String token = "A1E-LPYLf6ncoIZNpHxVXoIdJd1NGZ3U0O";

// Wifi variables
const char* ssid = "SFR-8ec0";
const char* password = "Q29CGARCDS2M";

//////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
void ubidots_getValues(String, String);

// The setup function is executed once by the ESP8266 when it's powered up or reset
void setup()
{
  
  Serial.begin(115200);
  
  // Start the communication with the DHT sensor
  dht.begin();

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Using the scanNetworks function to scan available wifi network available
  int n = WiFi.scanNetworks();

  Serial.println("scan done"); 
  if (n == 0)
  {
    Serial.println("No networks found");
    Serial.println("Try again ...");

  }

  // Start the connection
  WiFi.begin(ssid, password);

  // Establishing the connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("#");
  }

  // Wifi connected !
  Serial.println("");
  Serial.println("Wi-Fi connected !!");
}


void loop()
{
  //Setting the SR-04 Ultrasonic sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, which will return the sound wave travel time in microseconds
  timeTravel = pulseIn(echoPin, HIGH);

  // Calculating distance
  distance= timeTravel*0.034/2;

  Serial.print("Distance: ");
  Serial.println(distance);
  // Read the current temperature and humidity measured native dht sensor functions
  float temp = dht.readTemperature(true);
  float hum = dht.readHumidity();

  // Call the function defined at the bottom
  ubidots_getValues(String(temperaturekey), String(temp));
  ubidots_getValues(String(humiditykey), String(hum));
  ubidots_getValues(String(distancekey), String(distance));

  // Send some debug messages over USB
  Serial.println("Ubidots data");
  Serial.println("temperature: "+String(temp));
  Serial.println("humidity: "+String(hum));
  Serial.println("distance: ");
  Serial.println(distance);
  Serial.println(" Wait a moment ;)" );

  
  delay(5000);  
}

//The main function. Make the link with the API on Ubidots
void ubidots_getValues(String variable_id, String value)
{
  
  String var = "{\"value\": " + value +"}"; 
  String length = String(var.length());

  
  if (client.connect("things.ubidots.com", 80))
  {
    Serial.println("Connected to Ubidots...");
    delay(200);

    // Sending the datas to the specified URL using the POST method
    client.println("POST /api/v1.6/variables/"+variable_id+"/values HTTP/1.1");
    // We also use the Serial terminal to show how the POST request looks like
    Serial.println("POST /api/v1.6/variables/"+variable_id+"/values HTTP/1.1");
    // Specify the contect type so it matches the format of the data (JSON)
    client.println("Content-Type: application/json");
    Serial.println("Content-Type: application/json");
    // Specify the content length
    client.println("Content-Length: "+ length);
    Serial.println("Content-Length: "+ length);
    // Use our own API token so that we can actually publish the data
    client.println("X-Auth-Token: "+ token);
    Serial.println("X-Auth-Token: "+ token);
    // Specify the host
    client.println("Host: things.ubidots.com\n");
    Serial.println("Host: things.ubidots.com\n");
    // Send the actual data
    client.print(var);
    Serial.print(var+"\n");
  }
  else
  {
    Serial.println("Ubidots connection failed...");
  }
  
}
