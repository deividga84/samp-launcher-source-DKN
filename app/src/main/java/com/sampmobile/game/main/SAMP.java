package com.sampmobile.game.main;

import android.os.Bundle;
import android.util.Log;
//import android.media.SoundPool;
import android.view.KeyEvent;
import android.view.View;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.google.firebase.crashlytics.FirebaseCrashlytics;
import com.sampmobile.game.R;
import com.sampmobile.game.main.ui.AttachEdit;
import com.sampmobile.game.main.ui.CustomKeyboard;
import com.sampmobile.game.main.ui.dialog.DialogManager;
import com.sampmobile.game.main.ui.RadialMenu;
import com.sampmobile.game.main.ui.RadialVehicles;
import com.sampmobile.game.main.ui.Radinho;

import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;
import java.util.Locale;

//API
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;
import org.json.JSONException;
import org.json.JSONObject;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import android.widget.EditText;
import android.widget.Button;
import android.widget.Toast;
import android.os.AsyncTask;
import java.net.URL;
import java.net.HttpURLConnection;
import java.io.OutputStream;
import java.util.Scanner;

//WEBVIEW
import android.webkit.WebView;
import android.webkit.WebSettings;
import android.webkit.WebViewClient;
import android.graphics.Color;
import android.webkit.JavascriptInterface;

public class SAMP extends GTASA implements CustomKeyboard.InputListener, HeightProvider.HeightListener {
    private static final String TAG = "SAMP";
    private static SAMP instance;

    private CustomKeyboard mKeyboard;
    private DialogManager mDialog;
    private HeightProvider mHeightProvider;

    //webviewcell
    private WebView webcell;
    private FrameLayout framewebcell;

    //public static SoundPool soundPool = null;
    private AttachEdit mAttachEdit;
    private RadialMenu mRadialMenu;
    private RadialVehicles mRadialVehicles;
    private Radinho mRadinho;

    ConstraintLayout hud_main;
    ConstraintLayout loadingscreen;

    private int iShowHud;
    private boolean iShowLogo;
    private boolean TeclasAbertas;

    //API
    private View connectScreenView;
    private View LoginScreenView;
    private View RegisterScreenView;
    private Handler handler;
    private Runnable apiCheckerRunnable;

    //WEBVIEW
    private View WebViewScreenView;

    long buttonLockCD;

    static String vmVersion;

    static {
        vmVersion = null;
        Log.i(TAG, "**** Loading SO's");

        try {
            vmVersion = System.getProperty("java.vm.version");
            Log.i(TAG, "vmVersion " + vmVersion);

            System.loadLibrary("bass");
            System.loadLibrary("SAMP");
        }
        catch (ExceptionInInitializerError | UnsatisfiedLinkError e) {
            Log.e(TAG, e.getMessage());
        }
    }

    public native void sendDialogResponse(int i, int i2, int i3, byte[] str);

    public static SAMP getInstance() {
        return instance;
    }

    private void showTab()
    {

    }

    private void hideTab()
    {

    }

    private void setTab(int id, String name, int score, int ping)
    {

    }

    private void clearTab()
    {

    }

