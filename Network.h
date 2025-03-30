#ifndef NETWORK_H
#define NETWORK_H

#include "GLOBALS.h";


bool connectToWiFi()
{
  if (WiFi.status() == wl_status_t::WL_CONNECTED)
  {
    WiFi.disconnect();
  }
  if (strlen(settings.wifi.ssid) != 0)
  {
    WiFi.begin(settings.wifi.ssid, settings.wifi.password);
    static uint8_t failedAttempts = 0;
    while (WiFi.status() != WL_CONNECTED && failedAttempts < 3)
    {
      // attempting to join STA
      delay(1000);
      Serial.printf("Connecting to %s...\n", settings.wifi.ssid);
      failedAttempts++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      IP = WiFi.localIP();
      Serial.println("WiFi Connected!");
    }
    else
    {
      Serial.println("Failed to connect!");
    }
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    // failed to connect to STA, start AP
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    showCaptivePortal = true;
    bool success;
    Serial.printf("\nCreating AP %s...\n", settings.wifi.apSsid);
    if(!WiFi.softAPConfig(local_ip, gateway, subnet)) {
      Serial.println("Failed to configure AP!");
    }
    else {
      Serial.println("AP Config Successful!");
    }
    if (strlen(settings.wifi.apPassword) == 0)
    {
      String ssid = WIFI_AP_NAME + (WiFi.macAddress().substring(12));
      ssid.replace(":", "");
      memcpy(&settings.wifi.apSsid, ssid.c_str(), ssid.length());
      success = WiFi.softAP(settings.wifi.apSsid);
    }
    else
    {
      Serial.printf("\nCreating AP %s...\n", settings.wifi.apSsid);
      success = WiFi.softAP(settings.wifi.apSsid, settings.wifi.apPassword);
    }
    if (!success)
    {
      Serial.println("Soft AP creation failed. Try Rebooting...");
      return false;
      while (1);
    }
    Serial.println("AP created!");
    IP = WiFi.softAPIP();
  }
  String strIP = IP.toString();
  Serial.println(strIP);
  settings.wifi.mode = WiFi.getMode();
  memcpy(&settings.wifi.ip, strIP.c_str(), strIP.length());

  // set up dns for captive portal
  Serial.println("\nCreating DNS...");
  dnsServer.start(53, "*", IP);
  Serial.println("DNS created!");
  return true;
}

void networkLoop()
{
  dnsServer.processNextRequest(); // for captive portal
}

#endif