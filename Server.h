#ifndef SERVER_H
#define SERVER_H

#include "GLOBALS.h";
#include "Storage.h"

void debugRequest(AsyncWebServerRequest *request) {
    size_t params = request->params();
    for(size_t i = 0; i < params; ++i) {
      auto param = request->getParam(i);
      Serial.printf("%s%s=%s", i == 0 ? String("?") : String("&"), param->name().c_str(), param->value().c_str());
    }
    Serial.println();
    auto headers = request->getHeaders();
    for(auto it = headers.begin(); it != headers.end(); ++it) {
      Serial.printf("  %s=%s\n", it->name().c_str(), it->value().c_str());
    }
    Serial.println();
}

// Handle 404
void notFound(AsyncWebServerRequest *request)
{
  if (showCaptivePortal)
  {
    Serial.printf("Captive Portal: %s", request->url().c_str());
    // debugRequest(request);
    // captive portal
    if(request->url() == "/done") {
      request->send(204);
    }
    else {
      server.sendStatic(request, &FILE_CAPTIVE_PORTAL_HTML);
    }
    return;
  }
  Serial.print(F("404: "));
  Serial.println(request->url().c_str());
  request->send(404, "text/plain", "404: Not found!");
}

void setupWebServer()
{
  Serial.println("Creating WebServer Handlers...");
  server.onBuffer("/api/settings/wifi", AsyncBufferType::WIFISETTINGS, (uint8_t *)&settings.wifi, sizeof(settings.wifi), nullptr, [](AsyncWebServerRequest *request)
              {
                // settings data have been updated you can do something
                // here with the modified data before sending the
                // response back to the client.
                saveSettings(); // save settings
                Serial.printf("POST: %s OK!\n", request->url().c_str());
                return true; // send the response
              });
  server.onBuffer("/api/settings/app", AsyncBufferType::APPSETTINGS, (uint8_t *)&settings.app, sizeof(settings.app), nullptr, [](AsyncWebServerRequest *request)
              {
                // settings data have been updated you can do something
                // here with the modified data before sending the
                // response back to the client.
                saveSettings(); // save settings
                Serial.printf("POST: %s OK!\n", request->url().c_str());
                return true; // send the response
              });
  server.onBuffer("/api/settings", AsyncBufferType::SETTINGS, (uint8_t *)&settings, sizeof(settings), nullptr, [](AsyncWebServerRequest *request)
              {
                // settings data have been updated you can do something
                // here with the modified data before sending the
                // response back to the client.
                saveSettings(); // save settings
                Serial.printf("POST: %s OK!\n", request->url().c_str());
                return true; // send the response
              });

  Serial.println("Done!\n");

  Serial.println("Creating WebSocket Handlers...");
  // websocket listeners
  server.addHandler(&ws);
  ws.onBuffer("settings", AsyncBufferType::SETTINGS, (uint8_t *)&settings, sizeof(settings), [](AsyncWebSocketClientBuffer *client, String command, AsyncBufferType type, uint8_t *data, size_t len, AsyncWebSocketBufferStatus status)
              {
                Serial.printf("%s settings\n", status == AsyncWebSocketBufferStatus::SET ? "SET" : "GET");
                return true; // send response?
              });
  ws.onBuffer("fps", AsyncBufferType::UINT8_T, [](AsyncWebSocketClientBuffer *client, String command, AsyncBufferType type, uint8_t *data, size_t len, AsyncWebSocketBufferStatus status)
              {
                // validate fps value before setting
                if (status == AsyncWebSocketBufferStatus::SET)
                {
                  uint8_t value = data[0];
                  if (value > 0)
                  {
                    settings.app.fps = value;
                  }
                  nextFrame = 0; // trigger next frame
                }
                Serial.printf("%s fps %d\n", status == AsyncWebSocketBufferStatus::SET ? "SET" : "GET", settings.app.fps);
                return true; // send response?
              });
  ws.onBuffer("close", AsyncBufferType::SETTINGS, (uint8_t *)&settings, sizeof(settings), [](AsyncWebSocketClientBuffer *client, String command, AsyncBufferType type, uint8_t *data, size_t len, AsyncWebSocketBufferStatus status)
              {
                Serial.printf("Client #%d: closed\n", client->id());
                client->close();
                return false; // send response?
              });
  ws.onBuffer("commands", [](AsyncWebSocketClientBuffer *client, String command, AsyncBufferType type, uint8_t *data, size_t len, AsyncWebSocketBufferStatus status)
              {
                String msg = "";
                AsyncWebSocketBufferCommandList *commands = ws.getCommands();
                for (auto it = commands->begin(); it != commands->end(); ++it)
                {
                  AsyncWebSocketBufferCommand *c = it->get();
                  msg += c->command + ":" + c->type + ",";
                }
                if (msg.length() > 0)
                {
                  msg[msg.length() - 1] = '\0'; // remove last comma
                }
                client->sendBuffer("commands", AsyncBufferType::CHAR, (uint8_t *)msg.c_str(), msg.length());
                return false; // send response?
              });
  ws.onBuffer("stream", AsyncBufferType::UINT8_T, [](AsyncWebSocketClientBuffer *client, String command, AsyncBufferType type, uint8_t *data, size_t len, AsyncWebSocketBufferStatus status)
              {
                uint8_t value = 0;
                if (status == AsyncWebSocketBufferStatus::SET)
                {
                  // fps value or off
                  value = data[0];
                  if (value > 0)
                  {
                    settings.app.fps = value;
                    if (streamClientId != 0 && streamClientId != client->id())
                    {
                      // stopping the previous stream
                      Serial.printf("Client #%d: stop stream\n", streamClientId);
                    }
                    streamClientId = client->id();
                    nextFrame = 0; // trigger next frame
                    Serial.printf("Client #%d: start stream @ %dfps\n", client->id(), settings.app.fps);
                  }
                  else
                  {
                    Serial.printf("Client #%d: stop stream\n", client->id());
                    streamClientId = 0;
                  }
                }
                else if (status == AsyncWebSocketBufferStatus::GET)
                {
                  // return who is currently being streamed to
                  client->sendBuffer("stream", AsyncBufferType::UINT32_T, (uint8_t *)streamClientId, sizeof(streamClientId));
                }
                return false; // send response?
              });
  Serial.println("Done!\n");
  
  Serial.println("Starting the server...");
  // CORS only needed for STA mode
  server.disableCORS(); // useful for local development
  server.onNotFound(notFound);
  server.begin();
  Serial.println("Done!\n");
}


StreamData data; // for the data stream response payload
void serverLoop()
{
  ws.cleanupClients(); // cleanup any disconnected clients
  if (streamClientId)
  {
    // You could provide a list of client IDs that have requested the data stream
    // but that is outside the scope of this example.
    // Or you could also just stream to all connected clients when any of the clients starts the stream.
    // For demonstration, stream to one client at a time.
    AsyncWebSocketClientBuffer *c = ws.client(streamClientId);
    if (c != nullptr && c->status() == AwsClientStatus::WS_CONNECTED)
    {
      // stream data to client who asked for it.
      data.id = c->id();
      data.clients = ws.count();
      data.frame = frame;
      data.fps = 1000000.0F / (float)(now - lastFrameTime);
      data.time = millis();
      ws.sendBuffer(streamClientId, "data", AsyncBufferType::STREAMDATA, (uint8_t *)&data, sizeof(data));
    }
  }
}

#endif