

const char* mqtt_server = "192.168.1.150";
const char* clientId = "DEF_DEVICE_NAME";

#define HEARTBEAT_FREQUENCY (2000)

const char* MQTT_TOPIC_HEARTBEAT = "infrastructure/DEF_DEVICE_NAME/heartbeat";
const char* MQTT_TOPIC_RELAY_COMMAND = "ham/DEF_DEVICE_NAME/relay/command";
const char* MQTT_TOPIC_RELAY_STATUS = "ham/DEF_DEVICE_NAME/relay/status";

#define MSG_BUFFER_SIZE (500)
#define MSG_COMMAND_SIZE (10)

