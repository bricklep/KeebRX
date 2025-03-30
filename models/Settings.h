// Settings.h
#ifndef Settings_H
#define Settings_H

#pragma pack(1)
struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  String toString() {
    return String(r) + "," + String(g) + "," + String(b);
  }
};

#pragma pack(1)
struct SubSetting
{
  uint8_t id;
  bool enabled;
  unsigned long value;
  Color color;
};

#pragma pack(1)
struct WiFiSettings {
  char ssid[32] = "";
  char password[63] = "";
  char apSsid[32] = "";
  char apPassword[63] = "";
  char ip[15] = "0.0.0.0";
  uint8_t mode = 0;
};

#pragma pack(1)
struct AppSettings {
  char version[12] = "v1.0.0";
  uint8_t fps = 1;
  SubSetting subSettings[5]; // Array of structs
};

#pragma pack(1)
struct Settings {
  WiFiSettings wifi;
  AppSettings app;
};


struct StreamData {
  uint32_t id = 0;
  size_t clients = 0;
  int frame = 0;
  float fps = 0;
  unsigned long time = 0;
};

#endif