package com.sampmobile.game.launcher.fragments;

import android.os.Bundle;
import android.text.Html;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.SimpleItemAnimator;

import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.sampmobile.game.R;
import com.sampmobile.game.launcher.adapters.ServerAdapter;
import com.sampmobile.game.launcher.data.Config;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;

public class HomeFragment extends Fragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_home, container, false);

        Volley.newRequestQueue(getContext()).add(new StringRequest("http://178.132.198.226/apineon/api/players.php", new Response.Listener<String>() {
            @Override
            public void onResponse(String response) {
                String response1;
                try {
                    byte[] u = response.toString().getBytes("ISO-8859-1");
                    response1 = new String(u, "UTF-8");
                } catch (UnsupportedEncodingException e) {
                    throw new RuntimeException(e);
                }

                Log.d("x1y2z", "response: " + response1);

                try {
                    JSONObject jsonObject = new JSONObject(response1);
                    JSONArray jsonArray = jsonObject.getJSONArray("servers");
                    Config.mServersOnline = new String[jsonArray.length()];
                    Config.mServersPing = new int[jsonArray.length()];
                    Config.mServersDoubling = new int[jsonArray.length()];
                    Config.mServersIsNew = new int[jsonArray.length()];

                    for (int i = 0; i < jsonArray.length(); i++) {
                        JSONObject item = jsonArray.getJSONObject(i);

                        Config.mServersOnline[i] = item.getString("players1");
                        Config.mServersPing[i] = item.getInt("ping");
                        Config.mServersDoubling[i] = item.getInt("doubling");
                        Config.mServersIsNew[i] = item.getInt("new");
                    }

                    if(jsonArray.length() != 0 && Config.mServersOnline != null && Config.mServersPing != null && Config.mServersDoubling != null || Config.mServersIsNew != null) {
                        ServerAdapter adapter = new ServerAdapter(getContext(), 0);

                        RecyclerView recyclerView = view.findViewById(R.id.recycler_servers);
                        ((SimpleItemAnimator) recyclerView.getItemAnimator()).setSupportsChangeAnimations(false);
                        recyclerView.setLayoutManager(new LinearLayoutManager(recyclerView.getContext()));
                        if (recyclerView.getContext() != null) {
                            recyclerView.setAdapter(adapter);
                        }

                        int online = 0;

                        for (int i = 0; i < Config.mServersOnline.length; i++) {
                            online = online + Integer.parseInt(Config.mServersOnline[i]);
                        }

                        String text = "<font color=\"#FFFFFF\">Online agora: </font><font color=\"#FBBF23\">" + online + "</font></font><font color=\"#FFFFFF\"> jogadores</font>";
                        ((TextView) view.findViewById(R.id.server_fullplayers_text)).setText(Html.fromHtml(text), TextView.BufferType.SPANNABLE);
                    }
                } catch (JSONException e) {
                    throw new RuntimeException(e);
                }
            }
        }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                Log.d("x1y2z", "error: " + error.networkResponse);
            }
        }));


        return view;
    }
}
