import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'sky_device_info_method_channel.dart';

abstract class SkyDeviceInfoPlatform extends PlatformInterface {
  /// Constructs a SkyDeviceInfoPlatform.
  SkyDeviceInfoPlatform() : super(token: _token);

  static final Object _token = Object();

  static SkyDeviceInfoPlatform _instance = MethodChannelSkyDeviceInfo();

  /// The default instance of [SkyDeviceInfoPlatform] to use.
  ///
  /// Defaults to [MethodChannelSkyDeviceInfo].
  static SkyDeviceInfoPlatform get instance => _instance;
  
  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [SkyDeviceInfoPlatform] when
  /// they register themselves.
  static set instance(SkyDeviceInfoPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
