# sky_device_info
A Flutter plugin to get device info.


## Usage
Quick simple usage example:

#### Read network info
```dart
final _plugin = SkyDeviceInfo();

DeviceInfo? deviceInfo = await _plugin.loadDeviceInfo();
if (deviceInfo != null) {
  NetworkInfo? networkInfo = _plugin.networkInfo;
  if (networkInfo != null && networkInfo.networkAdapters.isNotEmpty) {
    String? ipAddress = networkInfo.networkAdapters.first.ipAddress;
    String? connectionName =
        networkInfo.networkAdapters.first.connectionName;
    String? ssid = networkInfo.networkAdapters.first.ssid;
  } else {
    // Network disconnected
  }
}
```

<figure class="half">
    <img src="https://raw.githubusercontent.com/xuqiqiang/sky_device_info/main/example_android.png" width="300px" alt="Screenshot_1" />
</figure>
<img src="https://raw.githubusercontent.com/xuqiqiang/sky_device_info/main/example_windows.png" alt="Screenshot_2" />