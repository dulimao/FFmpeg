package com.ad.ffmpeg;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


    }

    public void decode(View view) {
        String input = new File(Environment.getExternalStorageDirectory(),"input.mp4").getAbsolutePath();
        String output = new File(Environment.getExternalStorageDirectory(),"output_1280x720_yuv420p.yuv").getAbsolutePath();
        VideoUtils.decode(input,output);

    }
}
