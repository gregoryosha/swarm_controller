#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

class Car
{
private:
  //Front Motor Driver Connections
  int r_front_motorB1 = 2; //HIGH is forward 19 21
  int r_front_motorB2 = 15; 
  int l_front_motorA1 = 21;
  int l_front_motorA2 = 19; //HIGH is forward

  //Back Motor Driver Connections
  int r_back_motorB1 = 18; //HIGH is forward
  int r_back_motorB2 = 4;
  int l_back_motorA1 = 23; 
  int l_back_motorA2 = 22; //HIGH is forward

public:
  Car()
  {
    // Set all pins to output
    pinMode(r_front_motorB1, OUTPUT);
    pinMode(r_front_motorB2, OUTPUT);
    pinMode(l_front_motorA1, OUTPUT);
    pinMode(l_front_motorA2, OUTPUT);

    // Set initial motor state to OFF
    digitalWrite(r_front_motorB1, LOW);
    digitalWrite(r_front_motorB2, LOW);
    digitalWrite(l_front_motorA1, LOW);
    digitalWrite(l_front_motorA2, LOW);

    // Set all pins to output
    pinMode(r_back_motorB1, OUTPUT);
    pinMode(r_back_motorB2, OUTPUT);
    pinMode(l_back_motorA1, OUTPUT);
    pinMode(l_back_motorA2, OUTPUT);

    // Set initial motor state to OFF
    digitalWrite(r_back_motorB1, LOW);
    digitalWrite(r_back_motorB2, LOW);
    digitalWrite(l_back_motorA1, LOW);
    digitalWrite(l_back_motorA2, LOW);

  }

  // Turn the car right
  void turnRight()
  {
    Serial.println("car is turning right...");
    //Front Motors
    digitalWrite(r_front_motorB1, LOW);
    digitalWrite(r_front_motorB2, HIGH);
    digitalWrite(l_front_motorA1, LOW);
    digitalWrite(l_front_motorA2, HIGH);
    
    //Back Motors
    digitalWrite(r_back_motorB1, LOW);
    digitalWrite(r_back_motorB2, HIGH);
    digitalWrite(l_back_motorA1, LOW);
    digitalWrite(l_back_motorA2, HIGH);
  }

  // Turn the car left
  void turnLeft()
  {
    //Front Motors
    Serial.println("car is turning left...");
    digitalWrite(r_front_motorB1, HIGH);
    digitalWrite(r_front_motorB2, LOW);
    digitalWrite(l_front_motorA1, HIGH);
    digitalWrite(l_front_motorA2, LOW);

    //Back Motors
    digitalWrite(r_back_motorB1, HIGH);
    digitalWrite(r_back_motorB2, LOW);
    digitalWrite(l_back_motorA1, HIGH);
    digitalWrite(l_back_motorA2, LOW);
  }

  // Move the car forward
  void moveForward()
  {
    //Front Motors
    Serial.println("car is moving forward...");
    digitalWrite(r_front_motorB1, HIGH);
    digitalWrite(r_front_motorB2, LOW);
    digitalWrite(l_front_motorA1, LOW);
    digitalWrite(l_front_motorA2, HIGH);

    //Back Motors
    digitalWrite(r_back_motorB1, HIGH);
    digitalWrite(r_back_motorB2, LOW);
    digitalWrite(l_back_motorA1, LOW);
    digitalWrite(l_back_motorA2, HIGH);
  }

  // Move the car backward
  void moveBackward()
  {
    //Front Motors
    Serial.println("car is moving backward...");
    digitalWrite(r_front_motorB1, LOW);
    digitalWrite(r_front_motorB2, HIGH);
    digitalWrite(l_front_motorA1, HIGH);
    digitalWrite(l_front_motorA2, LOW);

    //Back Motors
    digitalWrite(r_back_motorB1, LOW);
    digitalWrite(r_back_motorB2, HIGH);
    digitalWrite(l_back_motorA1, HIGH);
    digitalWrite(l_back_motorA2, LOW);
  }

  // Stop the car
  void stop()
  {
    Serial.println("car is stopping...");
    // // Turn off motors
    digitalWrite(r_front_motorB1, LOW);
    digitalWrite(r_front_motorB2, LOW);
    digitalWrite(l_front_motorA1, LOW);
    digitalWrite(l_front_motorA2, LOW);

    digitalWrite(r_back_motorB1, LOW);
    digitalWrite(r_back_motorB2, LOW);
    digitalWrite(l_back_motorA1, LOW);
    digitalWrite(l_back_motorA2, LOW);
  }
};

// Change this to your network SSID
const char *ssid = "Tufts_Wireless";
const char *password = "";

// AsyncWebserver runs on port 80 and the asyncwebsocket is initialize at this point also
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Our car object
Car car;

// Function to send commands to car
void sendCarCommand(const char *command)
{
  // command could be either "left", "right", "forward" or "reverse" or "stop"
  // or speed settingg "slow-speed", "normal-speed", or "fast-speed"
  if (strcmp(command, "left") == 0)
  {
    car.turnLeft();
  }
  else if (strcmp(command, "right") == 0)
  {
    car.turnRight();
  }
  else if (strcmp(command, "up") == 0)
  {
    car.moveForward();
  }
  else if (strcmp(command, "down") == 0)
  {
    car.moveBackward();
  }
  else if (strcmp(command, "stop") == 0)
  {
    car.stop();
  }
}

// Callback function that receives messages from websocket client
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
  {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    // client->printf("Hello Client %u :)", client->id());
    // client->ping();
  }

  case WS_EVT_DISCONNECT:
  {
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  }

  case WS_EVT_DATA:
  {
    //data packet
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len)
    {
      //the whole message is in a single frame and we got all of it's data
      if (info->opcode == WS_TEXT)
      {
        data[len] = 0;
        char *command = (char *)data;
        sendCarCommand(command);
      }
    }
  }

  case WS_EVT_PONG:
  {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  }

  case WS_EVT_ERROR:
  {
    // Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  }
}

// Function called when resource is not found on the server
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

// Setup function
void setup()
{
  // Initialize the serial monitor baud rate
  Serial.begin(115200);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  // Connect to your wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Add callback function to websocket server
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Serial.println("Requesting index page...");
              request->send(SPIFFS, "/index.html", "text/html", false);
            });

  // Route to load entireframework.min.css file
  server.on("/css/entireframework.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/css/entireframework.min.css", "text/css"); });

  // Route to load custom.css file
  server.on("/css/custom.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/css/custom.css", "text/css"); });

  // Route to load custom.js file
  server.on("/js/custom.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/js/custom.js", "text/javascript"); });
            
  // Route to load swarm_chan.jpg file
  server.on("/swarm_chan.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/swarm_chan.jpg", "image/jpg"); });

  // On Not Found
  server.onNotFound(notFound);

  // Start server
  server.begin();
}

void loop()
{
  // No code in here.  Server is running in asynchronous mode
}
