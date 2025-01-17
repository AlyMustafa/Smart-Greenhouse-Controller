#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "DHT.h"

//wifi SSID & Password
const char *ssid = "Mario";
const char *password = "#Enayaa12345#";

#define DHTPIN 26
#define fan 4

#define Humidifier  18

float HumidityThreshold = 82.5;
float H_Hysteresis = 2.5;             // 5% hysteresis to prevent rapid switching
bool  HumidifierState = false;         // Stores the current state of the humidifier
float Humidity = 0;

float TemperatureThreshold = 25.2;  //29
float T_Hysteresis = 0.1; //1
bool  TemperatureState = false; 
float Temperature = 0;

WebServer server(80);
DHT dht(DHTPIN, DHT11);

//new
void handleRoot() {
  char msg[2000];  // Increased the size to accommodate extra content

  float temperature = readDHTTemperature();
  float humidity = readDHTHumidity();

  snprintf(msg, 2000,
           "<html>\
  <head>\
    <meta http-equiv='refresh' content='1'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body>\
      <h2>ESP32 DHT Server!</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperature</span>\
        <span>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </p>",
           temperature);

  // Add "Fan Works" if the temperature is greater than 28
  if (Temperature_status()) {
    snprintf(msg + strlen(msg), 500, 
             "<p style='color:#ca3517; font-size:1.5rem;'>Fan Works</p>");
  }

  snprintf(msg + strlen(msg), 1500,
           "<p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humidity</span>\
        <span>%.2f</span>\
        <sup class='units'>&percnt;</sup>\
      </p>",
           humidity);

  // Add "Humidifier Works" if the humidity is greater than 85
  if (Humidifier_status()) {
    snprintf(msg + strlen(msg), 500, 
            "<p style='color:#00add6; font-size:1.5rem;'>Humidifier Works</p>");
  }

  // Close the HTML body and send the response
  snprintf(msg + strlen(msg), 500,
           "</body>\
</html>");

  server.send(200, "text/html", msg);
}

void setup() {
 
  delay(500);
  Serial.begin(9600);
  dht.begin();

  //Fan & Humidifier
  pinMode(fan,OUTPUT);
  pinMode(Humidifier,OUTPUT);

  //Wifi configration
   WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
      digitalWrite(fan, HIGH); // Turn OFF the Fan, The relay is active low
    digitalWrite(Humidifier, HIGH); // Turn OFF the Fan, The relay is active low

server.handleClient();
 delay(200);
 bool  TemperatureState = false; 
 bool  HumidifierState = false;
  // Reading temperature or humidity takes about 250 milliseconds!
   // Read Humidity
  
while(1){
Humidity = readDHTHumidity();
  // Read temperature as Celsius (the default)
  Temperature = readDHTTemperature();
Temperature_status();
Humidifier_status();
}

}

float readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds
  // Read temperature as Celsius (the default)
  if (isnan(dht.readTemperature())) {    
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  else {
  Serial.print("Temperature: ");
  Serial.print(Temperature);
  Serial.println("°C ");
  Serial.print("TemperatureState: ");
  Serial.println(TemperatureState);
}
    return dht.readTemperature();
  }


float readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds
 // float h = dht.readHumidity();
  if (isnan(dht.readHumidity())) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  else {
  Serial.print("Humidity: ");
  Serial.print(Humidity);
  Serial.print("% ");
  return dht.readHumidity();
  }
}

char Temperature_status() {
    Temperature = readDHTTemperature();

  if (Temperature >= (TemperatureThreshold + T_Hysteresis) && !TemperatureState) { //25.1
    digitalWrite(fan, LOW); // Turn ON the Fan, The relay is active low
    TemperatureState = true; // Update state
    return 1;
  }
  else if (Temperature <= (TemperatureThreshold - T_Hysteresis) && TemperatureState) {//24.9
    digitalWrite(fan, HIGH); // Turn OFF the Fan, The relay is active low
    TemperatureState = false; // Update state
    return 0;
  }
  
  // No change in state
  return TemperatureState ? 1 : 0;
}


char Humidifier_status() {
    Humidity = readDHTHumidity();

  if (Humidity >= (HumidityThreshold + H_Hysteresis) && HumidifierState) {
    digitalWrite(Humidifier, HIGH); // Turn OFF the humidifier, The relay is active low
    HumidifierState = false; // Update state
    return 0;
  }
  else if (Humidity <= (HumidityThreshold - H_Hysteresis) && !HumidifierState) {
    digitalWrite(Humidifier, LOW); // Turn ON the humidifier, The relay is active low
    HumidifierState = true; // Update state
    return 1;
  }
  
  // No change in state
  return HumidifierState ? 1 : 0;
}




