package com.sampmobile.game.launcher;


import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import com.sampmobile.game.R;

import java.util.Map;


public class SampActivity extends AppCompatActivity {


    public SharedPreferences mPref;
    Toast mToast;

    boolean isDark;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mPref = PreferenceManager.getDefaultSharedPreferences(this);
    }






    public static void putSettingSwitchToPref(SharedPreferences pref, String tag, boolean state) {
        SharedPreferences.Editor editor = pref.edit();
        editor.putBoolean(tag, state);
        editor.apply();
    }
    boolean isNetworkConnected(){

        ConnectivityManager connMng = (ConnectivityManager)
                getSystemService(Context.CONNECTIVITY_SERVICE);
        if (connMng == null) return false;
        NetworkInfo networkInfo = connMng.getActiveNetworkInfo();

        return networkInfo != null && networkInfo.isConnected();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }
}