import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'sky_device_info_platform_interface.dart';

/// An implementation of [SkyDeviceInfoPlatform] that uses method channels.
class MethodChannelSkyDeviceInfo extends SkyDeviceInfoPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('sky_device_info');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
