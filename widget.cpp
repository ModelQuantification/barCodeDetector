#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include "widget.h"
#include "./ui_widget.h"
#include "QFileDialog"
#include "Python.h"
#include "string.h"
#include "./User/ocv.h"

using namespace cv;
using namespace std;

int CameraStatus = 0;
string img_file_dir;

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
    // 暂时不知道作用
    int WIDTH = 720;
    int HEIGHT = 480;
    int FPS = 30;

    VideoCapture capture(0);
    if (!capture.isOpened())
    {
        std::cout << "摄像头没连接";
    }

    CameraStatus = 1;
    Mat frame;
    Mat cvTempImg;
    QImage qtShowImg;
    while (CameraStatus)
    {
        capture >> frame;
        // 在QT中显示效果
        cv::cvtColor(frame, cvTempImg, COLOR_BGR2RGB);
        qtShowImg = QImage((const unsigned char *)(cvTempImg.data), cvTempImg.cols, cvTempImg.rows, cvTempImg.step, QImage::Format_RGB888);
        ui->imgFrame->clear();
        ui->imgFrame->setPixmap(QPixmap::fromImage(qtShowImg));
        ui->imgFrame->show();
        // ESC
        if (cv::waitKey(1) == 27)
            break;
        // frame就是一个图片
    }
}

void Widget::on_detectImage_clicked()
{
    QString imgName = QFileDialog::getOpenFileName(this, tr("open image file"), "./",
                                                   tr("Image files(*.png *.jpg);;All files (*.*)"));
    if (imgName.isEmpty())
        return;
    Mat originImg, barCodeMaskImg;
    Mat barCodeImg, framedBarCodeImg;

    img_file_dir = imgName.toStdString();
    originImg = imread(imgName.toStdString());

    barCodeMaskImg = DetectBarCodeInImage(originImg);

    // 可以修改函数代码为裁减或带框
    barCodeImg = DrawFrame4BarCode(originImg, barCodeMaskImg);

    // unitTest 不框测试
    // barCodeImg = originImg;

    // 读取条形码宽度，把图片的宽度作为请求的内存大小
    int barCodeWeight = barCodeImg.cols;
    uint8_t *pWeight = (uint8_t *)malloc(barCodeWeight * sizeof(uint8_t) + 1);
    if (pWeight == NULL) // 如果请求不了内存
        return;

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

    // 方法选择
    int methodFlag = ui->comboBox->currentIndex();
    // printf("使用方法: %d\n", methodFlag);

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
        return;
    float temp;
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
    char barCodeNumStr[14] = {0};
    for (int i = 0; i < 13; i++)
    {
        barCodeNumStr[i] = barCodeNumber[i] + 48;
    }
    // printf("%s\n", barCodeNumStr);
    QString qstr = QString::fromStdString(barCodeNumStr);
    ui->detect->clear();
    ui->detect->setText(qstr);

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

    // 释放条形码信息指针
    free(pCodeInfo);
    pCodeInfo = NULL;

    // 当没有imshow时关闭waitKey
    waitKey();
}

void Widget::on_detectBarCode_clicked()
{
    // cout << img_file_dir << std::endl;
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        cout << "Py_Initialize failed." << endl;
        return;
    }
    // 设置module位置
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");
    // 打开模块
    PyObject *pModule = PyImport_ImportModule("tencentsdk");
    if (pModule == NULL)
    {
        cout << "tencentsdk not found" << endl;
        return;
    }
    // 打开模块的方法
    PyObject *pFunDetectBarCode = PyObject_GetAttrString(pModule, "detectBarCode");
    if (!pFunDetectBarCode || !PyCallable_Check(pFunDetectBarCode))
    {
        cout << "not found function detectBarCode" << endl;
        return;
    }
    // 把参数转换为Python类型
    char *ptr_img_file_dir = (char *)img_file_dir.data();
    PyObject *pArgs = PyTuple_New(1);
    PyObject *barCode_img_file_dir = Py_BuildValue("s", ptr_img_file_dir);
    PyTuple_SetItem(pArgs, 0, barCode_img_file_dir);
    // 使用该方法(函数)并得到返回值
    PyObject *pyValue = PyEval_CallObject(pFunDetectBarCode, pArgs);
    char *barCodeNum;
    PyArg_Parse(pyValue, "s", &barCodeNum);
    Py_Finalize();
    // printf("返回值：%s\n", barCodeNum);
    QString qstr = QString::fromStdString(barCodeNum);
    ui->barCodeShow->clear();
    ui->barCodeShow->setText(qstr);
    ui->barCodeShow->setStyleSheet("color:green;");
}

void Widget::on_closeCamera_clicked()
{
    CameraStatus = 0;
    ui->imgFrame->clear();
}

void Widget::on_comboBox_currentIndexChanged(int index)
{
    index = ui->comboBox->currentIndex();
}
