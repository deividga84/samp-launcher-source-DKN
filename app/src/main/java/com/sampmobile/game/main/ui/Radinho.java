package com.sampmobile.game.main.ui;

import android.app.Activity;
import android.view.View;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.sampmobile.game.R;
import com.sampmobile.game.launcher.util.Util;

import java.nio.charset.StandardCharsets;

public class Radinho {
    private static ConstraintLayout radinhoLayout;
    private Activity activity;
    public static boolean radinhoVisible;
    private RadialMenu mRadialMenu;

    native void sendCommandV(byte[] str);

    public Radinho(Activity activity) {
        this.activity = activity;

        // Inflar o layout radial
        ConstraintLayout layout = (ConstraintLayout) activity.getLayoutInflater().inflate(R.layout.radinho, null);
        activity.addContentView(layout, new ConstraintLayout.LayoutParams(-1, -1));

        radinhoLayout = activity.findViewById(R.id.radinho);
        radinhoLayout.setVisibility(View.GONE);

        // Configurar listeners para os botões
        radinhoVisible = false;

        // Esconder o layout inicialmente
        Util.HideLayout(radinhoLayout, false);
    }

    public void show() {
        if(!radinhoVisible)
        {
            Util.ShowLayout(radinhoLayout, true);
            radinhoVisible = true;
        }
    }

    public static void RadioNaTela() {
        if(radinhoVisible)
        {
            Util.HideLayout(radinhoLayout, true);
            radinhoVisible = false;
        }
        else
        {
            Util.ShowLayout(radinhoLayout, true);
            radinhoVisible = true;
        }
    }

    public void hide() {
        if(radinhoVisible)
        {
            Util.HideLayout(radinhoLayout, true);
            radinhoVisible = false;
        }
    }
}