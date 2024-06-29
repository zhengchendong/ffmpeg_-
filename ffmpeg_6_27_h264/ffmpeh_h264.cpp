#include "ffmpeh_h264.h"
#include "ui_ffmpeh_h264.h"
#include <QDebug>
#include <stdio.h>
extern "C"{
    #include "libavutil/avutil.h"
    #include "libavutil/log.h"
    #include "libavformat/avformat.h"
    #include "libavcodec/bsf.h"
}
ffmpeh_H264::ffmpeh_H264(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ffmpeh_H264)
{
    ui->setupUi(this);
    inputFileName = "D:/QTProject/ffmpegStudy/testVideo.mp4";
    outputFileName = "D:/QTProject/ffmpegStudy/h264Data.h264";
    extractH264InfoTo(inputFileName, outputFileName);

}

void ffmpeh_H264::extractH264InfoTo(const char *inputFileName, const char* outputFileName){
    av_log_set_level(AV_LOG_TRACE);

    AVFormatContext *inputFormatContext = nullptr;
    if(avformat_open_input(&inputFormatContext, inputFileName, nullptr, nullptr) < 0){//1.打开文件
        av_log(nullptr, AV_LOG_ERROR, "open file failed\n");
        return;
    }

    if(0 > avformat_find_stream_info(inputFormatContext, nullptr)){ //2.获取码流信息
        av_log(nullptr, AV_LOG_ERROR, "find stream failed\n");
        avformat_close_input(&inputFormatContext);
        return;
    }


    int videoIndex = av_find_best_stream(inputFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);//3.获取视频流
    if(0 > videoIndex){
        av_log(nullptr, AV_LOG_ERROR, "find video stream failed\n");
        avformat_close_input(&inputFormatContext);
        return;
    }
    qDebug() << "videoIndex : " << videoIndex;
    AVPacket packet;
    av_init_packet(&packet); //4.初始化packet

    FILE* dest_fp =  fopen(outputFileName, "wb+");//打开输出文件
    if(!dest_fp){
        qDebug() << "打开输出文件失败!!!";
        avformat_close_input(&inputFormatContext);
        return;
    }

    //进行格式转换 -> h264_mp4toannexb
    const AVBitStreamFilter *bsf = av_bsf_get_by_name("h264_mp4toannexb");
    if(!bsf){
        qDebug() << "创建失败!!!";
        avformat_close_input(&inputFormatContext);
        return;
    }
    AVBSFContext *bsfContext = nullptr;
    av_bsf_alloc(bsf, &bsfContext);
    avcodec_parameters_copy(bsfContext -> par_in, inputFormatContext -> streams[videoIndex] -> codecpar);
    av_bsf_init(bsfContext);
    while(0 == av_read_frame(inputFormatContext,&packet)){//5.读取packet数据
        if(packet.stream_index == videoIndex){//找到视频流信息写入文件
            if(0 == av_bsf_send_packet(bsfContext, &packet)){
                //接收数据进行转换
                while(0 == av_bsf_receive_packet(bsfContext, &packet)){
                    int writeResult = fwrite(packet.data, 1, packet.size, dest_fp);
                    if(writeResult != packet.size){//文件写失败
                        av_packet_unref(&packet);
                        break;
                    }
                }
            }
            //qDebug() << "---------------";
        }
        av_packet_unref(&packet);//6.释放packet资源
    }
    if(nullptr != inputFormatContext)
        avformat_close_input(&inputFormatContext); //7.关闭媒体文件
    if(nullptr != dest_fp)
        fclose(dest_fp);
    if(nullptr != bsf)
        av_bsf_free(&bsfContext);
}


ffmpeh_H264::~ffmpeh_H264()
{
    delete ui;
}

