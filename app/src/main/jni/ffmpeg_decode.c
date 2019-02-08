#include "com_ad_ffmpeg_VideoUtils.h"
#include <android/log.h>
#include <stdlib.h>
#include <stdio.h>


//编码
#include "include/libavcodec/avcodec.h"
//封装格式处理
#include "include/libavformat/avformat.h"
//像素处理
#include "include/libswscale/swscale.h"
#include "include/libavutil/avutil.h"
#include "include/libavutil/frame.h"

#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"DLM",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"DLM",FORMAT,##__VA_ARGS__);

JNIEXPORT void JNICALL Java_com_ad_ffmpeg_VideoUtils_decode
        (JNIEnv * env, jclass clazz, jstring input_jstr, jstring output_jstr)
        {
            const char* input = (*env)->GetStringUTFChars(env,input_jstr,NULL);
            const char* output = (*env)->GetStringUTFChars(env,output_jstr,NULL);

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
            AVFrame *frame = av_frame_alloc();
            AVFrame *yuvFrame = av_frame_alloc();

            //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
            //缓冲区分配内存
            uint8_t *out_buffer  = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,avCodecContext->width,avCodecContext->height));
            //初始化缓冲区
            avpicture_fill((AVPicture *)yuvFrame,out_buffer,AV_PIX_FMT_YUV420P,avCodecContext->width,avCodecContext->height);

            //输出文件
            FILE* fp_yuv = fopen(output,"wb");

            //用于像素格式转换或缩放
            struct SwsContext *sws_ctx = sws_getContext(
                    avCodecContext->width,avCodecContext->height,
                    avCodecContext-pix_fmt,
                    avCodecContext->width,avCodecContext->height,
                    AV_PIX_FMT_YUV420P,
                    SWS_BILINEAR,NULL,NULL,NULL
                    );

            int len,got_frame,frame_count = 0;


            //6.一帧一帧的读取压缩的视屏数据AVPacket
            while (av_read_frame(pFormatCtx,packet) >= 0)
            {
                //解码AVPacket->AVFrame
                len = avcodec_decode_video2(avCodecContext,frame,&got_frame,packet);

                //非零表示正在解码
                if (got_frame)
                {
                    //frame->yuvFrame(YUV420P)
                    //转为指定的YUV420P像素帧
                    sws_scale(sws_ctx,
                            frame->data,frame->linesize,0,frame->height,
                            yuvFrame->data,yuvFrame->linesize);
                    //向YUV文件保存解码之后的帧数据
                    //AVFrame->YUV
                    //一个像素包含一个Y
                    int y_size = avCodecContext->width * avCodecContext->height;
                    fwrite(yuvFrame->data[0],1,y_size,fp_yuv);
                    fwrite(yuvFrame->data[1],1,y_size,fp_yuv);
                    fwrite(yuvFrame->data[2],1,y_size,fp_yuv);
                    LOGI("解码%帧",frame_count++);
                }
                av_free_packet(packet);
            }

            fclose(fp_yuv);
            av_frame_free(&frame);
            avcodec_close(avCodecContext);
            avformat_free_context(pFormatCtx);

            (*env)->ReleaseStringUTFChars(env,input_jstr,input);
            (*env)->ReleaseStringUTFChars(env,output_jstr,output);
        }