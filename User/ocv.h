#pragma once

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

using namespace cv;
using namespace std;

Mat DetectBarCodeInImage(Mat image);
Mat DrawFrame4BarCode(Mat image,Mat mask);
int GenerateMiddleYData(Mat image);
