#ifndef FLUTTER_PLUGIN_SKY_DEVICE_INFO_PLUGIN_H_
#define FLUTTER_PLUGIN_SKY_DEVICE_INFO_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace sky_device_info {

class SkyDeviceInfoPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  SkyDeviceInfoPlugin();

  virtual ~SkyDeviceInfoPlugin();

  // Disallow copy and assign.
  SkyDeviceInfoPlugin(const SkyDeviceInfoPlugin&) = delete;
  SkyDeviceInfoPlugin& operator=(const SkyDeviceInfoPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

struct TParam {
    bool load_network_info;
    bool exclude_virtual_adapter;
};

}  // namespace sky_device_info

#endif  // FLUTTER_PLUGIN_SKY_DEVICE_INFO_PLUGIN_H_
