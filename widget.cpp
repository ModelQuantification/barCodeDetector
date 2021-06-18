#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include "widget.h"
#include "./ui_widget.h"
#include "QFileDialog"
#include "Python.h"
#include "string.h"
#include "./User/ocr.h"
#include "./User/ocv.h"
#include "./User/cam.h"

using namespace cv;
using namespace std;

#define NUM_THREADS 5

// 相机状态
int CameraStatus = 0;
// 图片全局路径
string imgFileDir;
// 视频帧
Mat cameraFrame;

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

// void *tFunc(void *img)
// {
//     Mat barCodeImg, barCodeMaskImg, framedBarCodeImg;
//     char barCodeNumStr[14] = {0};
//     Mat getFrame = *(Mat *)img;
//     // 经过探测得到的条形码蒙板图片
//     barCodeMaskImg = DetectBarCodeInImage(getFrame);

//     // 给条形码画框的原始图片
//     framedBarCodeImg = DrawFrame4BarCode(getFrame, barCodeMaskImg);

//     // 裁减条形码图片
//     barCodeImg = cropFrame4BarCode(getFrame, barCodeMaskImg);

//     // 使用方法一检测
//     // if (0 == methodFlag)
//     // {
//     methodLeaping2DetectBarCodeImg(barCodeImg, barCodeNumStr);
//     // }

//     // 返回条形码
//     return barCodeNumStr;
// }

void Widget::on_openCamera_clicked()
{
    // 打开摄像头
    // VideoCapture capture(0);

    // 打开文件
    VideoCapture capture("./img/barCode.mp4");

    if (!capture.isOpened())
    {
        std::cout << "摄像头已经被其他软件使用";
    }
    CameraStatus = 1;

    // 检测方法选择
    int methodFlag = ui->comboBox->currentIndex();
    // printf("使用方法: %d\n", methodFlag);

    int ret;
    Mat cvTempImg;
    QImage qtShowImg;

    // 经过裁减的条形码图片、条形码蒙板图片、画框后的条形码图片
    Mat barCodeImg, barCodeMaskImg, framedBarCodeImg;

    // 条形码字符串
    char barCodeNumStr[14] = {0};

    // 创建多个线程
    // pthread_t threads[NUM_THREADS];
    // 创建线程完成标志位，如果为1代表线程正在运行，如果为0代表现存并没有运行
    // int threadFinishFlag[NUM_THREADS] = {0};
    // 线程状态标志位
    // int threadStatusFlag[NUM_THREADS] = {0};
    // 控制的是第几个线程
    // int threadTag = 0;

    while (CameraStatus)
    {
        // cameraFrame就是一个图片
        capture >> cameraFrame;

        // 自动缩减手机视频(4800w)到合适的尺寸680/460
        cv::resize(cameraFrame, cameraFrame, cv::Size(cameraFrame.cols / 3, cameraFrame.rows / 3));

        // 创建一个新的处理线程
        // threadStatusFlag[threadTag] = pthread_create(&threads[threadTag], NULL, tFunc, &cameraFrame);
        // if (threadStatusFlag[threadTag])
        // {
        //     // 出现错误
        //     printf("ERROR; return code from pthread_create() is %d\n", threadStatusFlag[threadTag]);
        //     exit(-1);
        // }

        // 准备下一个线程
        // threadTag++;
        // if (threadTag == NUM_THREADS)
        // {
        //     threadTag = 0;
        // }

        // unitTest 测试摄像头是否有问题
        // barCodeMaskImg = cameraFrame;

        // 经过探测得到的条形码蒙板图片
        ret = DetectBarCodeInImage4Video(cameraFrame, barCodeMaskImg);

        // 给条形码画框的原始图片
        ret = DrawFrame4BarCode4Video(cameraFrame, barCodeMaskImg, framedBarCodeImg);

        // 裁减条形码图片
        ret = cropFrame4BarCode4Video(cameraFrame, barCodeMaskImg, barCodeImg);

        // 使用方法一检测
        if (0 == methodFlag && barCodeImg.data)
        {
            methodLeaping2DetectBarCodeImg(barCodeImg, barCodeNumStr);
        }

        // printf("%s\n", barCodeNumStr);
        QString qstr = QString::fromStdString(barCodeNumStr); // 输出字符串
        ui->detect->clear();
        ui->detect->setText(qstr);

        // 在QT中显示效果
        cv::cvtColor(framedBarCodeImg, cvTempImg, COLOR_BGR2RGB);
        qtShowImg = QImage((const unsigned char *)(cvTempImg.data), cvTempImg.cols, cvTempImg.rows, cvTempImg.step, QImage::Format_RGB888);
        ui->imgFrame->clear();
        ui->imgFrame->setPixmap(QPixmap::fromImage(qtShowImg));
        ui->imgFrame->show();

        if (barCodeImg.data)
        {
            cv::cvtColor(barCodeImg, cvTempImg, COLOR_BGR2RGB);
            qtShowImg = QImage((const unsigned char *)(cvTempImg.data), cvTempImg.cols, cvTempImg.rows, cvTempImg.step, QImage::Format_RGB888);
            ui->cropImg->clear();
            ui->cropImg->setPixmap(QPixmap::fromImage(qtShowImg));
            ui->cropImg->show();
        }

        // ESC-如果删除这句会报错
        if (cv::waitKey(30) == 27)
            break;
    }
    // TODO 创建多个新的线程，每秒完成1次识别操作，把识别的结果显示到QT
    // 实现过程：传给线程一帧已裁减的图片，线程传13位数据给QT
    // 实现目标：把一秒分为5段，在每个0.2秒都完成一个检测操作，然后只要操作完成就显示结果
    // 如果一个线程有未识别的结果，就丢弃这个结果，线程中止
}

