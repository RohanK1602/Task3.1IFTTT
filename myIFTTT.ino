#include "arduino_secrets.h"
// BH1750 - Version: 1.3.0
#include <BH1750.h>

#include <WiFiNINA.h>

#include <Wire.h>

BH1750 lightMeter;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASSWORD;

WiFiClient client;

char   HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME   = " MY KEY"; 
String queryString = "?value1=57&value2=25";

String PATH_NAME2   = "2ND KEY"; 

bool lightIncreaseTriggered = false; 
bool lightDecreaseTriggered = false; 

void setup() {
  Serial.begin(9600); 
  while(!Serial) ;    
  Wire.begin();       
  lightMeter.begin();
  Serial.println(F("Begin Light Reading"));

  // Connect to WiFi:
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
}

void loop() {
  
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);

  // Trigger when light level is greater than or equal to 25 lux
  if (lux >= 25 && !lightIncreaseTriggered) {
    Serial.println("Light has passed 25 lux");
    sendHttpRequest(PATH_NAME + queryString);
    lightIncreaseTriggered = true;
    lightDecreaseTriggered = false;
  }

  // Trigger when light level is less than or equal to 5 lux
  if (lux <= 5 && !lightDecreaseTriggered) {
    Serial.println("Light has decreased below 5 lux");
    sendHttpRequest(PATH_NAME2 + queryString);
    lightDecreaseTriggered = true;
    lightIncreaseTriggered = false;
  }

  // Reset the trigger flags if the light is between 5 and 25 lux
  if (lux > 5 && lux < 25) {
    lightIncreaseTriggered = false;
    lightDecreaseTriggered = false;
  }
}

void sendHttpRequest(String path) {
  if (client.connect(HOST_NAME, 80)) {
    Serial.println("Connected to server");
    client.println("GET " + path + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // End HTTP header

    while (client.connected()) {
      while (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }

    client.stop();
    Serial.println("\nDisconnected.");
  } else {
    Serial.println("Connection failed");
  }
}
