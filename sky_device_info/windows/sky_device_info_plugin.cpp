#include "wmi_info.h"
#include "sky_device_info_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <thread>

namespace sky_device_info {

static std::unique_ptr<flutter::MethodChannel<>> m_channel;

static bool IsCheckNetworkInfo = false;

DWORD WINAPI TCheckDeviceInfo(TParam* param) {
    bool load_network_info = param->load_network_info;
    bool exclude_virtual_adapter = param->exclude_virtual_adapter;
    delete param;
    PlatformInfo info{};
    info.QuerySystem();
    info.QueryGPU();

    std::string str = "{"
        + info.PrintSystem() + ","
        + info.PrintGPU() + "}";

    if (m_channel != NULL) {
        std::string result = GBK_2_UTF8(str);
        // std::cout << "CheckSystemInfo:" << result << std::endl;
        m_channel->InvokeMethod("onDeviceInfo",
            std::make_unique<flutter::EncodableValue>(result));
    }

    if (load_network_info) {
        // WMI的api只能在同一个线程中执行，所以保持此线程运行
        // 每隔5秒自动检测一次网络连接
        //Sleep(500);
        IsCheckNetworkInfo = true;
        while (IsCheckNetworkInfo) {
            info.QueryNetwork(exclude_virtual_adapter);
            std::string str = "{"
                + info.PrintNetwork() + "}";

            if (m_channel != NULL) {
                std::string result = str;//GBK_2_UTF8(str);
                //std::cout << "updateNetworkInfo:" << result << std::endl;
                m_channel->InvokeMethod("onNetworkInfo",
                    std::make_unique<flutter::EncodableValue>(result));
            }
            Sleep(5000);
        }
    }

    WMIWBEMINFO::deleteWMIWBEMINFOInstance();
    std::cout << "CheckSystemInfo finish" << std::endl;
    //::MessageBoxW(NULL, L"Dropped!", L"Dropped!", MB_ICONINFORMATION);
    return 0;
}

// static
void SkyDeviceInfoPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "sky_device_info",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<SkyDeviceInfoPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  m_channel = std::move(channel);
  registrar->AddPlugin(std::move(plugin));
}

SkyDeviceInfoPlugin::SkyDeviceInfoPlugin() {}

SkyDeviceInfoPlugin::~SkyDeviceInfoPlugin() {
    IsCheckNetworkInfo = false;
}

void SkyDeviceInfoPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  }
  else if (method_call.method_name().compare("loadDeviceInfo") == 0) {
      const flutter::EncodableMap& args =
          std::get<flutter::EncodableMap>(*method_call.arguments());
      TParam* param = new TParam;
      param->load_network_info = std::get<bool>(args.at(flutter::EncodableValue("loadNetworkInfo")));
      param->exclude_virtual_adapter = std::get<bool>(args.at(flutter::EncodableValue("excludeVirtualAdapter")));
      std::thread th(TCheckDeviceInfo, param);
      th.detach();
      result->Success();
  } else if (method_call.method_name().compare("release") == 0) {
      IsCheckNetworkInfo = false;
      result->Success();
  }
  else {
    result->NotImplemented();
  }
}

}  // namespace sky_device_info
