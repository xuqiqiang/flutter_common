package com.ssnwt.requester.proxy;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.DocumentsContract;
import android.util.Log;
import androidx.documentfile.provider.DocumentFile;
import com.ssnwt.requester.PermissionRequester;
import com.ssnwt.requester.PickerRequester;
import com.ssnwt.requester.utils.FileUtils;

public class PickerActivity extends Activity {
    public static final int REQUEST_PICK_SHARE_PATH = 0x002001;
    private static final String PARAM_TYPE = "PARAM_TYPE";
    private static final String PARAM_REQUEST = "PARAM_REQUEST";
    private static PickerRequester.OnPickUriListener mOnPickUriListener;
    private static PickerRequester.OnPickPathListener mOnPickPathListener;

    //public static void start(final Context context, final String type, final PickerRequester.OnPickUriListener listener) {
    //    PermissionRequester.requestForce(context, "存储", new PermissionRequester.OnSimplePermissionListener() {
    //        @Override
    //        public void onRequestPermission(boolean success) {
    //            if (success) {
    //                mOnPickUriListener = listener;
    //                Intent intent = new Intent(context, PickerActivity.class);
    //                intent.putExtra(PARAM_TYPE, type);
    //                if (!(context instanceof Activity))
    //                    intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
    //                context.startActivity(intent);
    //            }
    //        }
    //    }, Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE);
    //}

    //public static void start(final Context context, final String type, final PickerRequester.OnPickPathListener listener) {
    //    PermissionRequester.requestForce(context, "存储", new PermissionRequester.OnSimplePermissionListener() {
    //        @Override
    //        public void onRequestPermission(boolean success) {
    //            if (success) {
    //                mOnPickPathListener = listener;
    //                Intent intent = new Intent(context, PickerActivity.class);
    //                intent.putExtra(PARAM_TYPE, type);
    //                if (!(context instanceof Activity))
    //                    intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
    //                context.startActivity(intent);
    //            }
    //        }
    //    }, Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE);
    //
    //}

    public static void start(final Context context, final int request, final PickerRequester.OnPickPathListener listener) {
        PermissionRequester.requestForce(context, "存储", new PermissionRequester.OnSimplePermissionListener() {
            @Override
            public void onRequestPermission(boolean success) {
                if (success) {
                    mOnPickPathListener = listener;
                    Intent intent = new Intent(context, PickerActivity.class);
                    intent.putExtra(PARAM_REQUEST, request);
                    if (!(context instanceof Activity))
                        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    context.startActivity(intent);
                }
            }
        }, Manifest.permission.WRITE_EXTERNAL_STORAGE, Manifest.permission.READ_EXTERNAL_STORAGE);

    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        pickFile(getIntent().getIntExtra(PARAM_REQUEST, REQUEST_PICK_SHARE_PATH), getIntent().getStringExtra(PARAM_TYPE));
    }

    private void pickFile(int request, String type) {
        if(request ==REQUEST_PICK_SHARE_PATH) {
            Uri uri = Uri.parse(
                "content://com.android.externalstorage.documents/tree/primary%3AAndroid%2Fdata");
            DocumentFile documentFile = DocumentFile.fromTreeUri(this, uri);
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
            intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION
                | Intent.FLAG_GRANT_WRITE_URI_PERMISSION
                | Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION
                | Intent.FLAG_GRANT_PREFIX_URI_PERMISSION);
            assert documentFile != null;
            intent.putExtra(DocumentsContract.EXTRA_INITIAL_URI, documentFile.getUri());
            startActivityForResult(intent, REQUEST_PICK_SHARE_PATH);
        }
        //if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT) {
        //    startActivityForResult(new Intent(Intent.ACTION_GET_CONTENT).setType(type),
        //            REQUEST_PICK_IMAGE);
        //} else {
        //    Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        //    intent.addCategory(Intent.CATEGORY_OPENABLE);
        //    intent.setType(type);
        //    startActivityForResult(intent, REQUEST_SAF_PICK_IMAGE);
        //}
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_PICK_SHARE_PATH) {

            //String dirPath;
            Uri result = null;
            if (resultCode == Activity.RESULT_OK && data != null) {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    Uri uri;
                    if ((uri = data.getData()) != null) {
                        getContentResolver().takePersistableUriPermission(uri, data.getFlags() & (
                            Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION));

                        result = DocumentsContract.buildDocumentUriUsingTree(uri, DocumentsContract.getTreeDocumentId(uri));

                        //dirPath = FileUtils.getFullPathFromTreeUri(uri, this);
                    }
                }
            }
            onPick(result);
            finish();
        }

    }

    //@Override
    //public void onActivityResult(int requestCode, int resultCode, Intent data) {
    //    if (requestCode == REQUEST_PICK_IMAGE || requestCode == REQUEST_SAF_PICK_IMAGE) {
    //        if (resultCode == Activity.RESULT_OK && data != null) {
    //            if (requestCode == REQUEST_PICK_IMAGE) onPick(data.getData());
    //            else if (requestCode == REQUEST_SAF_PICK_IMAGE)
    //                onPick(PickerRequester.ensureUriPermission(this, data));
    //        } else {
    //            onPick(null);
    //        }
    //        finish();
    //    }
    //    super.onActivityResult(requestCode, resultCode, data);
    //}

    private void onPick(Uri uri) {
        if (mOnPickUriListener != null) mOnPickUriListener.onPick(uri);
        else if (mOnPickPathListener != null) {
            String path = null;
            if (uri != null) {
                try {
                    path = FileUtils.getFullPathFromTreeUri(uri, this);//UriUtils.getPath(this, uri);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            mOnPickPathListener.onPick(path);
        }
    }

    @Override
    protected void onDestroy() {
        mOnPickUriListener = null;
        mOnPickPathListener = null;
        super.onDestroy();
    }
}
