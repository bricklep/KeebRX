#ifndef STORAGE_H
#define STORAGE_H

#include "GLOBALS.h";

void loadSettings()
{
  prefs.begin("settings", false);
  if (prefs.isKey("settings"))
  {
    // only load settings if they exist
    if (sizeof(settings) == prefs.getBytesLength("settings"))
    {
      prefs.getBytes("settings", &settings, sizeof(settings));
    }
  }
  prefs.end();
}

void saveSettings()
{
  prefs.begin("settings", false);
  prefs.putBytes("settings", &settings, sizeof(settings));
  prefs.end();
}

void clearSettings()
{
  prefs.begin("settings", false);
  prefs.clear();
  prefs.end();
}

#endif