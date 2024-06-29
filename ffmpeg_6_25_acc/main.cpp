#include "ffmpeg.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ffmpeg w;
    w.show();

    return a.exec();
}
