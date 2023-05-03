#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

class Car
{
private:

  const int MOTOR_PINS[2][4] = {
    {2, 15, 21, 19}, //front pins (right, left)
    {18, 4, 23, 22}  //back pins (right, left)
  };
  //Alvin pins = {4, 16, 2, 15}, {18, 19, 17, 5}
  //Theodore pins = {4, 16, 2, 15}, {18, 19, 17, 5}
  //Simmon pins = {2, 15, 21, 19}, {18, 4, 23, 22} 

public:
  Car()
  {

    for (int i=0; i<2; i++){
      for (int j=0; j<4; j++) {
        pinMode(MOTOR_PINS[i][j], OUTPUT); //Sets all motor to output
        digitalWrite(MOTOR_PINS[i][j], LOW); //initially off
      }
    }

  }

  void writeMotors(bool motor_state[2][4]) 
  {
    for (int i=0; i<2; i++){
      for (int j=0; j<4; j++) {
        if (motor_state[i][j]){ //If the function reads a desired input, it will write that pin HIGH
          digitalWrite(MOTOR_PINS[i][j], HIGH); 
        }
        else  {
          digitalWrite(MOTOR_PINS[i][j], LOW);
        }
      }
    }
  }

  // Turn the car right
  void turnRight()
  {
    Serial.println("car is turning right...");
    bool motor_right[2][4] = { {0, 1, 0, 1}, {0, 1, 0, 1} }; //Right motors are written HIGH
    writeMotors(motor_right);
  }

  // Turn the car left
  void turnLeft()
  {
    //Front Motors
    Serial.println("car is turning left...");
    bool motor_left[2][4] = { {1, 0, 1, 0}, {1, 0, 1, 0} }; //Left motors are written HIGH
    writeMotors(motor_left);
  }

  // Move the car forward
  void moveForward()
  {
    //Front Motors
    Serial.println("car is moving forward...");
    bool motor_fwd[2][4] = { {1, 0, 0, 1}, {1, 0, 0, 1} }; //All motors driven forward
    writeMotors(motor_fwd);
  }

  // Move the car backward
  void moveBackward()
  {
    //Front Motors
    Serial.println("car is moving backward...");
    bool motor_back[2][4] = { {0, 1, 1, 0}, {0, 1, 1, 0} }; //All motors driven backward
    writeMotors(motor_back);
  }

  // Stop the car
  void stop()
  {
    Serial.println("car is stopping...");
    // // Turn off motors
    bool motor_stop[2][4] = { {0, 0, 0, 0}, {0, 0, 0, 0} }; //All motors driven backward
    writeMotors(motor_stop);
  }
};

// Our car object
Car swarmbot;

// Function to send commands to car
void sendCarCommand(const char *command)
{
  // command could be either "left", "right", "forward" or "reverse" or "stop"
  // or speed settingg "slow-speed", "normal-speed", or "fast-speed"
  if (strcmp(command, "left") == 0)
  {
    swarmbot.turnLeft();
  }
  else if (strcmp(command, "right") == 0)
  {
    swarmbot.turnRight();
  }
  else if (strcmp(command, "up") == 0)
  {
    swarmbot.moveForward();
  }
  else if (strcmp(command, "down") == 0)
  {
    swarmbot.moveBackward();
  }
  else if (strcmp(command, "stop") == 0)
  {
    swarmbot.stop();
  }
}

// Server name 
const char* ssid = "Theodore_swarmbot";
const char* password = "rlblabpassword"; 

// AsyncWebserver runs on port 80 and the asyncwebsocket is initialize at this point also
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

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
  Serial.begin(115200); // basic setup for serta communication when ESp32 is connected to pc via USB
  
  // Setting up the ESP32 Hosted wifi (IE you connect to a network being produced by the ESP32) 
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: "); Serial.println(IP); // displays the IP address for the ESP32, by default it is 192.168.4.1
  // you can confirm the IP by plugging in the ESP32, opening the serial monitor then resetting the ESP32, the above lines print the IP address

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

  // Route to load custom.css file
  server.on("/css/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/css/styles.css", "text/css"); });

  // Route to load custom.js file
  server.on("/js/custom.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/js/custom.js", "text/javascript"); });
            
  // Route to load swarm_chan.jpg file
  server.on("/profile.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/profile.jpg", "image/jpg"); });

  // On Not Found
  server.onNotFound(notFound);

  // Start server
  server.begin();
}

void loop()
{
  // No code in here.  Server is running in asynchronous mode
}
