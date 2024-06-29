#include "ADTSHeader.h"
#include <stdint.h>
int getADTSHeader(char *adtsHeader, int packetSize, int profile, int sampleRate, int channels)
{
    int sampleFrequenceIndex = 3; //设置默认采样率为44100
    int adtsLeght = packetSize + 7;

    for(int i = 0; i < sizeof(sampleFrequencyTable) / sizeof (sampleFrequencyTable[0]); i++){
        if(sampleRate == sampleFrequencyTable[i]){
            sampleFrequenceIndex = i;// 获得采样率
            break;
        }
    }
    adtsHeader[0] = 0xff;

    adtsHeader[1] = 0xf0;
    adtsHeader[1] |= (0 << 3);
    adtsHeader[1] |= (0 << 1);
    adtsHeader[1] |= 1;

    adtsHeader[2] = (profile << 6);
    adtsHeader[2] |= (sampleFrequenceIndex & 0x0f) << 2;
    adtsHeader[2] |= (0 << 1);
    adtsHeader[2] |= (channels &0x04) >> 2;

    adtsHeader[3] = (channels & 0x03) << 6;
    adtsHeader[3] |= (0 << 5);
    adtsHeader[3] |= (0 << 4);
    adtsHeader[3] |= (0 << 3);
    adtsHeader[3] |= (0 << 2);
    adtsHeader[3] |= ((adtsLeght &0x1800) >> 11);

    adtsHeader[4] = (uint8_t)((adtsLeght & 0x7f8) >> 3);
    adtsHeader[5] = (uint8_t)((adtsLeght & 0x7) << 5);
    adtsHeader[5] |= 0x1f;
    adtsHeader[6] = 0xfc;

    return 0;
}

