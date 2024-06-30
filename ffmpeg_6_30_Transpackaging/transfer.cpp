#include "transfer.h"
#include "ui_transfer.h"
extern "C"{
    #include <libavutil/log.h>
    #include <libavutil/avutil.h>
    #include <libavutil/mem.h>
    #include <libavformat/avformat.h>
}
transfer::transfer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::transfer)
{
    ui->setupUi(this);
    inputFileName = "D:/QTProject/ffmpegStudy/ffmpeg_study/testVideo.mp4";
    outputFileName = "D:/QTProject/ffmpegStudy/ffmpeg_study/flvData.flv";

    _mp4TransFlv();
}

transfer::~transfer()
{
    delete ui;
}

void transfer::_mp4TransFlv()
{
    av_log_set_level(AV_LOG_TRACE);

    AVFormatContext *inputFmctx = nullptr;
    //1.打开媒体输入文件
    if(0 != avformat_open_input(&inputFmctx, inputFileName, nullptr, nullptr)){
        av_log(nullptr, AV_LOG_ERROR, "open inputFile faied!!!");
        return;
    }

    //2.获取输入流信息
    if(0 != avformat_find_stream_info(inputFmctx, nullptr)){
        av_log(nullptr, AV_LOG_ERROR, "get streamInfo failed!!!");
        avformat_close_input(&inputFmctx);
        return;
    }

    //3.创建输出流上下文
    AVFormatContext *outputFmctx = nullptr;
    if(0 > avformat_alloc_output_context2(&outputFmctx, nullptr, nullptr, outputFileName)){
        av_log(nullptr, AV_LOG_ERROR, "create outputContext failed!!!");
        avformat_close_input(&inputFmctx);
        return;
    }

    //此时只处理视频中的视频流和音频流和字幕流
    int streamCount = inputFmctx -> nb_streams; //媒体文件里流的数量
    int *handleStreamIndexArray = (int*)av_malloc_array(streamCount, sizeof(int));

    if(nullptr == handleStreamIndexArray){
        av_log(nullptr, AV_LOG_ERROR, "create array failed!!!");
        avformat_close_input(&inputFmctx);
    }
    int stramIndex = 0;
    for(int i = 0; i < streamCount; i++){
        AVStream *inStream = inputFmctx -> streams[i];
        if(inStream -> codecpar -> codec_type != AVMEDIA_TYPE_VIDEO && // -> 视频流
           inStream -> codecpar -> codec_type != AVMEDIA_TYPE_AUDIO && // -> 音频流
           inStream -> codecpar -> codec_type != AVMEDIA_TYPE_SUBTITLE){ // -> 字幕流
                handleStreamIndexArray[i] = -1;
                continue;
        }
        handleStreamIndexArray[i] = stramIndex;
        stramIndex++;
        //4.创建输出码流的AVStrteam
        AVStream *outStream = avformat_new_stream(outputFmctx, nullptr);
        if(nullptr ==outStream){
            av_log(nullptr, AV_LOG_ERROR, "create outStream failed!!!");
            avformat_close_input(&inputFmctx);
            av_free(handleStreamIndexArray);
        }

        //5.拷贝编码参数
        avcodec_parameters_copy(outStream -> codecpar, inStream -> codecpar);
        outStream -> codecpar -> codec_tag = 0;
    }

    if(!(outputFmctx -> oformat -> flags & AVFMT_NOFILE)){
        if(0 > avio_open(&outputFmctx -> pb, outputFileName, AVIO_FLAG_WRITE)){
            av_log(nullptr, AV_LOG_ERROR, "write failed!!!");
            avformat_close_input(&inputFmctx);
            av_free(handleStreamIndexArray);
        }
    }
    //6.写入视频文件头
    avformat_write_header(outputFmctx, nullptr);

    //7.读取输入视频流
    AVPacket packet;
    av_init_packet(&packet);
    while(0 == av_read_frame(inputFmctx, &packet)){
        if(packet.stream_index >= streamCount || -1 == handleStreamIndexArray[packet.stream_index]){ //只处理三种流
            av_packet_unref(&packet);
            continue;
        }
        //8.计算pds/dts/duration
        AVStream *inputStream = inputFmctx -> streams[packet.stream_index];
        AVStream *outputStream = outputFmctx -> streams[packet.stream_index];

        packet.stream_index = handleStreamIndexArray[packet.stream_index];

        packet.pts = av_rescale_q(packet.pts, inputStream -> time_base, outputStream -> time_base);
        packet.dts = av_rescale_q(packet.dts, inputStream -> time_base, outputStream -> time_base);
        packet.duration = av_rescale_q(packet.duration, inputStream -> time_base, outputStream -> time_base);
        packet.pos = -1;
        //9.写入视频流数据
        av_interleaved_write_frame(outputFmctx, &packet);
        av_packet_unref(&packet);
    }

    //10.写入文件视频末尾
    if(0 != av_write_trailer(outputFmctx)){
        av_log(nullptr, AV_LOG_ERROR, "write failed!!!");
        avformat_close_input(&inputFmctx);
        av_free(handleStreamIndexArray);
        avio_close(outputFmctx -> pb);
    }
    av_log(nullptr, AV_LOG_INFO, "success!!!");
    avformat_close_input(&inputFmctx);
    av_free(handleStreamIndexArray);
    avio_close(outputFmctx -> pb);
}

