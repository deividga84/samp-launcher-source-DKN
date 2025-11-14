package com.sampmobile.game.launcher;

import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;

import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.google.android.material.tabs.TabLayout;
import com.sampmobile.game.R;
import com.sampmobile.game.launcher.data.Config;
import com.sampmobile.game.launcher.fragments.DonateFragment;
import com.sampmobile.game.launcher.fragments.FaqFragment;
import com.sampmobile.game.launcher.fragments.HomeFragment;
import com.sampmobile.game.launcher.fragments.NewsFragment;
import com.sampmobile.game.launcher.fragments.SettingsFragment;
import com.sampmobile.game.launcher.util.ConfigValidator;
import com.sampmobile.game.launcher.util.SAMPServerInfo;
import com.sampmobile.game.launcher.util.Util;
import com.sampmobile.game.launcher.util.ViewPagerWithoutSwipe;
import com.sampmobile.game.main.SAMP;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Objects;

public class MainActivity extends SampActivity {
    public int[] tabImages = { R.drawable.ic_home_off, R.drawable.ic_faq_off, R.drawable.ic_newspaper_off, R.drawable.ic_shop_off, R.drawable.ic_settings_off};
    public int[] tabSelectedImages = { R.drawable.ic_home_on, R.drawable.ic_faq_on, R.drawable.ic_newspaper_on, R.drawable.ic_shop_on, R.drawable.ic_settings_on};

    public static ArrayList<SAMPServerInfo> mServersFavouriteList = new ArrayList<>();

    boolean bAdsInitialized = false;

    int online;

    public int theme;

    public int getOnline() {
        return online;
    }

    public void setOnline(int online) {
        this.online = online;
    }

    public void startGta()
    {
        startActivity(new Intent(this, SAMP.class));
        finish();
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        Config.mainContext = this;

        mServersFavouriteList = new ArrayList<>();

        ConfigValidator.validateConfigFiles(this);

        File file = new File(getExternalFilesDir(null) + "/download/update.apk");
        file.getParentFile().mkdirs();
        if (file.exists()) {
            file.delete();
        }

        boolean theme = mPref.getBoolean("theme", false);
        changeTheme(theme);

        /*findViewById(R.id.startgame).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this, GTASA.class));
                finish();
            }
        });

        findViewById(R.id.updatefiles).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(MainActivity.this, UpdateActivity.class));
                finish();
            }
        });*/

        FragmentManager fm = getSupportFragmentManager();
        ViewPagerAdapter sa = new ViewPagerAdapter(fm);
        ViewPagerWithoutSwipe pa = findViewById(R.id.fragment_place);
        pa.setAdapter(sa);

        TabLayout tabLayout = findViewById(R.id.constraintLayout);

        tabLayout.setupWithViewPager(pa);

        for(int  i = 0; i < tabLayout.getTabCount(); i++)
        {
            View inflate = LayoutInflater.from(this).inflate(R.layout.tablayout_item, (ViewGroup) tabLayout, false);

            ImageView image = inflate.findViewById(R.id.imageView2);
            image.setBackgroundResource(tabImages[i]);

            Objects.requireNonNull(tabLayout.getTabAt(i)).setCustomView(inflate);

            tabLayout.clearOnTabSelectedListeners();
            tabLayout.addOnTabSelectedListener(new TabLayout.OnTabSelectedListener() {
                @Override
                public void onTabSelected(TabLayout.Tab tab) {
                    getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);
                    pa.setCurrentItem(tab.getPosition(), true);
                    ((ImageView)tab.getCustomView().findViewById(R.id.imageView2)).setBackgroundResource(tabSelectedImages[tab.getPosition()]);
                }

                @Override
                public void onTabUnselected(TabLayout.Tab tab) {
                    ((ImageView)tab.getCustomView().findViewById(R.id.imageView2)).setBackgroundResource(tabImages[tab.getPosition()]);
                }

                @Override
                public void onTabReselected(TabLayout.Tab tab) {

                }
            });

        }
        ((ImageView)tabLayout.getTabAt(0).getCustomView().findViewById(R.id.imageView2)).setBackgroundResource(tabSelectedImages[0]);

        Volley.newRequestQueue(this).add(new StringRequest("http://177.155.199.15/apimedusa/files/news_config.json", new Response.Listener<String>() {
            @Override
            public void onResponse(String response) {
                String response1;
                try {
                    byte[] u = response.toString().getBytes("ISO-8859-1");
                    response1 = new String(u, "UTF-8");
                } catch (UnsupportedEncodingException e) {
                    throw new RuntimeException(e);
                }

                try {
                    JSONObject jsonObject = new JSONObject(response1);
                    JSONArray jsonArray = jsonObject.getJSONArray("news");
                    Config.mNewsDescription = new String[jsonArray.length()];
                    Config.mNewsTitle = new String[jsonArray.length()];
                    Config.mNewsImage = new String[jsonArray.length()];
                    Config.mBitmap = new Bitmap[jsonArray.length()];

                    for (int i = 0; i < jsonArray.length(); i++) {
                        JSONObject item = jsonArray.getJSONObject(i);

                        Config.mNewsDescription[i] = item.getString("description");
                        Config.mNewsImage[i] = item.getString("image");
                        Config.mNewsTitle[i] = item.getString("title");

                        new Util.LoadNewsImage(i).execute(Config.mNewsImage[i]);
                    }
                } catch (JSONException e) {
                    throw new RuntimeException(e);
                }
            }
        }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {

            }
        }));
    }

    public void changeTheme(boolean theme) {
        if(theme) findViewById(R.id.main_layout).setBackgroundResource(R.drawable.bg_blue);
        else findViewById(R.id.main_layout).setBackgroundResource(R.drawable.bg_red);
    }

    public class ViewPagerAdapter extends FragmentPagerAdapter {

        public ViewPagerAdapter(FragmentManager fragmentManager) {
            super(fragmentManager);
        }

        @NonNull
        @Override
        public Fragment getItem(int position) {
            if(position == 1)
                return new FaqFragment();
            else if(position == 0)
                return new HomeFragment();
            else if(position == 2)
                return new NewsFragment();
            else if(position == 3)
                return new DonateFragment();
            else if(position == 4)
                return new SettingsFragment();
            return new HomeFragment();
        }

        @Override
        public int getCount() {
            return 5;
        }
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }
}
