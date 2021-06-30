#pragma once

#include <list>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

using namespace cv;
using namespace std;

class IMGEQue
{
public:
    IMGEQue();
    ~IMGEQue(void);

    void AddIMGE(const cv::Mat &frmIMGE);
    bool GetIMGE(cv::Mat &frmIMGE);
    void ClearQue();

private:
    std::list<cv::Mat> m_IMGEVector;
};
