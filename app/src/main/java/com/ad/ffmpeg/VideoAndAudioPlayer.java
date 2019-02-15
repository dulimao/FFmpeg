package com.ad.ffmpeg;


import android.view.Surface;

/**
*@author 杜立茂
*@date 2019/2/14 14:49
*@description 是视频同步解码播放
*/
public class VideoAndAudioPlayer {

    static {
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("video_audio_player");
    }

    public native static void render(String input, Surface surface);

    public native static void sound(String input,String output);

    public native static void play(String input,Surface surface);
}
