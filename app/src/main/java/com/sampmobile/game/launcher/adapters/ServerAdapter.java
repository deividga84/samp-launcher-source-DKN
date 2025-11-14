package com.sampmobile.game.launcher.adapters;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.media.Image;
import android.os.AsyncTask;
import android.util.Log;
import android.util.Pair;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.recyclerview.widget.RecyclerView;

import com.sampmobile.game.R;
import com.sampmobile.game.launcher.MainActivity;
import com.sampmobile.game.launcher.data.Config;
import com.sampmobile.game.launcher.util.ButtonAnimator;
import com.sampmobile.game.launcher.util.SAMPServerInfo;
import com.sampmobile.game.launcher.util.SampQueryApi;
import com.sampmobile.game.main.SAMP;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

public class ServerAdapter extends RecyclerView.Adapter<ServerAdapter.ViewHolder> {

    public Context mContext;
    int position = 0;

    private int pagePosition;

    public ServerAdapter(Context context, int page)
    {
        mContext = context;
        pagePosition = page;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.server_item, parent, false);

        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {

        holder.mOnlineText.setText(Config.mServersOnline[position] + "/300");
        holder.mPing.setText(Config.mServersPing[position] + " ms");

        if(Config.mServersIsNew[position] == 1)
        {
            holder.mIsNew.setVisibility(View.VISIBLE);
        } else {
            holder.mIsNew.setVisibility(View.GONE);
        }

        if(Config.mServersDoubling[position] == 1)
        {
            holder.mJoin.setImageResource(R.drawable.ic_join_doubling);
        }
        else if(Config.mServersDoubling[position] == 0)
        {
            holder.mJoin.setImageResource(R.drawable.ic_join);
        }

        holder.mJoin.setOnTouchListener(new ButtonAnimator(mContext, holder.mJoin));
        holder.mJoin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                ((MainActivity)mContext).startGta();
            }
        });
    }

    @Override
    public int getItemCount() {
        return Config.mServersOnline.length;
    }

    public class ViewHolder extends RecyclerView.ViewHolder
    {
        private View mView;
        public TextView mServerNameText;
        public TextView mOnlineText;
        public ConstraintLayout mLayout;
        public ImageView mJoin;
        public TextView mPing;
        public ImageView mIsNew;

        public ViewHolder(View view) {
            super(view);
            mView = view;
            mLayout = view.findViewById(R.id.server_item_layout);
            mServerNameText = view.findViewById(R.id.textView6);
            mOnlineText = view.findViewById(R.id.online_text);
            mJoin = view.findViewById(R.id.server_join);
            mPing = view.findViewById(R.id.ping_text);
            mIsNew = view.findViewById(R.id.new_image);
        }

        public View getView() {
            return mView;
        }
    }
}
