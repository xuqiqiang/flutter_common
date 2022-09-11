#import "SkyDeviceInfoPlugin.h"

@implementation SkyDeviceInfoPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  FlutterMethodChannel* channel = [FlutterMethodChannel
      methodChannelWithName:@"sky_device_info"
            binaryMessenger:[registrar messenger]];
  SkyDeviceInfoPlugin* instance = [[SkyDeviceInfoPlugin alloc] init];
  [registrar addMethodCallDelegate:instance channel:channel];
}

#define AddStr(str1,str2)           [str1 stringByAppendingString:str2]

- (void)handleMethodCall:(FlutterMethodCall*)call result:(FlutterResult)result {
  if ([@"getPlatformVersion" isEqualToString:call.method]) {
    result([@"iOS " stringByAppendingString:[[UIDevice currentDevice] systemVersion]]);
  } else if ([@"loadDeviceInfo" isEqualToString:call.method]) {
    NSString* str = AddStr(@"{\"deviceName\":\"", [[UIDevice currentDevice] name]);
    str = AddStr(str, @"\",\"osName\":\"iOS ");
    str = AddStr(str, [[UIDevice currentDevice] systemVersion]);
    str = AddStr(str, @"\"}");
    result(str);
  } else {
    result(FlutterMethodNotImplemented);
  }
}

@end

/*
#import "SkyDeviceInfoPlugin.h"
#if __has_include(<sky_device_info/sky_device_info-Swift.h>)
#import <sky_device_info/sky_device_info-Swift.h>
#else
// Support project import fallback if the generated compatibility header
// is not copied when this plugin is created as a library.
// https://forums.swift.org/t/swift-static-libraries-dont-copy-generated-objective-c-header/19816
#import "sky_device_info-Swift.h"
#endif

@implementation SkyDeviceInfoPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftSkyDeviceInfoPlugin registerWithRegistrar:registrar];
}
@end
*/