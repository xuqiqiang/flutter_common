package com.ssnwt.requester;

import static com.ssnwt.requester.utils.NotificationsUtils.isNotificationEnabled;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.provider.Settings;
import android.text.TextUtils;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;

import com.ssnwt.requester.proxy.PermissionActivity;
import com.ssnwt.requester.utils.PermissionPageUtils;

import java.util.ArrayList;
import java.util.List;

public class PermissionRequester {

    public static void request(Context context, OnPermissionListener listener, String... permissions) {
        String[] requestPermissions = checkSelfPermission(context, permissions);
        if (requestPermissions.length == 0) {
            if (listener != null)
                listener.onRequestPermission(true, null, null);
            return;
        }
        PermissionActivity.start(context, requestPermissions, null, false, listener);
    }

    public static void request(Context context, final OnSimplePermissionListener listener, String... permissions) {
        OnPermissionListener onPermissionListener = null;
        if (listener != null) {
            onPermissionListener = new OnPermissionListener() {
                @Override
                public void onRequestPermission(boolean success, @Nullable List<String> deniedPermissions, @Nullable List<String> rejectPermissions) {
                    listener.onRequestPermission(success);
                }
            };
        }
        request(context, onPermissionListener, permissions);
    }

    public static void requestForce(Context context, String permissionName, OnPermissionListener listener, String... permissions) {
        String[] requestPermissions = checkSelfPermission(context, permissions);
        if (requestPermissions.length == 0) {
            if (listener != null)
                listener.onRequestPermission(true, null, null);
            return;
        }
        PermissionActivity.start(context, requestPermissions, permissionName, true, listener);
    }

    public static void requestForce(Context context, String permissionName, final OnSimplePermissionListener listener, String... permissions) {
        OnPermissionListener onPermissionListener = null;
        if (listener != null) {
            onPermissionListener = new OnPermissionListener() {
                @Override
                public void onRequestPermission(boolean success, @Nullable List<String> deniedPermissions, @Nullable List<String> rejectPermissions) {
                    listener.onRequestPermission(success);
                }
            };
        }
        requestForce(context, permissionName, onPermissionListener, permissions);
    }

    public static boolean checkPermissions(Context context, String... permissions) {
        return checkSelfPermission(context, permissions).length == 0;
    }

    /**
     * ?????????????????????????????????
     *
     * @param permissions ??????????????????
     * @return ????????????????????????????????????????????????????????????.length==0, ??????permissions?????????????????????
     */
    public static String[] checkSelfPermission(Context context, String... permissions) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) return new String[]{};
        List<String> needRequest = new ArrayList<>();
        for (String permission : permissions) {
            if (!TextUtils.isEmpty(permission) && ContextCompat.checkSelfPermission(context, permission)
                    != PackageManager.PERMISSION_GRANTED) {
                needRequest.add(permission);
            }
        }
        return needRequest.toArray(new String[needRequest.size()]);//CommonUtil.toArray(rejectedPermissions);
    }

    // region ????????????
    public static void requestSpecialPermission(Context context, @NonNull String specialPermission, final OnSimplePermissionListener listener) {
        if (checkSpecialPermission(context, specialPermission)) {
            if (listener != null)
                listener.onRequestPermission(true);
            return;
        }

        OnPermissionListener onPermissionListener = null;
        if (listener != null) {
            onPermissionListener = new OnPermissionListener() {
                @Override
                public void onRequestPermission(boolean success, @Nullable List<String> deniedPermissions, @Nullable List<String> rejectPermissions) {
                    listener.onRequestPermission(success);
                }
            };
        }
        PermissionActivity.start(context, specialPermission, onPermissionListener);
    }

    /**
     * ?????????????????????????????????
     *
     * @param special ????????????????????????
     * @return true??????????????????false??????
     */
    public static boolean checkSpecialPermission(Context context, String special) {
        boolean isGranted = false;
        switch (special) {
            case SpecialPermission.DISPLAY_NOTIFICATION:
                isGranted = isNotificationEnabled(context);
                break;
            case SpecialPermission.INSTALL_UNKNOWN_APP:
                isGranted = checkSpecialInstallUnkownApp(context);
                break;
            case SpecialPermission.WRITE_SYSTEM_SETTINGS:
                isGranted = checkSpecialWriteSystemSettings(context);
                break;
            case SpecialPermission.SYSTEM_ALERT_WINDOW:
                isGranted = checkSpecialSystemAlertWindow(context);
                break;
            default:
                break;
        }
        return isGranted;
    }

    /**
     * ?????????????????? - ????????????????????????
     *
     * @return true???????????????????????????false????????????????????????
     */
    public static boolean checkSpecialInstallUnkownApp(Context context) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) return true;
        return context.getPackageManager().canRequestPackageInstalls();
    }

    /**
     * ?????????????????? - ??????????????????
     *
     * @return true???????????????????????????false????????????????????????
     */
    public static boolean checkSpecialWriteSystemSettings(Context context) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) return true;
        return Settings.System.canWrite(context);
    }

    /**
     * ?????????????????? - ???????????????
     *
     * @return true???????????????????????????false????????????????????????
     */
    public static boolean checkSpecialSystemAlertWindow(Context context) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) return true;
        return Settings.canDrawOverlays(context);
    }

    public static boolean gotoPermissionDetail(Context context) {
        try {
            new PermissionPageUtils(context).jumpPermissionPage();
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }
    // endregion

    public static void gotoAppDetail(Activity context, int requestCode) {
        Intent intent = new Intent(android.provider.Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
        intent.setData(Uri.parse("package:" + context.getPackageName()));
        context.startActivityForResult(intent, requestCode);
    }

    public interface SpecialPermission {
        String DISPLAY_NOTIFICATION = "DISPLAY_NOTIFICATION"; // ????????????
        String INSTALL_UNKNOWN_APP = "INSTALL_UNKNOWN_APP"; // ????????????????????????
        String SYSTEM_ALERT_WINDOW = "SYSTEM_ALERT_WINDOW"; // ???????????????
        String WRITE_SYSTEM_SETTINGS = "WRITE_SYSTEM_SETTINGS"; // ??????????????????
    }

    public interface OnPermissionListener {

        /**
         * ?????????????????????
         *
         * @param success           ??????????????????????????????deniedPermissions?????????/?????????????????????????????????????????????/??????????????????M
         * @param deniedPermissions ??????????????????????????????????????????rejectPermissions???
         * @param rejectPermissions ??????????????????????????????don???t ask again???????????????
         */
        void onRequestPermission(boolean success, @Nullable List<String> deniedPermissions, @Nullable List<String> rejectPermissions);
    }

    public interface OnSimplePermissionListener {
        /**
         * ?????????????????????
         *
         * @param success ??????????????????/?????????????????????????????????????????????/??????????????????M
         */
        void onRequestPermission(boolean success);
    }
}