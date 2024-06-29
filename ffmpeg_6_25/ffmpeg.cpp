#include "ffmpeg.h"
#include "ui_ffmpeg.h"
#include <QDebug>
#include <stdio.h>
extern "C"{
    #include "libavutil/log.h"
    #include "libavutil/ffversion.h"
    #include "libavformat/avformat.h"
    #include "libavutil/avutil.h"
}
ffmpeg::ffmpeg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ffmpeg)
{
    ui->setupUi(this);
    //qDebug() << "ffmpeg version:" << av_version_info();
    av_log_set_level(AV_LOG_TRACE); //设置打印级别
    /*av_log(nullptr, AV_LOG_ERROR, "this is error level!\n");
    av_log(nullptr, AV_LOG_INFO, "this is info level!\n");
    av_log(nullptr, AV_LOG_WARNING, "this is warning level!\n");
    av_log(nullptr, AV_LOG_DEBUG, "this is debug level!\n");
    av_log(nullptr, AV_LOG_INFO, "ffmpeg version:%s\n",FFMPEG_VERSION);*/


    const char* inputFileName = "D:/QTProject/ffmpegStudy/testVideo.mp4";
    const char* outputFileName = "D:/QTProject/ffmpegStudy/aacData.aac";
    AVFormatContext *pFormatContext = nullptr;
    //输出媒体文件信息
    /*if(0 != avformat_open_input(&pFormatContext, inputFileName, nullptr, nullptr)){
        av_log(nullptr, AV_LOG_ERROR, "open file failed\n");
        qDebug() << "打开文件失败！！！";
        return;
    }
    av_dump_format(pFormatContext, 0, inputFileName, 0);
    avformat_close_input(&pFormatContext);*/
    //提取AAC音频数据
    if(0 != avformat_open_input(&pFormatContext, inputFileName, nullptr, nullptr)){ //1.打开媒体文件
        av_log(nullptr, AV_LOG_ERROR, "open file failed\n");
        qDebug() << "打开文件失败！！！";
        return;
    }

    if(0 > avformat_find_stream_info(pFormatContext, nullptr)){ //2.获取码流信息
        av_log(nullptr, AV_LOG_ERROR, "find stream failed\n");
        avformat_close_input(&pFormatContext);
        return;
    }


    int audioIndex = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);//3.获取音频流
    if(0 > audioIndex){
        av_log(nullptr, AV_LOG_ERROR, "find audio stream failed\n");
        avformat_close_input(&pFormatContext);
        return;
    }
    qDebug() << "audioIndex : " << audioIndex;
    AVPacket packet;
    av_init_packet(&packet); //4.初始化packet

    FILE *dest_fp = fopen(outputFileName, "wb");
    if(nullptr == dest_fp){
        qDebug() << "打开文件失败!!!";
        avformat_close_input(&pFormatContext);
        return;
    }
    while(0 == av_read_frame(pFormatContext, &packet)){//5.读取packet数据
        if(packet.stream_index == audioIndex){
            char adtsHeader[7] = {0};
            getADTSHeader(adtsHeader, packet.size, pFormatContext->streams[audioIndex]->codecpar->profile,
                          pFormatContext->streams[audioIndex]->codecpar->sample_rate,
                          pFormatContext->streams[audioIndex]->codecpar->ch_layout.nb_channels);
            qDebug() << "通道数为:" << pFormatContext->streams[audioIndex]->codecpar->ch_layout.nb_channels;
            fwrite(adtsHeader, 1, sizeof(adtsHeader), dest_fp);
            int writeResult = fwrite(packet.data, 1, packet.size, dest_fp);
            if(writeResult != packet.size){//写文件失败
                qDebug() << "写文件失败!!!";
                avformat_close_input(&pFormatContext);
                fclose(dest_fp);
                return;
            }
        }
        av_packet_unref(&packet); //6.释放packet资源
    }
    if(nullptr != pFormatContext)
        avformat_close_input(&pFormatContext); //7.关闭媒体文件
    if(nullptr != dest_fp)
        fclose(dest_fp);

}

ffmpeg::~ffmpeg()
{
    delete ui;
}

