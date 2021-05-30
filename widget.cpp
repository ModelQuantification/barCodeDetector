#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include "widget.h"
#include "./ui_widget.h"
#include "QFileDialog"

#include "./User/ocv.h"

using namespace cv;
using namespace std;

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_openCamera_clicked()
{
}

void Widget::on_detectImage_clicked()
{
    QString imgName = QFileDialog::getOpenFileName(this, tr("open image file"), "./",
                                                   tr("Image files(*.png *.jpg);;All files (*.*)"));
    if (imgName.isEmpty())
        return;
    Mat originImg, barCodeMaskImg;
    Mat barCodeImg, framedBarCodeImg;

    originImg = imread(imgName.toStdString());

    barCodeMaskImg = DetectBarCodeInImage(originImg);

    // 可以修改函数代码为裁减或带框
    barCodeImg = DrawFrame4BarCode(originImg, barCodeMaskImg);

    // 读取条形码中间一行像素并返回
    int ret;

    // 读取条形码宽度，把图片的宽度作为请求的内存大小
    int barCodeWeight = barCodeImg.cols;
    uint8_t *pWeight = (uint8_t *)malloc(barCodeWeight * sizeof(uint8_t) + 1);
    if (pWeight == NULL) // 如果请求不了内存
        return;
    ret = GenerateMiddleYData(barCodeImg, pWeight);

    // 找到开始位置和结束位置
    if (0 == ret)
    {
        int i = 0;
        // while (*(pWeight + i) != 99)
        // {

        // }
    }

    // unitTest 检验返回的内存是否正确
    // if (0 == ret)
    // {
    //     int i = 0;
    //     while (*(pWeight + i) != 99)
    //     {
    //         printf("%d\n", *(pWeight + i));
    //         i++;
    //     }
    // }

    // 在QT中显示效果
    Mat cvTempImg;
    QImage qtShowImg;
    cv::cvtColor(barCodeImg, cvTempImg, COLOR_BGR2RGB);
    qtShowImg = QImage((const unsigned char *)(cvTempImg.data), cvTempImg.cols, cvTempImg.rows, cvTempImg.step, QImage::Format_RGB888);
    ui->imgFrame->clear();
    ui->imgFrame->setPixmap(QPixmap::fromImage(qtShowImg));
    ui->imgFrame->show();

    // 释放条形码宽度指针
    free(pWeight);
    pWeight = NULL;

    // 当没有imshow时关闭waitKey
    waitKey();
}

void Widget::on_detectBarCode_clicked()
{
}

void Widget::on_closeCamera_clicked()
{
}
