package com.sampmobile.game.launcher;

import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ProgressBar;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.google.android.material.progressindicator.LinearProgressIndicator;
import com.sampmobile.game.R;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class SplashActivity extends SampActivity {

    private final String[] permissions = {"android.permission.READ_EXTERNAL_STORAGE", "android.permission.WRITE_EXTERNAL_STORAGE", "android.permission.RECORD_AUDIO"};

    public int mGpuType;

    AlertDialog.Builder builder;

    private final ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            mService = new Messenger(iBinder);
            checkUpdate();
        }

        public void onServiceDisconnected(ComponentName componentName) {
            mService = null;
        }
    };


    public IncomingHandler mInHandler;
    public Messenger mMessenger;
    public Messenger mService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_splash);

        builder = new AlertDialog.Builder(this);

        ((ProgressBar)findViewById(R.id.progressBar)).setIndeterminate(true);

        mInHandler = new IncomingHandler();
        mMessenger = new Messenger(mInHandler);

        boolean theme = mPref.getBoolean("theme", false);
        changeTheme(theme);

        GLSurfaceView.Renderer mGlRenderer = new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
                UpdateActivity.eGPUType egputype;
                String glGetString = gl10.glGetString(GL10.GL_EXTENSIONS);
                String glGetString2 = gl10.glGetString(GL10.GL_EXTENSIONS);
                if (glGetString2.contains("GL_IMG_texture_compression_pvrtc")) {
                    egputype = UpdateActivity.eGPUType.PVR;
                    mGpuType = 3;
                } else if (glGetString2.contains("GL_EXT_texture_compression_dxt1") || glGetString2.contains("GL_EXT_texture_compression_s3tc") || glGetString2.contains("GL_AMD_compressed_ATC_texture")) {
                    egputype = UpdateActivity.eGPUType.DXT;
                    mGpuType = 1;
                } else {
                    egputype = UpdateActivity.eGPUType.ETC;
                    mGpuType = 2;
                }
                Log.e("x1y2z", "GPU name: " + glGetString);
                Log.e("x1y2z", "GPU type: " + egputype.name());

                if(isPermissionsGranted()) {
                    bindService(new Intent(SplashActivity.this, UpdateService.class), mConnection, Context.BIND_AUTO_CREATE);
                }
                else {
                    ActivityCompat.requestPermissions(SplashActivity.this,
                            permissions,
                            1);
                }
            }

            @Override
            public void onSurfaceChanged(GL10 gl10, int i, int i1) {

            }

            @Override
            public void onDrawFrame(GL10 gl10) {

            }
        };

        ConstraintLayout gpuLayout = findViewById(R.id.gpu);
        GLSurfaceView mGlSurfaceView = new GLSurfaceView(this);
        mGlSurfaceView.setRenderer(mGlRenderer);
        gpuLayout.addView(mGlSurfaceView);
    }

    public void changeTheme(boolean theme) {
        if(theme)  {
            findViewById(R.id.main_splash_layout).setBackgroundResource(R.drawable.bg_blue);
            ((LinearProgressIndicator)findViewById(R.id.progressBarBlue)).setVisibility(View.VISIBLE);
        }
        else {
            findViewById(R.id.main_splash_layout).setBackgroundResource(R.drawable.bg_red);
            ((LinearProgressIndicator)findViewById(R.id.progressBar)).setVisibility(View.VISIBLE);
        }
    }

    public class IncomingHandler extends Handler {
        public IncomingHandler() {}

        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (msg.what == 4) {
                UpdateActivity.UpdateStatus valueOf = UpdateActivity.UpdateStatus.valueOf(msg.getData().getString("status", ""));
                if (valueOf == UpdateActivity.UpdateStatus.Undefined) {
                    Message obtain = Message.obtain((Handler) null, 5);
                    obtain.replyTo = mMessenger;
                    try {
                        mService.send(obtain);
                    } catch (RemoteException e5) {
                        e5.printStackTrace();
                    }
                } else if (valueOf == UpdateActivity.UpdateStatus.CheckUpdate) {
                    Message obtain2 = Message.obtain((Handler) null, 4);
                    obtain2.replyTo = mMessenger;
                    try {
                        mService.send(obtain2);
                    } catch (RemoteException e6) {
                        e6.printStackTrace();
                    }
                }
            } else if (msg.what == 5) {
                UpdateActivity.GameStatus valueOf2 = UpdateActivity.GameStatus.valueOf(msg.getData().getString("status", ""));
                Log.i("x1y2z", "gameStatus = " + valueOf2);

                if (valueOf2 == UpdateActivity.GameStatus.UpdateRequired) {
                    builder.setMessage("Encontrei uma atualização da data!\nPretende atualizar? (se tiver uma compilação, clique em não)")
                            .setCancelable(false)
                            .setPositiveButton("Sim", new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    finish();
                                    Intent intent = new Intent(SplashActivity.this, UpdateActivity.class);
                                    intent.putExtra("mode", UpdateActivity.UpdateMode.GameDataUpdate.name());
                                    startActivity(intent);
                                    finish();
                                }
                            })
                            .setNegativeButton("Não", new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog, int id) {
                                    dialog.cancel();
                                    startActivity(new Intent(SplashActivity.this, MainActivity.class));
                                    finish();
                                }
                            });
                    AlertDialog alert = builder.create();
                    alert.setTitle("Update");
                    alert.show();
                }
                else if (valueOf2 == UpdateActivity.GameStatus.GameUpdateRequired) {
                    Intent intent = new Intent(SplashActivity.this, UpdateActivity.class);
                    intent.putExtra("mode", UpdateActivity.UpdateMode.GameDataUpdate.name());
                    startActivity(intent);
                    finish();
                }
                else {
                    startActivity(new Intent(SplashActivity.this, MainActivity.class));
                    finish();
                }

            }
        }
    }

    public void checkUpdate() {
        Log.d("x1y2z", "checkUpdate");
        Message obtain = Message.obtain((Handler) null, 0);
        obtain.getData().putInt("gputype", mGpuType);
        obtain.replyTo = mMessenger;
        try {
            mService.send(obtain);
        } catch (RemoteException e5) {
            e5.printStackTrace();
        }
    }

    public boolean isPermissionsGranted()
    {
        int size = permissions.length;

        for (int i = 0; i < size; i++) {
            if (ContextCompat.checkSelfPermission(this, permissions[i])
                    == PackageManager.PERMISSION_GRANTED) {
                return true;
            }
        }

        return false;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode != 1) {
            return;
        }
        if (grantResults.length <= 0 || grantResults[0] != 0) {
            Toast.makeText(this, "Permissions not granted!", Toast.LENGTH_LONG).show();
        } else {
            bindService(new Intent(this, UpdateService.class), mConnection, Context.BIND_AUTO_CREATE);
        }

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unbindService(mConnection);
    }
}
