package com.sampmobile.game.launcher.fragments;

import android.os.Bundle;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.SimpleItemAnimator;

import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.sampmobile.game.R;
import com.sampmobile.game.launcher.UpdateActivity;
import com.sampmobile.game.launcher.adapters.NewsAdapter;
import com.sampmobile.game.launcher.adapters.ServerAdapter;
import com.sampmobile.game.launcher.data.Config;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

public class NewsFragment extends Fragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_news, container, false);

        NewsAdapter adapter = new NewsAdapter(getContext());

        RecyclerView recyclerView = view.findViewById(R.id.recycler_news);
        ((SimpleItemAnimator) recyclerView.getItemAnimator()).setSupportsChangeAnimations(false);
        recyclerView.setLayoutManager(new LinearLayoutManager(recyclerView.getContext(), LinearLayoutManager.HORIZONTAL, true));
        if (recyclerView.getContext() != null) {
            recyclerView.setAdapter(adapter);
        }

        return view;
    }
}
