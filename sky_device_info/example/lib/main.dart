import 'dart:convert';

import 'package:flutter/material.dart';
import 'dart:async';

import 'package:sky_device_info/beans.dart';
import 'package:sky_device_info/sky_device_info.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await SkyDeviceInfo().loadDeviceInfo();
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  final _skyDeviceInfoPlugin = SkyDeviceInfo();
  String? _deviceInfo;
  String? _networkInfo;

  @override
  void initState() {
    super.initState();
    loadDeviceInfo();
  }

  @override
  void dispose() {
    super.dispose();
    _skyDeviceInfoPlugin.release();
  }

  String formatJson(Object? object) {
    return jsonEncode(object)
        .replaceAll('{', '{\n')
        .replaceAll('}', '\n}')
        .replaceAll(',', ',\n');
  }

  loadDeviceInfo() async {
    DeviceInfo? deviceInfo = await _skyDeviceInfoPlugin.loadDeviceInfo();
    setState(() {
      _deviceInfo = formatJson(deviceInfo ?? 'Unknown');
    });
    _skyDeviceInfoPlugin.addNetworkCallback((NetworkInfo? info) {
      NetworkInfo? networkInfo = _skyDeviceInfoPlugin.networkInfo;
      setState(() {
        _networkInfo = formatJson(networkInfo ?? 'Unknown');
      });
    });
  }

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

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Container(
          padding: const EdgeInsets.all(20),
          child: Center(
            child: SingleChildScrollView(
              child:
                  Text('DeviceInfo: $_deviceInfo\nNetworkInfo: $_networkInfo'),
            ),
          ),
        ),
      ),
    );
  }
}
