#ifndef ADTSHEADER_H
#define ADTSHEADER_H
//设置采样率
const int sampleFrequencyTable[] = {
    96000,
    88200,
    64000,
    48000,
    44100,
    32000,
    22050,
    16000,
    12000,
    11025,
    8000,
    7350
};

int getADTSHeader(char* adtsHeader, int packetSize, int profile, int sampleRate, int channels);
#endif // ADTSHEADER_H
