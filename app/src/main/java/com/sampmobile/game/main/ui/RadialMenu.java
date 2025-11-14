package com.sampmobile.game.main.ui;

import android.app.Activity;
import android.view.View;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.sampmobile.game.R;
import com.sampmobile.game.launcher.util.Util;

import com.sampmobile.game.main.SAMP;

import java.nio.charset.StandardCharsets;

public class RadialMenu {
    private ConstraintLayout radialLayout;
    private Activity activity;
    public static boolean menuVisible;
    private RadialVehicles mRadialVehicles;
    //int RadinhoSom;

    native void sendCommand(byte[] str);

    public RadialMenu(Activity activity) {
        this.activity = activity;

        // Inflar o layout radial
        ConstraintLayout layout = (ConstraintLayout) activity.getLayoutInflater().inflate(R.layout.radial, null);
        activity.addContentView(layout, new ConstraintLayout.LayoutParams(-1, -1));

        radialLayout = activity.findViewById(R.id.radial);
        radialLayout.setVisibility(View.GONE);

        //Som de radinho
        //RadinhoSom = SAMP.soundPool.load(SAMP.getInstance(), R.raw.somradinho, 0);

        // Configurar listeners para os botões
        setListeners();
        menuVisible = false;

        // Esconder o layout inicialmente
        Util.HideLayout(radialLayout, false);
    }

    private void setListeners()
    {
        // Botão central para fechar o menu
        activity.findViewById(R.id.radial_close).setOnClickListener(view -> {
            hide();
        });

        // Configurar cliques nos botões radiais
        setRadialButtonClick(R.id.radial_button_00, 0);
        setRadialButtonClick(R.id.radial_button_01, 1);
        setRadialButtonClick(R.id.radial_button_02, 2);
        setRadialButtonClick(R.id.radial_button_03, 3); // Botão 3 configurado
        setRadialButtonClick(R.id.radial_button_04, 4);
        setRadialButtonClick(R.id.radial_button_05, 5);
        setRadialButtonClick(R.id.radial_button_06, 6);
        setRadialButtonClick(R.id.radial_button_07, 7);
        setRadialButtonClick(R.id.radial_button_08, 8);
        setRadialButtonClick(R.id.radial_button_09, 9);
    }

    private void setRadialButtonClick(int buttonId, int actionId)
    {
        activity.findViewById(buttonId).setOnClickListener(view -> {
            if (actionId == 0)//Celular
            {
                //sendCommand("/celular".getBytes(StandardCharsets.UTF_8));
                SAMP.getInstance().ShowCell();
                hide();
            }
            if (actionId == 1)//Gps
            {
                hide();
                sendCommand("/gps".getBytes(StandardCharsets.UTF_8));
            }
            if (actionId == 2)//Mochila
            {
                hide();
                sendCommand("/inventario".getBytes(StandardCharsets.UTF_8));
            }
            if (actionId == 3)//Tab
            {
                hide();
                Radinho.RadioNaTela();
            }
            if (actionId == 4)//Propriedades
            {

            }
            if (actionId == 5)//Missoes
            {
            }
            if (actionId == 6)//Loja Vip
            {
                sendCommand("/menuvip".getBytes(StandardCharsets.UTF_8));
                hide();
            }
            if (actionId == 7)//Atendimento
            {

            }
            if (actionId == 8)//Animacoes
            {
                hide();
                sendCommand("/anims".getBytes(StandardCharsets.UTF_8));
            }
            if (actionId == 9)//Registro RG
            {
                hide();
                sendCommand("/rg".getBytes(StandardCharsets.UTF_8));
            }
        });
    }

    public void show() {
        if(!menuVisible && !RadialVehicles.menuVisibleV)
        {
            Util.ShowLayout(radialLayout, true);
            menuVisible = true;
        }
    }

    public void hide() {
        if(menuVisible)
        {
            Util.HideLayout(radialLayout, true);
            menuVisible = false;
        }
    }
}
