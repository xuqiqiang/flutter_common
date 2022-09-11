import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:path_provider/path_provider.dart';

class CommonConfig {
  static bool? inDebugMode;
}

mixin CommonUtils {
  bool get inDebugMode {
    if (CommonConfig.inDebugMode != null) return CommonConfig.inDebugMode!;
    bool _inDebugMode = false;
    assert(_inDebugMode = true);
    return _inDebugMode;
  }

  void log(Object? object, {String? tag, bool? write}) {
    if (!inDebugMode) return;
    String text = tag != null ? '$tag $object' : '$object';
    // ignore: avoid_print
    print(text);
    if (write == true) {
      writeLog('${DateTime.now()} $text');
    }
  }

  Future writeLog(Object? object) async {
    File logFile = File('${(await getCachePath())}${getSep()}log.txt');
    await logFile.writeAsString('$object\n', mode: FileMode.append);
  }

  Map<String, dynamic> filter(Map<String, dynamic> map) {
    var newMap = <String, dynamic>{};
    map.forEach((key, value) {
      if (value != null) newMap[key] = value;
    });
    return newMap;
  }

  Future<Directory> getCacheDir({String? subDir}) async {
    Directory? tempDir;
    if (Platform.isAndroid) {
      tempDir = (await getExternalCacheDirectories())?.first;
    }
    tempDir ??= await getTemporaryDirectory();
    return subDir == null
        ? tempDir
        : Directory('${tempDir.path}${getSep()}$subDir');
  }

  Future<String> getCachePath({String? subDir}) async {
    Directory dir = await getCacheDir(subDir: subDir);
    if (!(await dir.exists())) await dir.create(recursive: true);
    return dir.path;
  }

  String getSep() {
    return !kIsWeb && Platform.isWindows ? "\\" : "/";
  }
}

class CommonObject with CommonUtils {
  CommonObject._();

  static final CommonObject instance = CommonObject._();
}

final commonUtils = CommonObject.instance;