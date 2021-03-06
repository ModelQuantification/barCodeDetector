#include "cam.h"

/**
 * @brief 探测条形码,返回条形码Mask图
 */
int DetectBarCodeInImage4Video(Mat image, Mat &resultImage)
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
    threshold(SobelOut, resultImage, 180, 255, CV_THRESH_BINARY);
    // imshow("二值化",resultImage);

    // 闭运算，填充条形码间隙
    Mat element = getStructuringElement(0, Size(7, 7));
    morphologyEx(resultImage, resultImage, MORPH_CLOSE, element);
    // imshow("闭运算",resultImage);

    // 腐蚀，去除孤立的点
    erode(resultImage, resultImage, element);
    // imshow("腐蚀",resultImage);

    // 膨胀，填充条形码间空隙
    for (int i = 0; i < 5; ++i)
        dilate(resultImage, resultImage, element);
    // imshow("膨胀", resultImage);

    // 闭运算，填充条形码间隙
    element = getStructuringElement(0, Size(5, 5));
    morphologyEx(resultImage, resultImage, MORPH_CLOSE, element);
    // imshow("再做闭运算",resultImage);

    return 0;
}

/**
 * @brief 对条形码画框,返回条形码画框图
 */
int DrawFrame4BarCode4Video(Mat image, Mat mask, Mat &resultImage)
{
    Rect rect;
    // 角点初始化
    vector<vector<Point>> contours;
    vector<Vec4i> hiera;

    // 对原始图片深拷贝，为了保持原始图片不改变
    image.copyTo(resultImage);

    // 通过findContours找到条形码区域的矩形边界
    findContours(mask, contours, hiera, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    for (int i = 0; i < contours.size(); i++)
    {
        rect = boundingRect((Mat)contours[i]);
        rectangle(resultImage, rect, Scalar(255, 0, 0), 2);
    }

    // imshow("二维码画框图", resultImage);
    return 0;
}

/**
 * @brief 对条形码画框,返回条形码裁减图
 */
int cropFrame4BarCode4Video(Mat image, Mat mask, Mat &resultImage)
{
    Rect rect;
    // 角点初始化
    vector<vector<Point>> contours;
    vector<Vec4i> hiera;

    // 对原始图片深拷贝，为了保持原始图片不改变
    image.copyTo(resultImage);

    // 通过findContours找到条形码区域的矩形边界
    findContours(mask, contours, hiera, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    for (int i = 0; i < contours.size(); i++)
    {
        rect = boundingRect((Mat)contours[i]);
    }

    // 对带框图片裁减
    resultImage = Mat(resultImage, rect);

    // imshow("二维码矩形区域图像裁剪", resultImage);
    return 0;
}
