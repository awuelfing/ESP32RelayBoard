#include "config.h"
#include "password.h"

#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastHeartbeat = 0;

char outmsg[MSG_BUFFER_SIZE];
char inmsg[MSG_BUFFER_SIZE];
char command[MSG_COMMAND_SIZE];
int pin = 0;
bool pins[32] = {false};

void heartbeat()
{
  snprintf(outmsg, MSG_BUFFER_SIZE, "%u", millis());
  client.publish(MQTT_TOPIC_HEARTBEAT, outmsg);
}

void subscribe()
{
  client.subscribe(MQTT_TOPIC_RELAY_COMMAND);
}

void setup_wifi()
{
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println(topic);

  /*  bring the byte array into a char array,
      zero terminate that,
      and print the contents to serial while it's happening */
  for (int i = 0; i < length; i++)
  {
    inmsg[i] = (char)payload[i];
    Serial.print((char)payload[i]);
  }
  inmsg[length] = (char)NULL;
  Serial.println();

  if (strcmp(topic, MQTT_TOPIC_RELAY_COMMAND) == 0)
  {
    sscanf(inmsg, "%s %d", command, &pin);
    Serial.println(command);
    Serial.println(pin);

    if (strcmp(command, "ON") == 0)
    {
      pinMode(pin, OUTPUT);
      pins[pin] = true;
      digitalWrite(pin, HIGH);
    }
    else if (strcmp(command, "OFF") == 0)
    {
      pins[pin] = false;
      digitalWrite(pin, LOW);
    }
    //    else if (strcmp(command, "STATUS") == 0)
    //    {
    snprintf(outmsg, MSG_BUFFER_SIZE, "STATUS %d %s", pin, pins[pin] ? "ON" : "OFF");
    client.publish(MQTT_TOPIC_RELAY_STATUS, outmsg);
    //    }
  }
  else
  {
    // unhandled topic
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(clientId))
    {
      Serial.println("connected");
      heartbeat();
      subscribe();
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup()
{
  // serial setup
  Serial.begin(115200);

  // wifi setup
  setup_wifi();

  // mqtt setup
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  /*connect to mqtt in this loop
    because right now the code will hang
    if the current sensor isn't found
  */
  reconnect();
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();

  if (now - lastHeartbeat > HEARTBEAT_FREQUENCY)
  {
    lastHeartbeat = now;
    heartbeat();
  }
}
