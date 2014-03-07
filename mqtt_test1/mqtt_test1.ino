
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

// Defines 1 Client and 1 Server devices
Modbus_RTU myServer = Modbus_RTU(0);
Modbus_RTU myClient = Modbus_RTU(0);

// Defines a message and data buffers
Modbus_Frame myFrame;
Modbus_Data myData;

unsigned short CRC16;

t_baud Baudrate;
unsigned long MsgTimeout;
int i;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0x00 };
byte server[] = { 10, 10, 10, 2 };
byte ip[]     = { 10, 10, 10, 10 };

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

void setup()
{
  Ethernet.begin(mac, ip);
  if (client.connect("arduinoClient")) {
    client.publish("outTopic", "hello world");
    client.subscribe("inTopic");
  }
  // Force type for each device
  myServer.SetType(MDB_SERVER);
  myClient.SetType(MDB_CLIENT);

  // Preset Server address
  myServer.Server_SetAddress(5);

  // Initialize serial line
  myClient.GetBaudrate(&Baudrate);
  Serial.begin(Baudrate);

  // Get the inter-frame time (equivalent to 3,5 char)
  myClient.GetFrameTimeout(&MsgTimeout);

  Serial.println("");
  //Serial.println("Test Modbus_RTU library");
  //Serial.println("=======================");

  // Modbus test type
  Serial.println("");
  Serial.println("   Test Function code 03: Read Holding Registers");
  //Serial.println("   ---------------------------------------------");
}


void loop()
{
  client.loop(); //Handle MQTT
  
  Serial.println("");
  Serial.println("  --> Read 5 registers from address 10");
  Serial.println("      Result should be 0x1111, 0x2222, 0x3333, 0x4444, 0x5555");

  // Build Client request
  //Serial.println("");
  //Serial.println("  Request sent by the Client");
  myClient.Client_ReadHoldingRegisters(5, 10, 5, &myFrame);
  DisplayFrame(&myFrame);

  // Build Server response
  if (myServer.Server_Update(&myFrame))
  {
    //Serial.println("  --> OK Response available");
    //Serial.println("");
    //Serial.println("  Packet sent by the Server");
    DisplayFrame(&myFrame);

    // extract Data received by the Client
    myClient.Client_Update(&myFrame, &myData);
    Serial.println("");
    Serial.println("  Data");
    DisplayData(&myData);
  }
  else
    Serial.println("  --> No response available");

  while (1)
  {
  }
}


// Function to display a complete frame (Debug mode)
void DisplayFrame(Modbus_Frame* msg)
{
  int i;

  Serial.print("  Frame size ");
  Serial.print(msg->length, DEC);
  Serial.print(" -> ");
  if (msg->length > 0)
  {
    for (i = 0; i < msg->length; i++)
    {
      Serial.print((unsigned char)(msg->data[i]) >> 4, HEX);
      Serial.print((unsigned char)(msg->data[i]) & 0x0F, HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
}

// Function to display Data received
void DisplayData(Modbus_Data* Data)
{
  int i;

  Serial.print("    ==> Number of data received = ");
  Serial.println(Data->length, DEC);
  if (Data->length > 0)
  {
    Serial.print("        Data type = ");
    switch (Data->type)
    {
      case MDB_BIT:
        Serial.println("BIT");
        Serial.print("        Data = ");
        for (i = 0; i < Data->length; i++)
        {
          Serial.print(Data->data[i] & 1, DEC);
          Serial.print((Data->data[i] & 2) >> 1, DEC);
          Serial.print((Data->data[i] & 4) >> 2, DEC);
          Serial.print((Data->data[i] & 8) >> 3, DEC);
          Serial.print((Data->data[i] & 18) >> 4, DEC);
          Serial.print((Data->data[i] & 32) >> 5, DEC);
          Serial.print((Data->data[i] & 64) >> 6, DEC);
          Serial.print((Data->data[i] & 128) >> 7, DEC);
        }
        break;
      case MDB_BYTE:
        Serial.println("BYTE");
        Serial.print("        Data = ");
        for (i = 0; i < Data->length; i++)
        {
          Serial.print("0x");
          Serial.print((unsigned char)(Data->data[i]) >> 4, HEX);
          Serial.print((unsigned char)(Data->data[i]) & 0x0F, HEX);
          Serial.print(" ");
        }
        break;
      case MDB_WORD:
        Serial.println("WORD");
        Serial.print("        Data = ");
        for (i = 0; i < Data->length; i++)
        {
          Serial.print("0x");
          Serial.print((unsigned char)(Data->data[i * 2]) >> 4, HEX);
          Serial.print((unsigned char)(Data->data[i * 2]) & 0x0F, HEX);
          Serial.print((unsigned char)(Data->data[i * 2 + 1]) >> 4, HEX);
          Serial.print((unsigned char)(Data->data[i * 2 + 1]) & 0x0F, HEX);
          Serial.print(" ");
        }
        break;
      default:
        Serial.println("Unknown");
        break;
    }
    Serial.println();
  }
}

void DisplayDataOnly(Modbus_Data* Data)
{
  int i;
  if (Data->length > 0)
  {
    for (i = 0; i < Data->length; i++)
    {
      if (Data->type == MDB_BIT)
      {
        Serial.print(Data->data[i] & 1, DEC);
        Serial.print((Data->data[i] & 2) >> 1, DEC);
        Serial.print((Data->data[i] & 4) >> 2, DEC);
        Serial.print((Data->data[i] & 8) >> 3, DEC);
        Serial.print((Data->data[i] & 18) >> 4, DEC);
        Serial.print((Data->data[i] & 32) >> 5, DEC);
        Serial.print((Data->data[i] & 64) >> 6, DEC);
        Serial.print((Data->data[i] & 128) >> 7, DEC);
      }
      if (Data->type == MDB_BYTE)
      {
        Serial.print("0x");
        Serial.print((unsigned char)(Data->data[i]) >> 4, HEX);
        Serial.print((unsigned char)(Data->data[i]) & 0x0F, HEX);
        Serial.print(" ");
      }
      if (Data->type == MDB_WORD)
      {
        Serial.print("0x");
        Serial.print((unsigned char)(Data->data[i * 2]) >> 4, HEX);
        Serial.print((unsigned char)(Data->data[i * 2]) & 0x0F, HEX);
        Serial.print((unsigned char)(Data->data[i * 2 + 1]) >> 4, HEX);
        Serial.print((unsigned char)(Data->data[i * 2 + 1]) & 0x0F, HEX);
        Serial.print(" ");
      }
    }
    Serial.println();
  }
}


/******************************************************************************
*  Callback functions
*  Allow the user to define all device objects
******************************************************************************/

/******************************************************************************
* t_status Modbus_CB_GetRegister (unsigned short Addr, int* Value)
*     Callback function to read register value
* Parameters:
*     - Addr: Address of the register from 0x0000 to 0xFFFF
*     - Value: pointer to a variable which will contain the register value
* Return value:
*     - OK if register address exists
*     - NOK if register address doesn't exist
******************************************************************************/
t_status Modbus_CB_GetRegister(unsigned short Addr, int* Value)
{
  t_status Status = OK;

  // Value = f(Addr) To be defined by application
  switch (Addr)
  {
    case 10:
      *Value = 0x1111;
      break;
    case 11:
      *Value = 0x2222;
      break;
    case 12:
      *Value = 0x3333;
      break;
    case 13:
      *Value = 0x4444;
      break;
    case 14:
      *Value = 0x5555;
      break;
    default:
      Status = NOK;
  }
  return (Status);
}

