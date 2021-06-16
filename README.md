# barCodeDetector  
机器视觉大作业，菜鸟驿站条形码识别实现  
![Build](https://github.com/WangShuoran/barCodeDetector/actions/workflows/cmake.yml/badge.svg) ![ScanCode](https://github.com/WangShuoran/barCodeDetector/actions/workflows/codeql-analysis.yml/badge.svg)  

# 实现方式  
本项目使用多种实现方式完成条形码提取、检测、解码  
通过QT实现GUI显示  
![IndexScreenshot](img/index.png)  

# 环境搭建  
本项目基于OpenCV4.5+和QT5.12+完成基础环境配置  
如果QT是以安装包形式安装，需要在项目根目录CMakeLists.txt中指定QT的CMAKE_PREFIX_PATH  
请替换`/home/hi/Qt5.12.10/5.12.10/gcc_64`为你自己机器的QT安装位置  
```cmake  
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}  
                      ${GITHUB_WORKSPACE}/qtApp/Qt/5.12.10/5.12.10/gcc_64  
                      /home/hi/Qt5.12.10/5.12.10/gcc_64)  
```  
如果OpenCV安装位置没有设置为环境变量，需要自行配置OpenCV的一些路径为环境变量，否则需要修改CMakeLists.txt完成编译  

# 使用方式  
## 按钮一：打开摄像头并检测  
打开摄像头，并一直检测条形码，如果有条形码则按EAN13编码方式解码并在本地识别结果框中显示  

## 按钮二：打开图片并检测  
打开图片，如果有条形码则按EAN13编码解码并在本地识别结果框中显示，如果此前已经打开摄像头，则会自动关闭  

## 按钮三：腾讯云识别一帧图片  
只是检测前面两个功能的结果是否正确，会调用腾讯云识别并解码刚刚的一帧或者刚才打开的那幅图片，在腾讯云识别结果框中显示  

## 按钮四：关闭摄像头  
如果之前打开摄像头，则关闭摄像头  
