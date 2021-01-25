#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <dht.h>

#define LED_PIN LED_BUILTIN
#define DHT_PIN D1

const char* wifi_ssid = "ssid";
const char* wifi_key = "password";

const char* mqtt_host = "192.168.1.10";

WiFiClient wifi_client;
MQTTClient mqtt_client;
DHT dht_sensor(DHT_PIN, DHT22);

void setup() {
  // Configure pins for LED and DHT
  pinMode(LED_PIN, OUTPUT);
  pinMode(DHT_PIN, INPUT);

  // Set BaudRate to 115k
  Serial.begin(115200);

  // Setup DHT
  dht_sensor.begin();

  // Setup WiFi
  WiFi.begin(wifi_ssid, wifi_key);

  // Setup MQTT
  mqtt_client.begin(mqtt_host, wifi_client);

}

void processWiFi() {
  if(WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to WiFi: ");
    Serial.println(wifi_ssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    // Good. We have WiFi now!
    Serial.println("... Success!");
  }
}

void processMQTT() {
  if(!mqtt_client.connected()) {

    // Connect to MQTT server
    Serial.print("Connecting to MQTT server: ");
    Serial.println(mqtt_host);
    while (!mqtt_client.connect("NodeMCU", "", "")) {
      delay(500);
      Serial.print(".");
    }

    // Awesome. MQTT is ready!
    Serial.println("... Success!");
  }
}

void loop() {
  
  // Put this into the loop to also show reconnecting attempts
  processWiFi();

  // Put this into the loop to ensure we keep an active mqtt connection
  processMQTT();

  // LED off
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED is off");
  mqtt_client.publish("/led", "LED is off");
  delay(5000);

  // Read temperature
  float temperature = dht_sensor.readTemperature();
  if(!isnan(temperature)) {
    Serial.print("Temperature: ");
    Serial.println(temperature);
    mqtt_client.publish("/dht/temperature", String(temperature));
  }

  // LED on
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED is on");
  mqtt_client.publish("/led", "LED is on");
  delay(5000);

  // Read humidity
  float humidity = dht_sensor.readHumidity();
  if(!isnan(humidity)) {
    Serial.print("Humidity: ");
    Serial.println(humidity);
    mqtt_client.publish("/dht/humidity", String(humidity));
  }
}