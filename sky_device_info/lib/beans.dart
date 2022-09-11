import 'package:sky_device_info/utils.dart';

part 'beans.g.dart';

// @JsonSerializable()
class DeviceInfo {
  DeviceInfo({
    this.deviceName,
    this.osName,
    this.monitorName,
    this.gpu = const [],
  });

  String? deviceName;
  String? osName;
  String? monitorName;
  List<GPUInfo> gpu;

  factory DeviceInfo.fromJson(Map<String, dynamic> srcJson) =>
      _$DeviceInfoFromJson(srcJson);

  Map<String, dynamic> toJson() => filter(_$DeviceInfoToJson(this));
}

// @JsonSerializable()
class GPUInfo {
  GPUInfo({this.name, this.version});

  String? name;
  String? version;

  factory GPUInfo.fromJson(Map<String, dynamic> srcJson) =>
      _$GPUInfoFromJson(srcJson);

  Map<String, dynamic> toJson() => filter(_$GPUInfoToJson(this));
}

// @JsonSerializable()
class NetworkInfo {
  NetworkInfo({required this.networkAdapters});

  List<NetworkAdapter> networkAdapters;

  factory NetworkInfo.fromJson(Map<String, dynamic> srcJson) =>
      _$NetworkInfoFromJson(srcJson);

  Map<String, dynamic> toJson() => filter(_$NetworkInfoToJson(this));

  @override
  String toString() {
    return '{networkAdapters: $networkAdapters}';
  }
}

// @JsonSerializable()
class NetworkAdapter {
  final int ifTypeIEEE80211 = 71; // Radio spread spectrum
  String? name;
  String? id;
  int? index;
  int? bandwidth;
  String? connectionName;
  int? connectionStatus;
  String? ssid;
  bool? enabled;
  int? type; // 网卡类型
  String? macAddress; // 网卡MAC地址
  String? ipAddress; // IP 地址
  String? ipAddressIPv6; // IPv6地址
  String? ipMaskAddress; // 子网地址
  String? gatewayIpAddress; // 网关地址
  String? ipBroadcast; // 子网地址

  NetworkAdapter(
      {this.name,
      this.id,
      this.index,
      this.bandwidth,
      this.connectionName,
      this.connectionStatus,
      this.ssid,
      this.enabled,
      this.type,
      this.macAddress,
      this.ipAddress,
      this.ipAddressIPv6,
      this.ipMaskAddress,
      this.gatewayIpAddress,
      this.ipBroadcast});

  factory NetworkAdapter.fromJson(Map<String, dynamic> srcJson) =>
      _$NetworkAdapterFromJson(srcJson);

  Map<String, dynamic> toJson() => filter(_$NetworkAdapterToJson(this));

  bool isWireless() => type == ifTypeIEEE80211;

  @override
  String toString() =>
      '{name: $name, id: $id, index: $index, bandwidth: $bandwidth,'
      ' connectionName: $connectionName, connectionStatus: $connectionStatus,'
      ' ssid: $ssid, enabled: $enabled, type: $type, macAddress: $macAddress,'
      ' ipAddress: $ipAddress, ipMaskAddress: $ipMaskAddress, gatewayIpAddress: $gatewayIpAddress}';
}
