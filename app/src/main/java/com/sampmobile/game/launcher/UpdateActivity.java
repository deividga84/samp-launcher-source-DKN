package com.sampmobile.game.launcher;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.Uri;
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
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.content.FileProvider;

//import com.sampmobile.game.BuildConfig;
import com.sampmobile.game.R;

import java.io.File;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class UpdateActivity extends SampActivity {
    public Messenger mMessenger = new Messenger(new IncomingHandler());
    public Messenger mService;
    boolean isBind = false;
    private UpdateMode mUpdateMode = UpdateMode.Undefined;
    public int mGpuType;

    private File mGameApk;

    boolean mIsStartingUpdate = false;

    public enum UpdateMode {
        Undefined,
        GameDataUpdate
    }

    public enum GameStatus {
        Unknown,
        GameUpdateRequired,
        UpdateRequired,
        Updated
    }

    public enum UpdateStatus {
        Undefined,
        CheckUpdate,
        CheckFiles,
        DownloadGame,
        DownloadGameData
    }

    public enum eGPUType {
        DXT,
        PVR,
        ETC
    }

    public void changeTheme(boolean theme) {
        if(theme)  {
            findViewById(R.id.main_layout).setBackgroundResource(R.drawable.bg_blue);
            ((ProgressBar)findViewById(R.id.download_progress)).setProgressDrawable(getResources().getDrawable(R.drawable.download_progress_blue));
            ((View)findViewById(R.id.view_blue)).setVisibility(View.VISIBLE);
        }
        else {
            findViewById(R.id.main_layout).setBackgroundResource(R.drawable.bg_red);
            ((ProgressBar)findViewById(R.id.download_progress)).setProgressDrawable(getResources().getDrawable(R.drawable.download_progress));
            ((View)findViewById(R.id.view_red)).setVisibility(View.VISIBLE);
        }
    }

    class IncomingHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            if (msg.what == 4) {
                UpdateActivity.UpdateStatus valueOf = UpdateActivity.UpdateStatus.valueOf(msg.getData().getString("status", ""));
                if (valueOf == UpdateStatus.DownloadGameData) {
                    ((TextView)findViewById(R.id.installation_text)).setText("Atualizando a data do jogo...");
                    Log.d("x1y2z", "statusname = " + valueOf);
                    long j = msg.getData().getLong("total");
                    long j2 = msg.getData().getLong("current");
                    ((TextView) findViewById(R.id.fileName)).setText(msg.getData().getString("filename"));
                    ((TextView) findViewById(R.id.fileCount)).setText(j2/1048576 + "MB/" + j/1048576+"MB");
                    ProgressBar progressBar = findViewById(R.id.download_progress);
                    progressBar.setIndeterminate(false);
                    Log.d("UpdateActivity", (int) (j/1048576) + "/" + (int) (j2/1048576));
                    progressBar.setMax((int) (j/1048576));
                    progressBar.setProgress((int) (j2/1048576));

                    ((TextView) findViewById(R.id.fileProgressPercent)).setText(j2*100/(j+1) + "%");
                } else if (valueOf == UpdateActivity.UpdateStatus.CheckUpdate) {
                    Log.d("x1y2z", "statusname = " + valueOf);
                    long j = msg.getData().getLong("total");
                    long j2 = msg.getData().getLong("current");
                    ((TextView) findViewById(R.id.fileName)).setText(msg.getData().getString("filename"));
                    ProgressBar progressBar = (ProgressBar) UpdateActivity.this.findViewById(R.id.download_progress);
                    progressBar.setMax((int) (j/1048576));
                    progressBar.setProgress((int) (j2/1048576));
                } else if (valueOf == UpdateStatus.DownloadGame) {
                    ((TextView)findViewById(R.id.installation_text)).setText("Atualizando o jogo...");
                    Log.d("x1y2z", "statusname = " + valueOf);
                    long j = msg.getData().getLong("total");
                    long j2 = msg.getData().getLong("current");
                    ((TextView) findViewById(R.id.fileName)).setText(msg.getData().getString("filename"));
                    ((TextView) findViewById(R.id.fileCount)).setText(msg.getData().getLong("currentfile") + "/" + msg.getData().getLong("totalfiles"));
                    ProgressBar progressBar = (ProgressBar) UpdateActivity.this.findViewById(R.id.download_progress);
                    progressBar.setIndeterminate(false);
                    progressBar.setMax((int) (j/1048576));
                    progressBar.setProgress((int) (j2/1048576));
                }else if (!mIsStartingUpdate) {
                    Message obtain2 = Message.obtain((Handler) null, 1);
                    obtain2.replyTo = UpdateActivity.this.mMessenger;
                    try {
                        UpdateActivity.this.mService.send(obtain2);
                    } catch (RemoteException e6) {
                        e6.printStackTrace();
                    }
                    mIsStartingUpdate = true;
                }
            }
            else if(msg.what == 2)
            {
                Intent intent;
                Log.d("x1y2z", "UpdateService.UPDATE_GAME_DATA");
                //resetProgress(true, 100, 100);
                if (msg.getData().getBoolean("status", false)) {
                    String string3 = msg.getData().getString("apkPath", "");
                    if (string3.length() > 0) {
                        mGameApk = new File(string3);
                    }
                    if (mGameApk == null || !mGameApk.exists()) {
                        intent = new Intent(UpdateActivity.this, SplashActivity.class);
                        startActivity(intent);
                        finish();
                    } else {
                        requestInstallGame();
                        return;
                    }
                }
                Log.d("x1y2z", "Error update game data");
            }
            else if (msg.what == 1) {
                Log.i("UpdateActivity", "UpdateService.UPDATE_GAME");
                ((TextView)findViewById(R.id.installation_text)).setText("Instalando...");
                ProgressBar progressBar = (ProgressBar) UpdateActivity.this.findViewById(R.id.download_progress);
                progressBar.setIndeterminate(true);
                String string = msg.getData().getString("apkPath", "");
                if (msg.getData().getBoolean("status", false)) {
                    if (string.length() > 0) {
                        mGameApk = new File(string);
                    }
                    Message obtain = Message.obtain((Handler) null, 2);
                    obtain.replyTo = UpdateActivity.this.mMessenger;
                    try {
                        mService.send(obtain);
                    } catch (RemoteException e5) {
                        e5.printStackTrace();
                    }
                } else {
                    Log.d("UpdateActivity", "Error update game");
                }
            }
        }
    }

    void requestInstallGame()
    {
        Log.d("x1y2z", "request install game");

        Uri contentUri1 = FileProvider.getUriForFile(getApplicationContext(), getApplicationContext().getPackageName() + ".provider", mGameApk);

        Intent intent = new Intent(Intent.ACTION_VIEW, contentUri1);
        intent.setDataAndType(contentUri1, "application/vnd.android.package-archive");
        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        startActivity(intent);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        Log.i("UpdateActivity", "onActivityResult -> code quest: " + resultCode + ", resultCode: " + data);
        if (requestCode == 0) {
            if (mGameApk != null && mGameApk.exists()) {
                mGameApk.delete();
            }
            startActivity(new Intent(this, SplashActivity.class));
            finish();
        }
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_update);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        boolean theme = mPref.getBoolean("theme", false);
        changeTheme(theme);

        GLSurfaceView.Renderer mGlRenderer = new GLSurfaceView.Renderer() {
            @Override
            public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
                eGPUType egputype;
                String glGetString = gl10.glGetString(GL10.GL_EXTENSIONS);
                String glGetString2 = gl10.glGetString(GL10.GL_EXTENSIONS);
                if (glGetString2.contains("GL_IMG_texture_compression_pvrtc")) {
                    egputype = eGPUType.PVR;
                    mGpuType = 3;
                } else if (glGetString2.contains("GL_EXT_texture_compression_dxt1") || glGetString2.contains("GL_EXT_texture_compression_s3tc") || glGetString2.contains("GL_AMD_compressed_ATC_texture")) {
                    egputype = eGPUType.DXT;
                    mGpuType = 1;
                } else {
                    egputype = eGPUType.ETC;
                    mGpuType = 2;
                }
                Log.e("x1y2z", "GPU name: " + glGetString);
                Log.e("x1y2z", "GPU type: " + egputype.name());

                UpdateMode valueOf = UpdateMode.valueOf(getIntent().getStringExtra("mode"));
                mUpdateMode = valueOf;

                if (mService == null) {
                    Intent intent = new Intent(UpdateActivity.this, UpdateService.class);
                    bindService(intent, serviceConnection, Context.BIND_AUTO_CREATE);
                } else if (valueOf == UpdateMode.GameDataUpdate) {
                    Message obtain = Message.obtain((Handler) null, 7);
                    obtain.getData().putInt("gputype", mGpuType);
                    obtain.replyTo = mMessenger;
                    try {
                        mService.send(obtain);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }

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

    private ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder iBinder) {
            mService = new Messenger(iBinder);

            if (mUpdateMode == UpdateMode.GameDataUpdate) {
                Message obtain = Message.obtain((Handler) null, 7);
                obtain.getData().putInt("gputype", mGpuType);
                obtain.replyTo = mMessenger;
                try {
                    mService.send(obtain);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }

            isBind = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mService = null;
            isBind = false;
        }
    };

    @Override
    public void onDestroy() {
        super.onDestroy();
        unbindService(serviceConnection);
    }
}
