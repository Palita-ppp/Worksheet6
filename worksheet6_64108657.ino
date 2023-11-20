#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN D4
#define DHTTYPE DHT11
#define LEDPIN D6

const char* ssid = "Feezoh";
const char* password = "254519";
const char* mqtt_server = "172.20.10.2";
const char* dht_topic = "dht11";
const char* led_topic = "LED";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void connectToMqtt() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("connected");
      mqttClient.subscribe(led_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void handleMqttMessage(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (message == "On") {
    digitalWrite(LEDPIN, HIGH);
  } else if (message == "Off") {
    digitalWrite(LEDPIN, LOW);
  }
}

void setup() {
  pinMode(LEDPIN, OUTPUT);
  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(handleMqttMessage);
  dht.begin();
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMqtt();
  }
  mqttClient.loop();

  static unsigned long lastReadTime = 0;
  if (millis() - lastReadTime > 5000) {
    lastReadTime = millis();
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    if (!isnan(humidity) && !isnan(temperature)) {
      String dhtData = "Humidity: " + String(humidity) + "%, Temperature: " + String(temperature) + "°C";
      mqttClient.publish(dht_topic, dhtData.c_str());
    }
  }
}
