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

    // 转化为灰度图
    cvtColor(image, gray, CV_RGB2GRAY);
    //imshow("灰度图",gray);

    // 高斯平滑滤波
    GaussianBlur(gray, gaus, Size(3, 3), 0);
    //imshow("高斯平衡滤波",gaus);

    // 求得水平和垂直方向灰度图像的梯度差,使用Sobel算子
    Mat imageX16S, imageY16S;
    Sobel(gaus, imageX16S, CV_16S, 1, 0, 3, 1, 0, 4);
    Sobel(gaus, imageY16S, CV_16S, 0, 1, 3, 1, 0, 4);
    convertScaleAbs(imageX16S, SobelX, 1, 0);
    convertScaleAbs(imageY16S, SobelY, 1, 0);
    SobelOut = SobelX - SobelY;
    // imshow("XY方向梯度差", SobelOut);

    // 均值滤波，消除高频噪声
    blur(SobelOut, SobelOut, Size(3, 3));
    // imshow("均值滤波",SobelOut);

    // 二值化
    Mat Threshold;
    threshold(SobelOut, Threshold, 180, 255, CV_THRESH_BINARY);
    // imshow("二值化",Threshold);

    // 闭运算，填充条形码间隙
    Mat element = getStructuringElement(0, Size(7, 7));
    morphologyEx(Threshold, Threshold, MORPH_CLOSE, element);
    // imshow("闭运算",Threshold);

    // 腐蚀，去除孤立的点
    erode(Threshold, Threshold, element);
    // imshow("腐蚀",Threshold);

    // 膨胀，填充条形码间空隙
    for (int i = 0; i < 10; ++i)
        dilate(Threshold, Threshold, element);
    imshow("膨胀", Threshold);

    // 闭运算，填充条形码间隙
    element = getStructuringElement(0, Size(5, 5));
    morphologyEx(Threshold, Threshold, MORPH_CLOSE, element);
    // imshow("再做闭运算",Threshold);

    // 角点配置
    vector<vector<Point>> contours;
    vector<Vec4i> hiera;

    // 通过findContours找到条形码区域的矩形边界
    findContours(Threshold, contours, hiera, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    for (int i = 0; i < contours.size(); i++)
    {
        Rect rect = boundingRect((Mat)contours[i]);
        rectangle(image, rect, Scalar(255), 2);
        Mat resultImage = Mat(image, rect);
        // imshow("图像裁剪", resultImage);
        // printf("%d\n", contours.size());
    }

    Mat temp;
    QImage qtImg;
    cv::cvtColor(image, temp, COLOR_BGR2RGB);
    qtImg = QImage((const unsigned char *)(temp.data), temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    ui->imgFrame->clear();
    ui->imgFrame->setPixmap(QPixmap::fromImage(qtImg));
    ui->imgFrame->show();

    imshow("找出二维码矩形区域", image);

    waitKey();
}

void Widget::on_detectBarCode_clicked()
{
}

void Widget::on_closeCamera_clicked()
{
}