    private void showLoadingScreen()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                loadingscreen.setVisibility(View.VISIBLE);
            }
        });
    }

    private void hideLoadingScreen()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                loadingscreen.setVisibility(View.GONE);
                //exibirTelaDeConexaoEVerificarAPI();
                MostrarChat();
            }
        });
    }

    private void setPauseState(boolean pause)
    {
        if(pause)
        {
            hideSystemUI();
        }
        else
        {
            showSystemUI();
        }
    }

    public void exitGame(){
        FirebaseCrashlytics.getInstance().setCrashlyticsCollectionEnabled(false);

        finishAndRemoveTask();
        System.exit(0);
    }

    public void showDialog(int dialogId, int dialogTypeId, byte[] bArr, byte[] bArr2, byte[] bArr3, byte[] bArr4) {
        final String caption = new String(bArr, StandardCharsets.UTF_8);
        final String content = new String(bArr2, StandardCharsets.UTF_8);
        final String leftBtnText = new String(bArr3, StandardCharsets.UTF_8);
        final String rightBtnText = new String(bArr4, StandardCharsets.UTF_8);
        runOnUiThread(() -> { this.mDialog.show(dialogId, dialogTypeId, caption, content, leftBtnText, rightBtnText); });
    }

    public void hideWithoutReset()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                hud_main.setVisibility(View.GONE);
                mDialog.hideWithoutReset();
                mAttachEdit.hideWithoutReset();
            }
        });
    }

    public void showWithoutReset()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(iShowHud == 1)
                    hud_main.setVisibility(View.VISIBLE);
                if(mAttachEdit.isShow)
                    mAttachEdit.showWithoutReset();
                if(mDialog.isShow)
                    mDialog.showWithOldContent();
            }
        });
    }

    private void showEditObject()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAttachEdit.show();
            }
        });
    }

    private void hideEditObject()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAttachEdit.hide();
            }
        });
    }

    private native void onInputEnd(byte[] str);
    @Override
    public void OnInputEnd(String str)
    {
        byte[] toReturn = null;
        try
        {
            toReturn = str.getBytes("windows-1251");
        }
        catch(UnsupportedEncodingException e)
        {

        }

        try {
            onInputEnd(toReturn);
        }
        catch (UnsatisfiedLinkError e5) {
            Log.e(TAG, e5.getMessage());
        }
    }

    private void showKeyboard()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.d("AXL", "showKeyboard()");
                mKeyboard.ShowInputLayout();
            }
        });
    }

    private void hideKeyboard()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mKeyboard.HideInputLayout();
            }
        });
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "**** onCreate");
        super.onCreate(savedInstanceState);
        
        //API
        handler = new Handler();

        mHeightProvider = new HeightProvider(this);

        mDialog = new DialogManager(this);

        mAttachEdit = new AttachEdit(this);

        mRadialMenu = new RadialMenu(this);

        mRadialVehicles = new RadialVehicles(this);

        mRadinho = new Radinho(this);

        hud_main = (ConstraintLayout) getLayoutInflater().inflate(R.layout.hud, null);
        addContentView(hud_main, new ConstraintLayout.LayoutParams(-1, -1));
        hud_main.setVisibility(View.GONE);

        loadingscreen = (ConstraintLayout) getLayoutInflater().inflate(R.layout.loading_screen, null);
        addContentView(loadingscreen, new ConstraintLayout.LayoutParams(-1, -1));
        loadingscreen.setVisibility(View.GONE);

        mKeyboard = new CustomKeyboard(this);

        /*AudioAttributes attributes = new AudioAttributes.Builder()
                .setUsage(AudioAttributes.USAGE_GAME)
                .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                .build();
        soundPool = new SoundPool.Builder().setAudioAttributes(attributes).build();*/

        instance = this;

        //WEBVIEW
        // Pré-inicializa o processo do WebView Chromium
        new WebView(getApplicationContext()).loadUrl("about:blank");
        
        // Inicialização leve do WebView no início
        /*framewebcell = findViewById(R.id.framewebcell);
        webcell = findViewById(R.id.webcell);
        webcell.post(() -> {
            webcell.setBackgroundColor(Color.TRANSPARENT);
            webcell.getSettings().setJavaScriptEnabled(true);
            webcell.getSettings().setDomStorageEnabled(true);
            webcell.getSettings().setCacheMode(WebSettings.LOAD_DEFAULT);

            // Desativa o zoom direto sem criar variável:
            webcell.getSettings().setSupportZoom(false);
            webcell.getSettings().setBuiltInZoomControls(false);
            webcell.getSettings().setDisplayZoomControls(false);

            // Bloqueia pinch-to-zoom de verdade
            webcell.getSettings().setUseWideViewPort(true);
            webcell.getSettings().setLoadWithOverviewMode(true);
            webcell.setInitialScale(100);

            webcell.setWebViewClient(new WebViewClient());
            webcell.loadUrl("file:///android_asset/interfaces/celular/index.html");  // Carregamento REAL antecipado
        
            webcellcarregada = true; // já marca como carregada

            webcell.setOnTouchListener((v, event) -> {
                if (event.getPointerCount() > 1) {
                    return true; // Bloqueia multi-toque (zoom com 2 dedos)
                }
                return false;
            });
        });
        webcell.setLayerType(View.LAYER_TYPE_HARDWARE, null);
        framewebcell.setVisibility(View.GONE);
        webcell.setVisibility(View.GONE);*/

        try {
            initializeSAMP();
        } catch (UnsatisfiedLinkError e5) {
            Log.e(TAG, e5.getMessage());
        }

    }

    private native void initializeSAMP();
    public native void togglePlayer(int toggle);



    @Override
    public void onStart() {
        Log.i(TAG, "**** onStart");
        super.onStart();
    }

    @Override
    public void onRestart() {
        Log.i(TAG, "**** onRestart");
        super.onRestart();
    }

    @Override
    public void onResume() {
        Log.i(TAG, "**** onResume");
        super.onResume();
        mHeightProvider.init(view);
    }

    public native void onEventBackPressed();
    native void onClickButton(int action);
    //onClickButton(2);
    @Override
    public void onBackPressed() {
        super.onBackPressed();
        onEventBackPressed();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if(keyCode == KeyEvent.KEYCODE_BACK)
        {
            onEventBackPressed();
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public void onPause() {
        Log.i(TAG, "**** onPause");
        super.onPause();
    }

    @Override
    public void onStop() {
        Log.i(TAG, "**** onStop");
        super.onStop();
    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "**** onDestroy");
        super.onDestroy();
    }

    @Override
    public void onHeightChanged(int orientation, int height) {
        mKeyboard.onHeightChanged(height);
        mDialog.onHeightChanged(height);
    }

    public ConstraintLayout logo;

    public ConstraintLayout hud1;
    private ProgressBar hpBar;
    private ProgressBar armourBar;
    private ProgressBar eatBar;
    private ProgressBar sedeBar;
    private ProgressBar sonoBar;
    private TextView moneyText;
    //private TextView hpText;
    //private TextView armourText;
    //private TextView eatText;
    private ImageView gunImg;
    private TextView ammoText;
    private ImageView enter_passengerB;
    private ImageView lock_vehicle;

    public ConstraintLayout hud_alt_v;
    public ConstraintLayout hud_f_v;

    public void ShowLogo(boolean show)
    {
        iShowLogo = show;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
            }
        });
    }

    public void togglePassengerButton(boolean toggle)
    {
        if(toggle)
        {
            runOnUiThread(new Runnable() {
                @Override
                public void run()
                {
                    if(hud_main == null) return;
                    enter_passengerB = hud_main.findViewById(R.id.enter_passenger);
                    enter_passengerB.setVisibility(View.VISIBLE);
                }
            });
        }
        else
        {
            runOnUiThread(new Runnable() {
                @Override
                public void run()
                {
                    if(hud_main == null) return;
                    enter_passengerB = hud_main.findViewById(R.id.enter_passenger);
                    enter_passengerB.setVisibility(View.INVISIBLE);
                }
            });
        }
    }
    void toggleLockButton(boolean toggle)
    {
        if(toggle)
        {
            runOnUiThread(new Runnable() {
                @Override
                public void run()
                {
                    if(hud_main == null) return;
                    lock_vehicle = hud_main.findViewById(R.id.vehicle_lock_butt);
                    lock_vehicle.setVisibility(View.VISIBLE);
                }
            });
        }
        else
        {
            runOnUiThread(new Runnable() {
                @Override
                public void run()
                {
                    if(hud_main == null) return;
                    lock_vehicle = hud_main.findViewById(R.id.vehicle_lock_butt);
                    lock_vehicle.setVisibility(View.INVISIBLE);
                }
            });
        }
    }

    void MostrarTeclas() {
        if (!TeclasAbertas) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (hud_main == null) return;
                    hud_alt_v = hud_main.findViewById(R.id.hud_alt);
                    hud_f_v = hud_main.findViewById(R.id.hud_f);
                    hud_alt_v.setVisibility(View.VISIBLE);
                    hud_f_v.setVisibility(View.VISIBLE);
                    TeclasAbertas = true;
                }
            });
        }
    }
    void EsconderTeclas()
    {
        if (TeclasAbertas)
        {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (hud_main == null) return;
                    hud_alt_v = hud_main.findViewById(R.id.hud_alt);
                    hud_f_v = hud_main.findViewById(R.id.hud_f);
                    hud_alt_v.setVisibility(View.INVISIBLE);
                    hud_f_v.setVisibility(View.INVISIBLE);
                    TeclasAbertas = false;
                }
            });
        }
    }
    
    native void ClickLockVehicleButton();
    public native void ClickEnterPassengerButton();
    public native void changeGun();
    native void MostrarChat();
    public void showhud()
    {
        iShowHud = 1;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(hud_main == null) return;
                hud_main.setVisibility(View.VISIBLE);
                hud1 = hud_main.findViewById(R.id.hud);

                hud1.setVisibility(View.VISIBLE);
                hud1.setAlpha(1.0f);

                //TESTEWEBVIEW
                //ShowCell();

                // Botão para sentar como passageiro
                enter_passengerB = hud_main.findViewById(R.id.enter_passenger);
                enter_passengerB.setVisibility(View.INVISIBLE);

                // Botão para trancar e destrancar
                lock_vehicle = hud_main.findViewById(R.id.vehicle_lock_butt);
                lock_vehicle.setVisibility(View.INVISIBLE);

                hud_alt_v = hud_main.findViewById(R.id.hud_alt);
                hud_f_v = hud_main.findViewById(R.id.hud_f);
                hud_alt_v.setVisibility(View.INVISIBLE);
                hud_f_v.setVisibility(View.INVISIBLE);
                TeclasAbertas = false;

                hud_main.findViewById(R.id.WeaponShowLayout).setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        Log.i("LogCat","Called change GUN");
                        changeGun();
                    }
                });
                hud_main.findViewById(R.id.btn_2).setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        Log.i("LogCat","Called change RadialVehicles");
                        mRadialVehicles.show();
                    }
                });
                hud_main.findViewById(R.id.btn_1).setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        Log.i("LogCat","Called change RadialMenu");
                        mRadialMenu.show();
                    }
                });
                hud_main.findViewById(R.id.btn_0).setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        Log.i("LogCat","Called change keys");
                        if (TeclasAbertas)
                        {
                            EsconderTeclas();
                        } else if (!TeclasAbertas)
                        {
                            MostrarTeclas();
                        }
                    }
                });
                hud_main.findViewById(R.id.enter_passenger).setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        Log.i("LogCat","Called change Passager");
                        ClickEnterPassengerButton();
                    }
                });
                hud_main.findViewById(R.id.vehicle_lock_butt).setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        Log.i("LogCat","Called change Trancar");
                        long currTime = System.currentTimeMillis()/1000;
                        if(buttonLockCD > currTime)
                        {
                            return;
                        }
                        buttonLockCD = currTime+2;
                        ClickLockVehicleButton();
                    }
                });
                hud_main.findViewById(R.id.hud_alt).setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        Log.i("LogCat","Called change alt");
                        onClickButton(1);
                        WebView WebView = findViewById(R.id.webcell);
                        HideWebView(WebView);
                        //EsconderTeclas();
                    }
                });
                hud_main.findViewById(R.id.hud_f).setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        Log.i("LogCat","Called change f");
                        onClickButton(2);
                        //EsconderTeclas();
                    }
                });
                hud_main.findViewById(R.id.hide_chat).setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        Log.i("LogCat","Called change ocultar chat");
                        MostrarChat();
                    }
                });
            }
        });
    }

    int DialogId = 0;

    public void hidehud()
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(hud_main == null) return;
                iShowHud = 0;
                hud_main.setVisibility(View.GONE);
                hud1 = hud_main.findViewById(R.id.hud);


                hud1.setVisibility(View.GONE);
                hud1.setAlpha(0.0f);

            }
        });
    }

    public void UpdateHud(int hp, int armour, int eat, int money,int gunId,int ammo)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(hud_main == null) return;

                //hpBar = hud_main.findViewById(R.id.progressBarHeart);
                //armourBar = hud_main.findViewById(R.id.progressBarArmour);
                //eatBar = hud_main.findViewById(R.id.progressBarEat);
                moneyText = hud_main.findViewById(R.id.money);

                //sedeBar = hud_main.findViewById(R.id.progressBarsede);
                //sonoBar = hud_main.findViewById(R.id.progressBarsono);

                //hpText = hud_main.findViewById(R.id.hpText);
                //armourText= hud_main.findViewById(R.id.Armourtext);
                //eatText = hud_main.findViewById(R.id.eatText);

                //hpBar.setProgress(hp);
                //armourBar.setProgress(100);
                //eatBar.setProgress(50);
                //sedeBar.setProgress(50);
                //sonoBar.setProgress(50);

                String moneyStr = String.format(Locale.ITALY, "R$%,d", money);

                moneyText.setText(moneyStr);

                //hpText.setText(Integer.toString(hp) + "%");
                //armourText.setText(Integer.toString(armour) + "%");
                //eatText.setText(Integer.toString(eat)+"%");

                gunImg = hud_main.findViewById(R.id.gunImg);
                ammoText = hud_main.findViewById(R.id.ammo);

                //Fist Update


                if(gunId == 0)
                {
                    hud_main.findViewById(R.id.WeaponShowLayout).setVisibility(View.VISIBLE);
                    hud_main.findViewById(R.id.Fist).setVisibility(View.VISIBLE);
                }
                else
                {
                    hud_main.findViewById(R.id.WeaponShowLayout).setVisibility(View.VISIBLE);
                    hud_main.findViewById(R.id.Fist).setVisibility(View.GONE);
                }


                if(gunId == 5) gunImg.setImageResource(R.drawable.bat);
                if(gunId == 22) gunImg.setImageResource(R.drawable.pistol);
                if(gunId == 23) gunImg.setImageResource(R.drawable.taser);
                if(gunId == 24) gunImg.setImageResource(R.drawable.deagle);
                if(gunId == 25) gunImg.setImageResource(R.drawable.shotgun);
                if(gunId == 29) gunImg.setImageResource(R.drawable.mppyat);
                if(gunId == 30) gunImg.setImageResource(R.drawable.ak);
                if(gunId == 31) gunImg.setImageResource(R.drawable.mka);
                if(gunId == 38) gunImg.setImageResource(R.drawable.minigun);
                ammoText.setText(Integer.toString(ammo));

            }
        });
    }
    //API
    private String lerNomeDoPlayer() {
        try {
            File file = new File(getExternalFilesDir(null) + "/SAMP/settings.ini");
            if (file.exists()) {
                BufferedReader reader = new BufferedReader(new FileReader(file));
                String line;
                while ((line = reader.readLine()) != null) {
                    if (line.trim().startsWith("name")) {
                        String[] parts = line.split("=");
                        if (parts.length == 2) {
                            return parts[1].trim();
                        }
                    }
                }
                reader.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "Jogador"; // fallback se der erro
    }
    //WEBVIEW
    // Flag para verificar se a página já foi carregada
    private boolean webcellcarregada = false;
    public void ShowCell() {
        runOnUiThread(() -> {
            framewebcell.setVisibility(View.VISIBLE);
            webcell.setVisibility(View.VISIBLE);
        });
    }
    private void HideWebView(WebView WebView) {
        if (WebView.getVisibility() == View.VISIBLE) {
            WebView.setVisibility(View.GONE);
            Log.i("WebView", "WebView ocultada com sucesso.");
        } else {
            Log.i("WebView", "WebView já estava oculta.");
        }
    }
    /*@JavascriptInterface
    public void fecharWebAcessJS() {
        runOnUiThread(() -> {
            framewebacess.setVisibility(View.GONE);
            webacess.setVisibility(View.GONE);
        });
    }*/
}
