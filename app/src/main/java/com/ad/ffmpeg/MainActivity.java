package com.ad.ffmpeg;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {


    private SurfaceView mSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mSurfaceView = this.findViewById(R.id.video_view);


    }

    /**
     * 解码
     * @param view
     */
    public void decode(View view) {
        String input = new File(Environment.getExternalStorageDirectory(),"input.mp4").getAbsolutePath();
        String output = new File(Environment.getExternalStorageDirectory(),"output_1280x720_yuv420p.yuv").getAbsolutePath();
        VideoUtils.decode(input,output);

    }

    /**
     * native原生视频渲染
     */
    public void render(){

    }
}
