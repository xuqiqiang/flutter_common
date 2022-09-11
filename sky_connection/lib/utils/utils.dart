import 'package:sky_device_info/beans.dart';
import 'package:sky_device_info/sky_device_info.dart';

final _skyDeviceInfoPlugin = SkyDeviceInfo();

Future<String?> getIntranetIp() async {
  DeviceInfo? deviceInfo = await _skyDeviceInfoPlugin.loadDeviceInfo();
  if (deviceInfo != null) {
    NetworkInfo? networkInfo = _skyDeviceInfoPlugin.networkInfo;
    if (networkInfo != null && networkInfo.networkAdapters.isNotEmpty) {
      return networkInfo.networkAdapters.first.ipAddress;
    }
  }
  return null;
}
