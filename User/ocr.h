#pragma once

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include "ocv.h"

using namespace cv;
using namespace std;

int methodLeaping2DetectBarCodeImg(Mat barCodeImg, char *str);
