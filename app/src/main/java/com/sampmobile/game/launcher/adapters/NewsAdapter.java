package com.sampmobile.game.launcher.adapters;

import android.content.Context;
import android.graphics.drawable.BitmapDrawable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.sampmobile.game.R;
import com.sampmobile.game.launcher.data.Config;

public class NewsAdapter extends RecyclerView.Adapter<NewsAdapter.ViewHolder> {

    private Context mContext;

    public NewsAdapter(Context context)
    {
        mContext = context;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.news_item, parent, false);

        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        BitmapDrawable ob = new BitmapDrawable(mContext.getResources(), Config.mBitmap[position]);
        holder.mImage.setBackground(ob);
        holder.mMainText.setText(Config.mNewsTitle[position]);
        holder.mMainInfo.setText(Config.mNewsDescription[position]);
    }

    @Override
    public int getItemCount() {
        return Config.mNewsDescription.length;
    }

    public class ViewHolder extends RecyclerView.ViewHolder
    {
        private View mView;
        public TextView mMainInfo;
        public TextView mMainText;
        public ImageView mImage;

        public ViewHolder(View view) {
            super(view);
            mView = view;
            mImage = view.findViewById(R.id.news_image);
            mMainText = view.findViewById(R.id.news_main);
            mMainInfo = view.findViewById(R.id.news_info);
        }

        public View getView() {
            return mView;
        }
    }
}
