#include "ocv.h"

/**
 * @brief 探测条形码,返回条形码Mask图
 */
Mat DetectBarCodeInImage(Mat image)
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
    for (int i = 0; i < 5; ++i)
        dilate(Threshold, Threshold, element);
    // imshow("膨胀", Threshold);

    // 闭运算，填充条形码间隙
    element = getStructuringElement(0, Size(5, 5));
    morphologyEx(Threshold, Threshold, MORPH_CLOSE, element);
    // imshow("再做闭运算",Threshold);

    return Threshold;
}

/**
 * @brief 对条形码画框,返回条形码裁减图
 * @note 可返回未裁减图
 */
Mat DrawFrame4BarCode(Mat image, Mat mask)
{
    Mat resultImage;
    Rect rect;
    // 角点初始化
    vector<vector<Point>> contours;
    vector<Vec4i> hiera;

    // 通过findContours找到条形码区域的矩形边界
    findContours(mask, contours, hiera, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    for (int i = 0; i < contours.size(); i++)
    {
        rect = boundingRect((Mat)contours[i]);
        rectangle(image, rect, Scalar(255, 0, 0), 2);
    }

    // 对带框图片裁减
    resultImage = Mat(image, rect);
    // 对带框图片深拷贝
    // image.copyTo(resultImage);

    // imshow("二维码矩形区域图像裁剪", resultImage);
    return resultImage;
}

/**
 * @brief 获取到中间一行像素的二值化像素值
 */
int GenerateMiddleYData(Mat image, uint8_t *ptrPx)
{
    Mat copy_img;
    image.copyTo(copy_img);
    threshold(copy_img, copy_img, 178, 255, CV_THRESH_BINARY);
    int height = copy_img.rows;
    int weight = copy_img.cols;
    int nc = copy_img.channels();
    // printf("通道数为%d\n",nc);

    // 二维码中间必定是有数据的，把中间那条线经过的每个像素都记录到内存中
    int middleHeight = height / 2;
    for (int i = 0; i < weight; ++i)
    {
        memset(ptrPx + i, (uint8_t)copy_img.ptr<Vec3b>(middleHeight)[i][0], 1);
        // printf("%d, ", copy_img.ptr<Vec3b>(middleHeight)[i][0]);
    }
    memset(ptrPx + weight, 99, 1);
    // printf("\n\n\n");
    return 0;
}

/**
 * @brief 在数组中找到条形码开始和结尾的Index
 */
int FindBarCodeStart_EndPxInArray(uint8_t *array, int *barCodeStartPx, int *barCodeEndPx)
{
    int i = 0;
    int barCodeStartPxFinded = 0;
    while (*(array + i) != 99)
    {
        // 判断第一个黑色像素
        if (barCodeStartPxFinded == 0 && *(array + i) == 0)
        {
            // 把黑色像素的位置打印出来
            // printf("BarCodeStart: %d\n", i);
            barCodeStartPxFinded = 1;
            *barCodeStartPx = i;
        }

        // 判断最后一个黑色像素, 但要注意一旦到最后一个就break掉
        if (barCodeStartPxFinded && *(array + i) == 255 && *(array + i - 1) == 0)
        {
            // 把白色像素的位置打印出来
            // printf("BarCodeEnd: %d\n", i);
            *barCodeEndPx = i - 1;
        }
        i++;
    }
    // printf("\n\n\n%d||%d\n", *barCodeStartPx, *barCodeEndPx);
    return 0;
}

/**
 * @brief 把条形码全部信息转为13位条形码值并输出
 * @author Ziyi Cheng
 */
int codeInfo2BarCodeNumber(uint8_t *pCodeInfo, uint8_t *barCodeNumber)
{
    int nowIndex, temp; //i循环6次；a95位数组的位数
    int index = 0;
    // -----------
    // 条形码每位由7位二进制组成，组成标志位，7位二进制由1个int表示
    int barCodeUnitCompose[7], unitComposeNumFlag, barCodeUnit;

    // 存放getBarCodeData生成的数据
    vector<int> barCodeData;

    // 前三位101
    for (nowIndex = 0; nowIndex < 3; nowIndex++)
    {
        // printf("%d ", pCodeInfo[nowIndex]);
    }
    // printf("\n");

    // 前面有效信息
    for (temp = 0; temp < 6; temp++)
    {
        unitComposeNumFlag = 0;
        barCodeUnit = 0;
        for (nowIndex = 3 + index; nowIndex < 10 + index; nowIndex++)
        {
            // printf("%d ", pCodeInfo[nowIndex]);
            barCodeUnitCompose[unitComposeNumFlag] = pCodeInfo[nowIndex];
            // printf("%d ", barCodeUnitCompose[unitComposeNumFlag]);
            unitComposeNumFlag++;
        }
        // 7位二进制数组成的数组转为1位条形码
        for (int i = 0; i < 7; i++)
        {
            barCodeUnit = barCodeUnit * 10 + barCodeUnitCompose[i];
        }

        // 显示7位转为1位条形码结果
        // printf("%07d\n", barCodeUnit);

        barCodeData = getBarCodeData(barCodeUnit);

        // 这里开始把每个数据做提取
        printf("%d\n", barCodeData[0]);

        index = index + 7;
        // printf("\n");
    }

    // 中间分隔符
    for (nowIndex = 45; nowIndex < 50; nowIndex++)
    {
        // printf("%d ", pCodeInfo[nowIndex]);
    }
    // printf("\n");

    // 后面的有效信息
    index = 0;
    for (temp = 0; temp <= 5; temp++)
    {
        for (nowIndex = 50 + index; nowIndex < 57 + index; nowIndex++)
        {
            // printf("%d ", pCodeInfo[nowIndex]);
        }
        index = index + 7;
        // printf("\n");
    }

    // 后三位101
    for (nowIndex = 92; nowIndex < 95; nowIndex++)
    {
        // printf("%d ", pCodeInfo[nowIndex]);
    }

    return 0;
}

/**
 * @brief 通过7位数字解析出1位数字
 * @author Ziyi Cheng
 */
vector<int> getBarCodeData(int barCodeUnit)
{
    // 创建一个条形码单元的信息容器
    // 第一个数据存条形码单元Num，第二个数据存条形码单元奇偶
    vector<int> barCodeData;
    switch (barCodeUnit)
    {
    // a组(奇数)
    case 1101:
        barCodeData.push_back(0);
        barCodeData.push_back(0);
        break;
    case 11001:
        barCodeData.push_back(1);
        barCodeData.push_back(0);
        break;
    case 10011:
        barCodeData.push_back(2);
        barCodeData.push_back(0);
        break;
    case 111101:
        barCodeData.push_back(3);
        barCodeData.push_back(0);
        break;
    case 100011:
        barCodeData.push_back(4);
        barCodeData.push_back(0);
        break;
    case 110001:
        barCodeData.push_back(5);
        barCodeData.push_back(0);
        break;
    case 101111:
        barCodeData.push_back(6);
        barCodeData.push_back(0);
        break;
    case 111011:
        barCodeData.push_back(7);
        barCodeData.push_back(0);
        break;
    case 110111:
        barCodeData.push_back(8);
        barCodeData.push_back(0);
        break;
    case 1011:
        barCodeData.push_back(9);
        barCodeData.push_back(0);
        break;

    // b组(偶数)
    case 100111:
        barCodeData.push_back(0);
        barCodeData.push_back(1);
        break;
    case 110011:
        barCodeData.push_back(1);
        barCodeData.push_back(1);
        break;
    case 11011:
        barCodeData.push_back(2);
        barCodeData.push_back(1);
        break;
    case 100001:
        barCodeData.push_back(3);
        barCodeData.push_back(1);
        break;
    case 11101:
        barCodeData.push_back(4);
        barCodeData.push_back(1);
        break;
    case 111001:
        barCodeData.push_back(5);
        barCodeData.push_back(1);
        break;
    case 101:
        barCodeData.push_back(6);
        barCodeData.push_back(1);
        break;
    case 10001:
        barCodeData.push_back(7);
        barCodeData.push_back(1);
        break;
    case 1001:
        barCodeData.push_back(8);
        barCodeData.push_back(1);
        break;
    case 10111:
        barCodeData.push_back(9);
        barCodeData.push_back(1);
        break;

    // c组
    case 1110010:
        barCodeData.push_back(0);
        break;
    case 1100110:
        barCodeData.push_back(1);
        break;
    case 1101100:
        barCodeData.push_back(2);
        break;
    case 1000010:
        barCodeData.push_back(3);
        break;
    case 1011100:
        barCodeData.push_back(4);
        break;
    case 1001110:
        barCodeData.push_back(5);
        break;
    case 1010000:
        barCodeData.push_back(6);
        break;
    case 1000100:
        barCodeData.push_back(7);
        break;
    case 1001000:
        barCodeData.push_back(8);
        break;
    case 1110100:
        barCodeData.push_back(9);
        break;
    default:
        barCodeData.push_back(-1);
    }
    return barCodeData;
}
