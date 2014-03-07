
/*
 Basic MQTT example 
 
  - connects to an MQTT server
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Modbus_RTU.h>

// Create client and server devices
Modbus_RTU myServer = Modbus_RTU(0);
Modbus_RTU myClient = Modbus_RTU(0);

// Define  message data buffers 
Modbus_Frame myFrame;
Modbus_Data myData;

unsigned short CRC16;

t_baud Baudrate;
unsigned long MsgTimeout;
int i;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 172, 16, 0, 2 };
byte ip[]     = { 172, 16, 0, 100 };

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

void setup()
{
  Ethernet.begin(mac, ip);
  if (client.connect("arduinoClient")) {
    client.publish("outTopic","hello world");
    client.subscribe("inTopic");
  }
}

void loop()
{
  client.loop();
}