void Widget::on_detectImage_clicked()
{
    // 检测方法选择
    int methodFlag = ui->comboBox->currentIndex();
    // printf("使用方法: %d\n", methodFlag);

    QString imgName = QFileDialog::getOpenFileName(this, tr("open image file"), "./",
                                                   tr("Image files(*.png *.jpg);;All files (*.*)"));
    if (imgName.isEmpty())
        return;
    Mat originImg;

    imgFileDir = imgName.toStdString();
    originImg = imread(imgName.toStdString()); // 输入一个图片

    // 经过裁减的条形码图片、条形码蒙板图片、画框后的条形码图片
    Mat barCodeImg, barCodeMaskImg, framedBarCodeImg;

    // 经过探测得到的条形码蒙板图片
    barCodeMaskImg = DetectBarCodeInImage(originImg);

    // 给条形码画框的原始图片
    framedBarCodeImg = DrawFrame4BarCode(originImg, barCodeMaskImg);

    // 裁减条形码图片
    barCodeImg = cropFrame4BarCode(originImg, barCodeMaskImg);

    // unitTest 不框测试
    // barCodeImg = originImg;

    // 条形码字符串
    char barCodeNumStr[14] = {0};
    // 使用方法一检测
    if (0 == methodFlag)
    {
        methodLeaping2DetectBarCodeImg(barCodeImg, barCodeNumStr);
    }

    // printf("%s\n", barCodeNumStr);
    QString qstr = QString::fromStdString(barCodeNumStr); // 输出字符串
    ui->detect->clear();
    ui->detect->setText(qstr);

    // 在QT中显示效果
    Mat cvTempImg;
    QImage qtShowImg;
    cv::cvtColor(framedBarCodeImg, cvTempImg, COLOR_BGR2RGB);
    qtShowImg = QImage((const unsigned char *)(cvTempImg.data), cvTempImg.cols, cvTempImg.rows, cvTempImg.step, QImage::Format_RGB888);
    ui->imgFrame->clear();
    ui->imgFrame->setPixmap(QPixmap::fromImage(qtShowImg));
    ui->imgFrame->show();

    cv::cvtColor(barCodeImg, cvTempImg, COLOR_BGR2RGB);
    qtShowImg = QImage((const unsigned char *)(cvTempImg.data), cvTempImg.cols, cvTempImg.rows, cvTempImg.step, QImage::Format_RGB888);
    ui->cropImg->clear();
    ui->cropImg->setPixmap(QPixmap::fromImage(qtShowImg));
    ui->cropImg->show();

    // 当没有imshow时关闭waitKey
    waitKey();
}

void Widget::on_detectBarCode_clicked()
{
    // cout << imgFileDir << std::endl;
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
    char *ptrImgFileDir = (char *)imgFileDir.data();
    PyObject *pArgs = PyTuple_New(1);
    PyObject *barCode_img_file_dir = Py_BuildValue("s", ptrImgFileDir);
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
    // 关闭摄像头
    CameraStatus = 0;

    // int threadStatusFlag[NUM_THREADS] = {1};
    // 关闭未执行的线程
    // for (int i = 0; i < NUM_THREADS; i++)
    // {
    //     // threadStatusFlag[i] = pthread_cancel(&threads[i]);
    // }

    // 清除UI框
    ui->imgFrame->clear();
    ui->cropImg->clear();
}

void Widget::on_comboBox_currentIndexChanged(int index)
{
    index = ui->comboBox->currentIndex();
}
