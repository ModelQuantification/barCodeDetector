#include "imque.h"

IMGEQue::IMGEQue()
{
}
IMGEQue::~IMGEQue(void)
{
}
void IMGEQue::AddIMGE(const cv::Mat &frmIMGE)
{
    try
    {
        m_IMGEVector.push_back(frmIMGE);
    }
    catch (...)
    {
    }
}
bool IMGEQue::GetIMGE(cv::Mat &frmIMGE)
{
    try
    {
        if (m_IMGEVector.size() > 0)
        {
            std::list<cv::Mat>::iterator it = m_IMGEVector.begin();
            frmIMGE = (*it);
            m_IMGEVector.erase(it);
            return true;
        }
    }
    catch (...)
    {
        return false;
    }
    return false;
}
void IMGEQue::ClearQue()
{
    m_IMGEVector.clear();
}
