#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "wifi@lsong.org";
const char *password = "song940@163.com";
const char *mqttServer = "broker.emqx.io";
const char *clientID = "esp8266-relay";
const char *relayTopic = "esp8266-relay";

const int RY1 = 0;
const int LED1 = LED_BUILTIN;

WiFiClient wlan;
PubSubClient mqtt(wlan);

void reconnect()
{
  while (!mqtt.connected())
  {
    Serial.println("Attempting to connect to the MQTT server...");
    if (mqtt.connect(clientID))
    {
      Serial.println("MQTT connected");
      mqtt.publish(relayTopic, "online");
      mqtt.subscribe(relayTopic);
    }
    else
    {
      Serial.println("MQTT connect failed, retrying...");
      delay(2000);
    }
  }
}

void onMessage(char *topic, byte *payload, unsigned int length)
{
  // Handle received messages
  String payloadStr = "";
  for (unsigned int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i];
  }
  Serial.println("Received message: [" + String(topic) + "] " + payloadStr);

  if (strcmp(topic, relayTopic) == 0)
  {
    if (payloadStr == "relay1on")
    {
      digitalWrite(RY1, LOW);
    }
    else if (payloadStr == "relay1off")
    {
      digitalWrite(RY1, HIGH);
    }
    else if (payloadStr == "led1on")
    {
      digitalWrite(LED1, LOW);
    }
    else if (payloadStr == "led1off")
    {
      digitalWrite(LED1, HIGH);
    }
    else if (payloadStr == "ping")
    {
      mqtt.publish(relayTopic, "pong");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(RY1, OUTPUT);
  pinMode(LED1, OUTPUT);
  digitalWrite(RY1, HIGH);
  digitalWrite(LED1, HIGH);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setAutoReconnect(true);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Set the MQTT server and port
  mqtt.setClient(wlan);
  mqtt.setServer(mqttServer, 1883);
  mqtt.setKeepAlive(15);
  mqtt.setCallback(onMessage);
}

void loop()
{
  if (!mqtt.connected())
  {
    reconnect();
  }
  mqtt.loop();
}