#include "com_ad_ffmpeg_VideoUtils.h"
#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>

//https://chromium.googlesource.com/external/libyuv
//视频渲染

//编码
#include "include/ffmpeg/libavcodec/avcodec.h"
//封装格式处理
#include "include/ffmpeg/libavformat/avformat.h"
//像素处理
#include "include/ffmpeg/libswscale/swscale.h"
#include "include/ffmpeg/libavutil/avutil.h"
#include "include/ffmpeg/libavutil/frame.h"

//yuv->rgb转换库
#include "include/libyuv/libyuv.h"

#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"DLM",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"DLM",FORMAT,##__VA_ARGS__);

JNIEXPORT void JNICALL Java_com_ad_ffmpeg_VideoUtils_render
        (JNIEnv * env, jclass clazz, jstring input_jstr, jobject surface)
        {
            const char* input = (*env)->GetStringUTFChars(env,input_jstr,NULL);


            //1.注册组件
            av_register_all();
            //封装格式上下文
            AVFormatContext *pFormatCtx = avformat_alloc_context();
            //2.打开文件
            int i = avformat_open_input(&pFormatCtx,input,NULL,NULL);
            if (i != 0){
                LOGE("%s","打开输入视频文件失败");
                return;
            }
            //3.获取视频信息
            if (avformat_find_stream_info(pFormatCtx,NULL) < 0)
            {
                LOGE("%s","获取视频信息失败");
                return;
            }
            //4.得到音频或视频解码器,找到视频对应的AVStream所在的索引位置
            int video_stream_idx = -1;
            int j = 0;
            for (; j < pFormatCtx->nb_streams; j++)
            {
                //根据类型判断是否是视频流
                if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    video_stream_idx = i;
                    break;
                }
            }

            if (video_stream_idx == -1)
            {
                LOGE("%s","获取视频信息失败");
                return;
            }

            //解码器上下文结构体
            AVCodecContext *avCodecContext = pFormatCtx->streams[video_stream_idx]->codec;

            //解码器
            AVCodec *pCodec = avcodec_find_decoder(avCodecContext->codec_id);

            if (pCodec == NULL)
            {
                //视频可能加密过，所以无法解码
                LOGE("%s","无法解码");
                return;
            }

            //5.打开解码器
            if (avcodec_open2(avCodecContext,pCodec,NULL) < 0)
            {
                LOGE("%s","解码器无法打开");
                return;
            }


            //编码数据
            AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

            //解码数据（像素数据）
            AVFrame *yuv_frame = av_frame_alloc();
            AVFrame *rgb_frame = av_frame_alloc();


            //(1)native绘制
            //窗体
            ANativeWindow *native_window = ANativeWindow_fromSurface(env,surface);
            //绘制时的缓冲区
            ANativeWindow_Buffer outBuffer;
            int len,got_frame,frame_count = 0;

            //6.一帧一帧的读取压缩的视频数据AVpacket
            while (av_read_frame(pFormatCtx,packet) >= 0)
            {
                len = avcodec_decode_video2(avCodecContext,yuv_frame,&got_frame,packet);
                //非零正在解码
                if (got_frame)
                {
                    LOGI("正在解码%d帧",frame_count);
                    //1-1. lock
                    //设置缓冲区的属性，宽高，像素格式
                    ANativeWindow_setBuffersGeometry(native_window,avCodecContext->width,avCodecContext->height,WINDOW_FORMAT_RGBA_8888);
                    ANativeWindow_lock(native_window,&outBuffer,NULL);

                    //1-2.设置rgb_frame的属性，宽高，像素格式，和缓冲区
                    //rgb_frame缓冲区和outBuffer.bits是同一块内存
                    avpicture_fill((AVPicture *)rgb_frame,outBuffer.bits,PIX_FMT_RGBA,avCodecContext->width,avCodecContext->height);

                    //yuv->rgba-8888
                    I420ToABGR(yuv_frame->data[0],yuv_frame->linesize[0],
                            yuv_frame->data[2],yuv_frame->linesize[2],//调换顺序，颜色显示才正常
                            yuv_frame->data[1],yuv_frame->linesize[1],
                            rgb_frame->data[0],rgb_frame->linesize[0],
                               avCodecContext->width,avCodecContext->height);

                    //1-3.unlock 渲染
                    ANativeWindow_unlockAndPost(native_window);

                    usleep(1000 * 16);


                }

                av_free_packet(packet);
            }

            ANativeWindow_release(native_window);
            av_frame_free(&yuv_frame);
            avcodec_close(avCodecContext);
            avformat_free_context(pFormatCtx);

            (*env)->ReleaseStringUTFChars(env,input_jstr,input);
        }