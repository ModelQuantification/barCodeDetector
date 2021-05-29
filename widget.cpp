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

Widget::Widget(QWidget *parent):QWidget(parent), ui(new Ui::Widget)
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
    Mat image;
    
    image = imread(imgName.toStdString());

    // 探测条形码并显示二值化全图
    image = DetectBarCodeInImage(image);

    // 角点初始化
    vector<vector<Point>> contours;
    vector<Vec4i> hiera;

    // 通过findContours找到条形码区域的矩形边界
    findContours(image, contours, hiera, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    for (int i = 0; i < contours.size(); i++)
    {
        Rect rect = boundingRect((Mat)contours[i]);
        rectangle(image, rect, Scalar(255), 2);
        Mat resultImage = Mat(image, rect);
        // imshow("二维码矩形区域图像裁剪", resultImage);
    }

    // 在QT中显示效果
    Mat temp;
    QImage qtImg;
    cv::cvtColor(image, temp, COLOR_BGR2RGB);
    qtImg = QImage((const unsigned char *)(temp.data), temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    ui->imgFrame->clear();
    ui->imgFrame->setPixmap(QPixmap::fromImage(qtImg));
    ui->imgFrame->show();

    // 当没有imshow时关闭waitKey
    waitKey();
}

void Widget::on_detectBarCode_clicked()
{
}

void Widget::on_closeCamera_clicked()
{
}
