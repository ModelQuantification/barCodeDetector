#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "pthread.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_openCamera_clicked();

    void on_detectImage_clicked();

    void on_detectBarCode_clicked();

    void on_closeCamera_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::Widget *ui;
    // pthread_t threads[];
};




#endif // WIDGET_H
