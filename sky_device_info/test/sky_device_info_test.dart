import 'package:flutter_test/flutter_test.dart';
import 'package:sky_device_info/sky_device_info.dart';
import 'package:sky_device_info/sky_device_info_platform_interface.dart';
import 'package:sky_device_info/sky_device_info_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockSkyDeviceInfoPlatform 
    with MockPlatformInterfaceMixin
    implements SkyDeviceInfoPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final SkyDeviceInfoPlatform initialPlatform = SkyDeviceInfoPlatform.instance;

  test('$MethodChannelSkyDeviceInfo is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelSkyDeviceInfo>());
  });

  // test('getPlatformVersion', () async {
  //   SkyDeviceInfo skyDeviceInfoPlugin = SkyDeviceInfo();
  //   MockSkyDeviceInfoPlatform fakePlatform = MockSkyDeviceInfoPlatform();
  //   SkyDeviceInfoPlatform.instance = fakePlatform;
  //
  //   expect(await skyDeviceInfoPlugin.getPlatformVersion(), '42');
  // });
}
