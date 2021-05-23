#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include "widget.h"
#include "./ui_widget.h"
#include "QFileDialog"

using namespace cv;
using namespace std;

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget)
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
    Mat image, gray, gaus;
    Mat SobelX, SobelY, SobelOut;
    image = imread(imgName.toStdString());

    //1. 原图像大小调整，提高运算效率
    // resize(image, image, Size(500, 300));
    //imshow("1.原图像",image);

    //2. 转化为灰度图
    cvtColor(image, gray, CV_RGB2GRAY);
    //imshow("2.灰度图",gray);

    //3. 高斯平滑滤波
    GaussianBlur(gray, gaus, Size(3, 3), 0);
    //imshow("3.高斯平衡滤波",gaus);

    //4.求得水平和垂直方向灰度图像的梯度差,使用Sobel算子
    Mat imageX16S, imageY16S;
    Sobel(gaus, imageX16S, CV_16S, 1, 0, 3, 1, 0, 4);
    Sobel(gaus, imageY16S, CV_16S, 0, 1, 3, 1, 0, 4);
    convertScaleAbs(imageX16S, SobelX, 1, 0);
    convertScaleAbs(imageY16S, SobelY, 1, 0);
    SobelOut = SobelX - SobelY;
    //imshow("4.X方向梯度",SobelX);
    //imshow("4.Y方向梯度",SobelY);
    //imshow("4.XY方向梯度差",SobelOut);

    //5.均值滤波，消除高频噪声
    blur(SobelOut, SobelOut, Size(3, 3));
    //imshow("5.均值滤波",SobelOut);

    //6.二值化
    Mat Threshold;
    threshold(SobelOut, Threshold, 180, 255, CV_THRESH_BINARY);
    //imshow("6.二值化",Threshold);

    //7.闭运算，填充条形码间隙
    Mat element = getStructuringElement(0, Size(7, 7));
    morphologyEx(Threshold, Threshold, MORPH_CLOSE, element);
    //imshow("7.闭运算",Threshold);

    //8. 腐蚀，去除孤立的点
    erode(Threshold, Threshold, element);
    //imshow("8.腐蚀",Threshold);

    //9. 膨胀，填充条形码间空隙，根据核的大小，有可能需要2~3次膨胀操作
    dilate(Threshold, Threshold, element);
    dilate(Threshold, Threshold, element);
    dilate(Threshold, Threshold, element);
    //imshow("9.膨胀",Threshold);
    vector<vector<Point>> contours;
    vector<Vec4i> hiera;

    //10.通过findContours找到条形码区域的矩形边界
    findContours(Threshold, contours, hiera, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    for (int i = 0; i < contours.size(); i++)
    {
        Rect rect = boundingRect((Mat)contours[i]);
        rectangle(image, rect, Scalar(255), 2);
        //11.将扫描的图像裁剪下来,并保存为相应的结果,保留一些X方向的边界,所以对rect进行一定的扩张
        rect.x = rect.x - (rect.width / 20);
        rect.width = rect.width * 1.1;
        Mat resultImage = Mat(image, rect);
        // imshow("11.图像裁剪", resultImage);
    }

    Mat temp;
    QImage qtImg;
    cv::cvtColor(image, temp, COLOR_BGR2RGB);
    qtImg = QImage((const unsigned char*)(temp.data), temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    ui->imgFrame->clear();
    ui->imgFrame->setPixmap(QPixmap::fromImage(qtImg));
    ui->imgFrame->show();
    imshow("10.找出二维码矩形区域", image);

    waitKey();
}

void Widget::on_detectBarCode_clicked()
{
}

void Widget::on_closeCamera_clicked()
{
}
