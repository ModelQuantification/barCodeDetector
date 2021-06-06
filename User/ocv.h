#pragma once

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

using namespace cv;
using namespace std;

Mat DetectBarCodeInImage(Mat image);
Mat DrawFrame4BarCode(Mat image, Mat mask);
int GenerateMiddleYData(Mat image, uint8_t *ptrPx);
int FindBarCodeStart_EndPxInArray(uint8_t *array, int *barCodeStartPx, int *barCodeEndPx);
int codeInfo2BarCodeNumber(uint8_t *pCodeInfo, uint8_t *barCodeNumber);
