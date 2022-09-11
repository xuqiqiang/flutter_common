package com.ssnwt.sky_device_info

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.provider.Settings
import androidx.annotation.NonNull
import androidx.core.content.ContextCompat

import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result

/** SkyDeviceInfoPlugin */
class SkyDeviceInfoPlugin: FlutterPlugin, MethodCallHandler {
  /// The MethodChannel that will the communication between Flutter and native Android
  ///
  /// This local reference serves to register the plugin with the Flutter Engine and unregister it
  /// when the Flutter Engine is detached from the Activity
  private lateinit var channel : MethodChannel
  private var context: Context? = null

  override fun onAttachedToEngine(@NonNull flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
    channel = MethodChannel(flutterPluginBinding.binaryMessenger, "sky_device_info")
    channel.setMethodCallHandler(this)
    context = flutterPluginBinding.getApplicationContext()
  }

  override fun onMethodCall(@NonNull call: MethodCall, @NonNull result: Result) {
    if (call.method == "loadDeviceInfo") {
      var deviceName = ""
      if (ContextCompat.checkSelfPermission(context!!, Manifest.permission.BLUETOOTH_CONNECT)
        === PackageManager.PERMISSION_GRANTED
      ) {
        deviceName = BluetoothAdapter.getDefaultAdapter().name
      } else {
        deviceName = Settings.Secure.getString(context?.getContentResolver(), "bluetooth_name")
      }
      var osName = "Android ${Build.VERSION.RELEASE}"//Build.DISPLAY
      result.success("{\"deviceName\":\"$deviceName\",\"osName\":\"$osName\"}")
    } else if (call.method == "getPlatformVersion") {
      result.success("Android ${Build.VERSION.RELEASE}")
    } else {
      result.notImplemented()
    }
  }

  override fun onDetachedFromEngine(@NonNull binding: FlutterPlugin.FlutterPluginBinding) {
    channel.setMethodCallHandler(null)
  }
}
