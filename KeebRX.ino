// Local Includes
#include "GLOBALS.h"
#include "Storage.h" // for loading and saving data 
#include "Network.h" // Wifi and DNS 
#include "Server.h" // WebServer routes and WebSocket commands

void setup() {
  Serial.begin(115200);
  loadSettings(); // load wifi and app settings
  connectToWiFi(); // Create or join a WiFi Network
  setupWebServer(); // Register WebServer routes and WebSocket commands
  while(!Serial);
  Serial.println("Test");
}

void loop()
{
  now = micros();

  networkLoop(); // handle DNS tasks
  
  if(now < nextFrame) return; // not time yet
  frame++;
  nextFrame = now + (1000000.0F / (float)(settings.app.fps));
  
  serverLoop(); // handle websocket data stream
  
  lastFrameTime = now;
}
