#include "ocr.h"

/**
 * @brief 方法一：跳跃法识别条形码
 * @param img 读取一个裁减有条形码的图片
 * @param barCodeNumStr 读取一个存放条形码数组的位置
 */
int methodLeaping2DetectBarCodeImg(Mat barCodeImg, char *barCodeNumStr)
{
    // 读取条形码宽度，把图片的宽度作为请求的内存大小
    int barCodeWeight = barCodeImg.cols;
    uint8_t *pWeight = (uint8_t *)malloc(barCodeWeight * sizeof(uint8_t) + 1);
    if (pWeight == NULL) // 如果请求不了内存
        return -1;

    // 读取条形码中间一行像素并返回
    int ret;
    ret = GenerateMiddleYData(barCodeImg, pWeight);
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

    // 创建条形码图片存在信息的像素区间(起始和结束)
    int barCodeStartPx = 0;
    int barCodeEndPx = 0;

    // 找到开始位置和结束位置
    if (0 == ret)
    {
        ret = FindBarCodeStart_EndPxInArray(pWeight, &barCodeStartPx, &barCodeEndPx);
    }
    // printf("%d %d\n", barCodeStartPx, barCodeEndPx);
    // printf("%d %d\n", *(pWeight + barCodeStartPx), *(pWeight + barCodeEndPx));

    // 条形码开始到结尾的长度
    int barCodeLen = barCodeEndPx - barCodeStartPx + 1;
    // printf("%d\n", barCodeLen);
    // 条形码有效信息间隔
    float payLoadInterval = barCodeLen / 95.0;
    // printf("%f\n", payLoadInterval);

    // 开始对指定开始和结尾的二维码数组进行解码
    // 把条形码每个存在信息的数据填入pCodeInfo中
    uint8_t *pCodeInfo = (uint8_t *)malloc(95 * sizeof(uint8_t) + 1);
    if (pCodeInfo == NULL) // 如果请求不了内存
        return -1;
    float temp;
    // 结尾符号
    *(pCodeInfo + 95) = 99;
    for (int i = 0; i < 95; i++)
    {
        temp = i * payLoadInterval;
        if (temp - (int)temp < 0.5) // 当间隔小数点后比5小舍去
        {
            // printf("%f\n", temp - (int)temp);
            *(pCodeInfo + i) = *(pWeight + barCodeStartPx + (int)temp);
        }
        else // 四舍五入
        {
            // printf("%f\n", temp - (int)temp);
            *(pCodeInfo + i) = *(pWeight + barCodeStartPx + (int)temp + 1);
        }
    }
    // unitTest 检验返回的内存是否正确
    // if (0 == ret)
    // {
    //     int i = 0;
    //     while (*(pCodeInfo + i) != 99)
    //     {
    //         // 这里数组和指针找数据都可以混用
    //         // printf("%d\n", *(pCodeInfo + i));
    //         printf("%d\n", pCodeInfo[i]);
    //         i++;
    //     }
    //     printf("%d", i);
    // }

    // 黑色从0变成结果1/白色从255变成0
    for (int i = 0; i < 95; i++)
    {
        if (*(pCodeInfo + i) == 255)
            *(pCodeInfo + i) = 0;
        else
            *(pCodeInfo + i) = 1;
    }

    // CodeInfo 2 barCodeNumber
    uint8_t barCodeNumber[14] = {0};
    codeInfo2BarCodeNumber(pCodeInfo, barCodeNumber);
    // unitTest 每个编码是否正确
    // for (int i = 0; i < 13; i++)
    // {
    //     printf("%d\n",barCodeNumber[i]);
    // }

    // 把条形码数组转为字符串并UI显示
    for (int i = 0; i < 13; i++)
    {
        barCodeNumStr[i] = barCodeNumber[i] + 48;
    }

    // 释放条形码宽度指针
    free(pWeight);
    pWeight = NULL;

    // 释放条形码信息指针
    free(pCodeInfo);
    pCodeInfo = NULL;

    return 0;
}
