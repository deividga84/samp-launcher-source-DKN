package com.sampmobile.game.launcher.fragments;

import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.appcompat.widget.SwitchCompat;
import androidx.fragment.app.Fragment;

import com.sampmobile.game.R;
import com.sampmobile.game.launcher.MainActivity;
import com.sampmobile.game.launcher.SplashActivity;
import com.sampmobile.game.launcher.util.ButtonAnimator;
import com.sampmobile.game.launcher.util.SharedPreferenceCore;
import com.sampmobile.game.launcher.util.Util;

import org.ini4j.Wini;

import java.io.File;
import java.io.IOException;

public class SettingsFragment extends Fragment {

    Wini mWini = null;

    SwitchCompat mKeyboardSwitch;

    @Override
    public View onCreateView(LayoutInflater layoutInflater, ViewGroup viewGroup, Bundle bundle) {
        View view = layoutInflater.inflate(R.layout.fragment_settings, viewGroup, false);

        EditText mNickName = view.findViewById(R.id.settings_nickname);

        mKeyboardSwitch = view.findViewById(R.id.keyboard_switch);

        File file = new File(getActivity().getExternalFilesDir(null) + "/SAMP/settings.ini");
        try {
            mWini = new Wini(file);
        } catch (IOException e) {
            e.printStackTrace();
        }
        if(file.exists()) {

            String[] sort = {"5", "10", "15", "20"};
            Spinner spinner = view.findViewById(R.id.chat_count_select);

            ArrayAdapter<String> adapter = new ArrayAdapter(getActivity(), R.layout.spinner_item, sort);
            adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            spinner.setAdapter(adapter);

            spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
                @Override
                public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                    int i1 = 5;
                    switch (i) {
                        case 0:
                            i1 = 5;
                            break;
                        case 1:
                            i1 = 10;
                            break;
                        case 2:
                            i1 = 15;
                            break;
                        case 3:
                            i1 = 20;
                            break;
                    }
                    Log.d("x1y2z", " " + i1);
                    try {
                        mWini.put("gui", "ChatMaxMessages", i1);
                        mWini.store();
                    } catch (IOException e) {
                        throw new RuntimeException(e);
                    }
                }

                @Override
                public void onNothingSelected(AdapterView<?> adapterView) {

                }
            });

            String message = mWini.get("gui", "ChatMaxMessages");
            switch (message)
            {
                case "5": spinner.setSelection(0); break;
                case "10": spinner.setSelection(1); break;
                case "15": spinner.setSelection(2); break;
                case "20": spinner.setSelection(3); break;
            }

            try {
                mWini.store();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }

        String[] sort1 = {"Red", "Blue"};
        Spinner spinner1 = view.findViewById(R.id.theme_select);

        ArrayAdapter<String> adapter = new ArrayAdapter(getActivity(), R.layout.spinner_item, sort1);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner1.setAdapter(adapter);

        spinner1.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                SharedPreferences.Editor editor = ((MainActivity)getActivity()).mPref.edit();
                boolean b = (i != 0);
                editor.putBoolean("theme", b);
                editor.apply();

                ((MainActivity)getActivity()).changeTheme(b);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        spinner1.setSelection(((MainActivity)getActivity()).mPref.getBoolean("theme", false) ? 1:0);

        try {
            mNickName.setText(mWini.get("client", "name"));
            mWini.store();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        mNickName.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                String text = charSequence.toString();
                File file = new File(getActivity().getExternalFilesDir(null) + "/SAMP/settings.ini");
                if(file.exists()) {
                    try {
                        mWini.put("client", "name", text);
                        mWini.store();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }

            @Override
            public void afterTextChanged(Editable editable) {

            }
        });

        TextView mButtonReinstall = view.findViewById(R.id.button_reinstall);
        mButtonReinstall.setOnTouchListener(new ButtonAnimator(getContext(), mButtonReinstall));
        mButtonReinstall.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                File dir = new File(getActivity().getExternalFilesDir(null) + "/");
                Util.delete(dir);

                startActivity(new Intent(getActivity(), SplashActivity.class));
                getActivity().finish();
            }
        });

        mKeyboardSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                new SharedPreferenceCore().setBoolean(requireContext().getApplicationContext(), "ANDROID_KEYBOARD", b);
                try {
                    mWini.put("gui", "androidkeyboard23123", b);
                    mWini.store();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        ImageView yt_image = (ImageView) view.findViewById(R.id.youtube);
        yt_image.setOnTouchListener(new ButtonAnimator(getContext(), yt_image));
        yt_image.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent link = new Intent(Intent.ACTION_VIEW, Uri.parse("https://www.youtube.com/@IBROLEPLAY"));
                startActivity(link);
            }
        });

        ImageView discord_image = (ImageView) view.findViewById(R.id.discord);
        discord_image.setOnTouchListener(new ButtonAnimator(getContext(), discord_image));
        discord_image.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent link = new Intent(Intent.ACTION_VIEW, Uri.parse("https://discord.gg/urbanorp"));
                startActivity(link);
            }
        });

        return view;
    }

    @Override
    public void onResume() {
        super.onResume();
        mKeyboardSwitch.setChecked(new SharedPreferenceCore().getBoolean(requireContext().getApplicationContext(), "ANDROID_KEYBOARD"));
    }
}
