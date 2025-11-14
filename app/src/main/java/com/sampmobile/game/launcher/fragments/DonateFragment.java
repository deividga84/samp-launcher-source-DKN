package com.sampmobile.game.launcher.fragments;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import androidx.fragment.app.Fragment;

import com.sampmobile.game.R;

public class DonateFragment extends Fragment {

    int serverId;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_donate, container, false);

        String[] sort = {"Medusa #1"};
        Spinner spinner = view.findViewById(R.id.server_select);

        ArrayAdapter<String> adapter = new ArrayAdapter(getActivity(), R.layout.spinner_item, sort);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner.setAdapter(adapter);

        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                serverId = i;
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        view.findViewById(R.id.button_donate).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse("https://samp-mobile.com/shop/"));
                startActivity(browserIntent);
            }
        });

        // https://pay.kassa.shop/?m=16219&oa=50&s=c97682232677f57cb3ec2a815f65e873&r=aHR0cHM6Ly9zYW1wLW1vYmlsZS5jb20v&currency=RUB&o=1684420622&us_account=Gor_Grigoryan&_r=https%3A%2F%2Fsamp-mobile.com%2F&strd=1

        return view;
    }
}
