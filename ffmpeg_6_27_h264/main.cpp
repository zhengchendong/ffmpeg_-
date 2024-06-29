#include "ffmpeh_h264.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ffmpeh_H264 w;
    w.show();
    return a.exec();
}
