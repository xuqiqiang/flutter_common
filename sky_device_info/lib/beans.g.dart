// GENERATED CODE - DO NOT MODIFY BY HAND

part of 'beans.dart';

// **************************************************************************
// JsonSerializableGenerator
// **************************************************************************

DeviceInfo _$DeviceInfoFromJson(Map<String, dynamic> json) => DeviceInfo(
      deviceName: json['deviceName'] ?? '',
      osName: json['osName'] ?? '',
      monitorName: json['monitorName'] ?? '',
      gpu: ((json['gpu'] ?? []) as List<dynamic>)
          .map((e) => GPUInfo.fromJson(e as Map<String, dynamic>))
          .toList(),
    );

Map<String, dynamic> _$DeviceInfoToJson(DeviceInfo instance) =>
    <String, dynamic>{
      'deviceName': instance.deviceName,
      'osName': instance.osName,
      'monitorName': instance.monitorName,
      'gpu': instance.gpu,
    };

GPUInfo _$GPUInfoFromJson(Map<String, dynamic> json) => GPUInfo(
      name: json['name'] ?? '',
      version: json['version'] ?? '',
    );

Map<String, dynamic> _$GPUInfoToJson(GPUInfo instance) => <String, dynamic>{
      'name': instance.name,
      'version': instance.version,
    };

NetworkInfo _$NetworkInfoFromJson(Map<String, dynamic> json) => NetworkInfo(
      networkAdapters: ((json['networkAdapters'] ?? []) as List<dynamic>)
          .map((e) => NetworkAdapter.fromJson(e as Map<String, dynamic>))
          .toList(),
    );

Map<String, dynamic> _$NetworkInfoToJson(NetworkInfo instance) =>
    <String, dynamic>{
      'networkAdapters': instance.networkAdapters,
    };

NetworkAdapter _$NetworkAdapterFromJson(Map<String, dynamic> json) =>
    NetworkAdapter(
      name: json['name'] ?? '',
      id: json['id'] ?? '',
      index: json['index'] ?? -1,
      bandwidth: json['bandwidth'] ?? 0,
      connectionName: json['connectionName'] ?? '',
      connectionStatus: json['connectionStatus'] ?? -1,
      ssid: json['ssid'] ?? '',
      enabled: json['enabled'] ?? false,
      type: json['type'] ?? -1,
      macAddress: json['macAddress'] ?? '',
      ipAddress: json['ipAddress'] ?? '',
      ipAddressIPv6: json['ipAddressIPv6'] ?? '',
      ipMaskAddress: json['ipMaskAddress'] ?? '',
      gatewayIpAddress: json['gatewayIpAddress'] ?? '',
      ipBroadcast: json['ipBroadcast'] ?? '',
    );

Map<String, dynamic> _$NetworkAdapterToJson(NetworkAdapter instance) =>
    <String, dynamic>{
      'name': instance.name,
      'id': instance.id,
      'index': instance.index,
      'bandwidth': instance.bandwidth,
      'connectionName': instance.connectionName,
      'connectionStatus': instance.connectionStatus,
      'ssid': instance.ssid,
      'enabled': instance.enabled,
      'type': instance.type,
      'macAddress': instance.macAddress,
      'ipAddress': instance.ipAddress,
      'ipAddressIPv6': instance.ipAddressIPv6,
      'ipMaskAddress': instance.ipMaskAddress,
      'gatewayIpAddress': instance.gatewayIpAddress,
      'ipBroadcast': instance.ipBroadcast,
    };
