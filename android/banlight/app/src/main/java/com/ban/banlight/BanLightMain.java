package com.ban.banlight;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Switch;
import android.widget.TextView;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class BanLightMain extends AppCompatActivity {

    private BluetoothAdapter BA;
    private TextView mTextMessage;

    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener
            = new BottomNavigationView.OnNavigationItemSelectedListener() {

        @Override
        public boolean onNavigationItemSelected(@NonNull MenuItem item) {
            switch (item.getItemId()) {
                case R.id.navigation_home:
                    mTextMessage.setText(R.string.title_home);
                    return true;
                case R.id.navigation_dashboard:
                    mTextMessage.setText(R.string.title_dashboard);
                    return true;
                case R.id.navigation_notifications:
                    mTextMessage.setText(R.string.title_notifications);
                    return true;
            }
            return false;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        BA = BluetoothAdapter.getDefaultAdapter();

        setContentView(R.layout.activity_ban_light_main);
        mTextMessage = (TextView) findViewById(R.id.message);

        final Switch switchOnOff = (Switch) findViewById(R.id.switchOnOff);
        switchOnOff.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean hasBT = false;
                if (BA != null) {

                    if (!BA.isEnabled()) {
                        Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                        startActivityForResult(turnOn, 0);
                    } else {
                        hasBT = true;
                    }

                    if (hasBT) {
                        for (BluetoothDevice dev : BA.getBondedDevices()) {
                            try {
                                if(dev.getName().startsWith("BANLAMP")) {
                                    BluetoothSocket socket = dev.createRfcommSocketToServiceRecord(dev.getUuids()[0].getUuid());
                                    socket.connect();
                                    OutputStream outputStream = socket.getOutputStream();
                                    InputStream inStream = socket.getInputStream();

                                    if (switchOnOff.isChecked()) {
                                        outputStream.write("ON\r\n".getBytes());
                                    } else {
                                        outputStream.write("OFF\n".getBytes());
                                    }

                                    outputStream.close();
                                    inStream.close();
                                }
                            } catch(IOException e){
                                e.printStackTrace();
                                Log.e("error", "No appropriate paired devices.");
                            }

                        }
                    }
                }
            }

        });

        BottomNavigationView navigation = (BottomNavigationView) findViewById(R.id.navigation);
        navigation.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);
    }

}
