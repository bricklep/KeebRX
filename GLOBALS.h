// GLOBALS.h
#ifndef GLOBALS_H
#define GLOBALS_H

// Library Includes
#include <Arduino.h>
#include <Preferences.h>
#include <DNSServer.h>
#include <WiFi.h>

// Remember to generate the sources. For details visit the link below.
// https://github.com/sheaivey/ESP32AsyncBuffer#installation
#include "dist/_GENERATED_SOURCE.h" // must be included before 'AsyncWebServerBuffer.h'

#include "AsyncWebServerBuffer.h"
#include "AsyncWebSocketBuffer.h"

// Local Includes
#include "models/Settings.h"

// Storage
Preferences prefs;

// App globals
unsigned long now = 0;
unsigned long lastFrameTime = 0;
unsigned long nextFrame = 0;
int frame = 0;

Settings settings = {};

// WiFi Globals
static IPAddress local_ip(192, 168, 4, 1);
static IPAddress gateway(192, 168, 4, 1);
static IPAddress subnet(255, 255, 255, 0);
#define WIFI_AP_NAME "AsyncBufferAdvanced-"
DNSServer dnsServer;
IPAddress IP;
bool showCaptivePortal = false;

// AsyncWebServer
AsyncWebServerBuffer server(80);
AsyncWebSocketBuffer ws("/ws");
uint32_t streamClientId = 0;

#endif
