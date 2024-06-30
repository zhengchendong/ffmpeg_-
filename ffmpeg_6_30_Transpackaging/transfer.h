#ifndef TRANSFER_H
#define TRANSFER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class transfer; }
QT_END_NAMESPACE

class transfer : public QWidget
{
    Q_OBJECT

public:
    transfer(QWidget *parent = nullptr);
    ~transfer();

    void _mp4TransFlv();

private:
    Ui::transfer *ui;
    const char* inputFileName = nullptr;
    const char* outputFileName = nullptr;
};
#endif // TRANSFER_H
