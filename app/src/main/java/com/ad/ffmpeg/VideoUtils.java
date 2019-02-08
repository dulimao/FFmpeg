package com.ad.ffmpeg;

public class VideoUtils {

    static{
        //必须按顺序加载
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("myffmpeg");
    }

    //mp4->yuv420p
    public native static void decode(String input,String output);
}
