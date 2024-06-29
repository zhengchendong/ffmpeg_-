#ifndef FFMPEH_H264_H
#define FFMPEH_H264_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ffmpeh_H264; }
QT_END_NAMESPACE

class ffmpeh_H264 : public QWidget
{
    Q_OBJECT

public:
    ffmpeh_H264(QWidget *parent = nullptr);

    //提取H264视频信息
    void  extractH264InfoTo(const char*, const char*);
    ~ffmpeh_H264();

private:
    Ui::ffmpeh_H264 *ui;
    const char* inputFileName;
    const char* outputFileName;
};
#endif // FFMPEH_H264_H
