package com.sampmobile.game.main.ui;

import android.app.Activity;
import android.view.View;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.sampmobile.game.R;
import com.sampmobile.game.launcher.util.Util;

import java.nio.charset.StandardCharsets;

public class RadialVehicles {
    private ConstraintLayout radialLayout;
    private Activity activity;
    public static boolean menuVisibleV;
    private RadialMenu mRadialMenu;

    native void sendCommandV(byte[] str);

    public RadialVehicles(Activity activity) {
        this.activity = activity;

        // Inflar o layout radial
        ConstraintLayout layout = (ConstraintLayout) activity.getLayoutInflater().inflate(R.layout.radialvehicles, null);
        activity.addContentView(layout, new ConstraintLayout.LayoutParams(-1, -1));

        radialLayout = activity.findViewById(R.id.radialvehicles);
        radialLayout.setVisibility(View.GONE);

        // Configurar listeners para os botões
        setListeners();
        menuVisibleV = false;

        // Esconder o layout inicialmente
        Util.HideLayout(radialLayout, false);
    }

    private void setListeners()
    {
        // Botão central para fechar o menu
        activity.findViewById(R.id.radialv_closev).setOnClickListener(view -> {
            hide();
        });

        // Configurar cliques nos botões radiais
        setRadialButtonClick(R.id.radialv_button_00, 0);
        //setRadialButtonClick(R.id.radialv_button_03, 3);
        //setRadialButtonClick(R.id.radialv_button_04, 4);
        //setRadialButtonClick(R.id.radialv_button_05, 5);
        //setRadialButtonClick(R.id.radialv_button_06, 6);
        setRadialButtonClick(R.id.radialv_button_07, 7);
        setRadialButtonClick(R.id.radialv_button_08, 8);
        setRadialButtonClick(R.id.radialv_button_09, 9);
    }

    private void setRadialButtonClick(int buttonId, int actionId)
    {
        activity.findViewById(buttonId).setOnClickListener(view -> {
            if (actionId == 0)//Motor
            {
                sendCommandV("/motor".getBytes(StandardCharsets.UTF_8));
                hide();
            }
            if (actionId == 1)//Trancar
            {
                sendCommandV("/trancar".getBytes(StandardCharsets.UTF_8));
                hide();
            }
            /*if (actionId == 3)
            {

            }
            if (actionId == 4)
            {

            }
            if (actionId == 5)
            {

            }
            if (actionId == 6)
            {
            }*/
            if (actionId == 7)//Farol
            {
                sendCommandV("/farol".getBytes(StandardCharsets.UTF_8));
                hide();
            }
            if (actionId == 8)//Carros
            {
                hide();
                sendCommandV("/car".getBytes(StandardCharsets.UTF_8));
            }
            if (actionId == 9)//Porta Malas
            {
                sendCommandV("/portamalas".getBytes(StandardCharsets.UTF_8));
                hide();
            }
        });
    }

    public void show() {
        if(!menuVisibleV && !RadialMenu.menuVisible)
        {
            Util.ShowLayout(radialLayout, true);
            menuVisibleV = true;
        }
    }

    public void hide() {
        if(menuVisibleV)
        {
            Util.HideLayout(radialLayout, true);
            menuVisibleV = false;
        }
    }
}