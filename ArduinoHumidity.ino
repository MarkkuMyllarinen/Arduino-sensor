

#include <DHTesp.h>

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Hash.h>
#include <Bridge.h>

#include <WiFiClientSecureBearSSL.h>

// Replace with your network credentials
const char* ssid =      "###################";
const char* password =  "###################";


const uint8_t fingerprint[20] = {0x08, 0x3B, 0x71, 0x72, 0x02, 0x43, 0x6E, 0xCA, 0xED, 0x42, 0x86, 0x93, 0xBA, 0x7E, 0xDF, 0x81, 0xC4, 0xBC, 0x62, 0x30};


#define DHTPIN 14    // Digital pin connected to the DHT sensor

/** Initialize DHT sensor */
DHTesp dht;



// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;



// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 300000;

int status = WL_IDLE_STATUS;

const String endpoint = "https://asema.herokuapp.com/newdata/" + String(t) + "/" + String(h);


void http_request()
{

  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status


     std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

     
    client->setFingerprint(fingerprint);

    HTTPClient http;

    http.begin(*client, endpoint); //Specify the URL
    int httpCode = http.GET();  //Make the request

    if (httpCode > 0) { //Check for the returning code

      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    }

    else {
      Serial.println("Error on HTTP request");
    }
    delay(5000);
    http.end(); //Free the resources
  }

}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  dht.setup(DHTPIN, DHTesp::DHT11);


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Print ESP8266 Local IP Address



  Serial.println(WiFi.localIP());
}

void loop() {



  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;

    // Read temperature as Celsius (the default)
    float newT = dht.getTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.getHumidity();
    // if humidity read failed, don't change h value
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
      http_request();
    }
  }
}
