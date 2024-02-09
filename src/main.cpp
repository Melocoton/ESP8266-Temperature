#include <Arduino.h>
#include <ESP8266Wifi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTTYPE DHT11

const uint8_t DHTpin = 2; // GPIO2 / D4

DHT dht(DHTpin, DHTTYPE);

float temperature;
float humidity;
String data = "";

const char *ssid =  "ssid";
const char *pass =  "pass";
const char *mqttServer = "server address";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);
    Serial.println("");
    Serial.println("WiFi connected"); 
    Serial.print("AP:\t");
    Serial.println(WiFi.BSSIDstr());
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
    Serial.print("MAC address:\t");
    Serial.println(WiFi.macAddress());

    Serial.println("Inicializando...");
    mqttClient.setServer(mqttServer, 1883);
    pinMode(DHTpin, INPUT_PULLUP);
    dht.begin();
}

// void sendMsg(const char *data, const char *topic) {
//     mqttClient.publish("casa/temperatura", data);
// }

void connectMqtt() {
    while (!mqttClient.connected()) {
        Serial.println("Connecting to MQTT broker...");
        if (mqttClient.connect(WiFi.macAddress().c_str())) {
            Serial.println("Connected, sending hello");
            data = "";
            data.concat(WiFi.macAddress());
            data.concat(":HELLO");
            mqttClient.publish("casa/hello", data.c_str());
        } else {
            Serial.println("Connection failed, retrying in 5 seconds");
            delay(5000);
        }
    }
}

// String formData(float t, float h) {
//     String a = WiFi.macAddress();
//     a.concat(";T:");
//     a.concat(t);
//     a.concat(";H:");
//     a.concat(h);
//     return a;
// }

void loop()
{

    if (!mqttClient.connected()) {
        connectMqtt();
    }

    // put your main code here, to run repeatedly:
    delay(3000);
    mqttClient.loop();

    temperature = dht.readTemperature(false);
    humidity = dht.readHumidity();

    Serial.print("Tmp: ");
    Serial.println(temperature);
    Serial.print("Hum: ");
    Serial.println(humidity);

    data = "";
    data = WiFi.macAddress();
    data.concat(";T:");
    data.concat(temperature);
    data.concat(";H:");
    data.concat(humidity);
    Serial.print("Sending data: ");
    Serial.println(data);

    mqttClient.publish("casa/temperature", data.c_str());

    Serial.println("");
    Serial.println("---------------");
    
}