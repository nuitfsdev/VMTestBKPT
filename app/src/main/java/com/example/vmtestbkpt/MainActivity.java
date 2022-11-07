package com.example.vmtestbkpt;

import android.content.pm.PackageManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.example.vmtestbkpt.Utils;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {
    static {
         System.loadLibrary("vmtestbkpt");
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final TextView textView = (TextView) findViewById(R.id.show);


        Button bkpt = (Button)findViewById(R.id.bkpt);
        bkpt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean bkptresult = checkQemuBreakpoint();
                if(bkptresult){
                    textView.setText("Check Success! This is an emulator!");
                }
                else{
                    textView.setText("Check Success! This is an Android device!");
                }
            }
        });
    }
    @Override
    public void onRequestPermissionsResult(int requestCode,String permissions[], int[] grantResults) {
        switch (requestCode){
            case 1:
                if(grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED){
                    Utils.hasKnownDeviceId((TextView)findViewById(R.id.show),MainActivity.this, getApplicationContext());
                }else{
                    Toast.makeText(this,"You denied",Toast.LENGTH_SHORT);
                }

        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

    //public native String mainthread();
    //public native String SMCdetection();


    public native int qemuBkpt();

    public boolean checkQemuBreakpoint() {
        boolean hit_breakpoint = false;

        // Potentially you may want to see if this is a specific value
        int result = qemuBkpt();

        if (result > 0) {
            hit_breakpoint = true;
        }

        return hit_breakpoint;
    }
}
