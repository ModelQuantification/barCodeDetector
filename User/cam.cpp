#include "cam.h"


/**
 * @brief 探测条形码,返回条形码Mask图
 */
Mat DetectBarCodeInImage4Video(Mat image, Mat Threshold)
{
    Mat gray, gaus;
    Mat SobelX, SobelY, SobelOut;
    // 转化为灰度图
    cvtColor(image, gray, CV_RGB2GRAY);
    // imshow("灰度图",gray);

    // 高斯平滑滤波
    GaussianBlur(gray, gaus, Size(3, 3), 0);
    // imshow("高斯平衡滤波",gaus);

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
    threshold(SobelOut, Threshold, 180, 255, CV_THRESH_BINARY);
    // imshow("二值化",Threshold);

    // // 闭运算，填充条形码间隙
    // Mat element = getStructuringElement(0, Size(7, 7));
    // morphologyEx(Threshold, Threshold, MORPH_CLOSE, element);
    // // imshow("闭运算",Threshold);

    // // 腐蚀，去除孤立的点
    // erode(Threshold, Threshold, element);
    // // imshow("腐蚀",Threshold);

    // // 膨胀，填充条形码间空隙
    // for (int i = 0; i < 5; ++i)
    //     dilate(Threshold, Threshold, element);
    // // imshow("膨胀", Threshold);

    // // 闭运算，填充条形码间隙
    // element = getStructuringElement(0, Size(5, 5));
    // morphologyEx(Threshold, Threshold, MORPH_CLOSE, element);
    // // imshow("再做闭运算",Threshold);

    return Threshold;
}
