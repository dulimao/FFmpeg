package com.ad.ffmpeg;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
*@author 杜立茂
*@date 2019/2/9 9:20
*@description native原生绘制
*/
public class VideoSurfaceView extends SurfaceView {

    public VideoSurfaceView(Context context) {
        this(context,null);
    }

    public VideoSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs,0);
    }

    public VideoSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init(){
        SurfaceHolder holder = this.getHolder();
        holder.setFormat(PixelFormat.RGBA_8888);
    }
}
