package com.sampmobile.game.launcher.fragments;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.recyclerview.widget.SimpleItemAnimator;

import com.sampmobile.game.R;
import com.sampmobile.game.launcher.adapters.FaqAdapter;

public class FaqFragment extends Fragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_faq, container, false);

        FaqAdapter faqAdapter = new FaqAdapter(getActivity());

        RecyclerView recyclerView = view.findViewById(R.id.recycler_faq);
        ((SimpleItemAnimator) recyclerView.getItemAnimator()).setSupportsChangeAnimations(false);
        recyclerView.setLayoutManager(new LinearLayoutManager(recyclerView.getContext()));
        if (recyclerView.getContext() != null) {
            recyclerView.setAdapter(faqAdapter);
        }

        return view;
    }
}
