#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

class Car
{
private:
  // Motor 1 connections
  int in1 = 12;
  int in2 = 13;
  // Motor 2 connections
  int in3 = 32;
  int in4 = 33;

public:
  Car()
  {
    // Set all pins to output
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

    // Set initial motor state to OFF
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);

  }

  // Turn the car left
  void turnLeft()
  {
    Serial.println("car is turning left...");
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }

  // Turn the car right
  void turnRight()
  {
    Serial.println("car is turning right...");
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }

  // Move the car forward
  void moveForward()
  {
    Serial.println("car is moving forward...");
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }

  // Move the car backward
  void moveBackward()
  {
    Serial.println("car is moving backward...");
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }

  // Stop the car
  void stop()
  {
    Serial.println("car is stopping...");
    // // Turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
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

  // On Not Found
  server.onNotFound(notFound);

  // Start server
  server.begin();
}

void loop()
{
  // No code in here.  Server is running in asynchronous mode
}
