#include "include/sky_device_info/sky_device_info_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "sky_device_info_plugin.h"

void SkyDeviceInfoPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  sky_device_info::SkyDeviceInfoPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
