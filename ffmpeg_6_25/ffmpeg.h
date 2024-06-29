#ifndef FFMPEG_H
#define FFMPEG_H

#include <QWidget>
#include <QString>
#include "ADTSHeader.h"
QT_BEGIN_NAMESPACE
namespace Ui { class ffmpeg; }
QT_END_NAMESPACE

class ffmpeg : public QWidget
{
    Q_OBJECT

public:
    ffmpeg(QWidget *parent = nullptr);
    ~ffmpeg();

private:
    Ui::ffmpeg *ui;
};
#endif // FFMPEG_H
