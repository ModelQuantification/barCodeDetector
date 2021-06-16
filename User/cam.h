#pragma once

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include "ocv.h"

using namespace cv;
using namespace std;

int DetectBarCodeInImage4Video(Mat image, Mat &resultImage);
int DrawFrame4BarCode4Video(Mat image, Mat mask, Mat &resultImage);
int cropFrame4BarCode4Video(Mat image, Mat mask, Mat &resultImage);
