#pragma once

#include <QtCharts>

#include <QLineSeries>
#include <QScatterSeries>
#include <QMainWindow>
#include <QWidget>

# include "qstring3d.h"

namespace Ui
{
class _MainWindow;
}

class Window : public QMainWindow
{

public:
    Window(QWidget* parent = nullptr);
    ~Window();

    bool checkGpxkmlInput();
    bool checkImgInput();
    void updatePlot();
    void convertClicked();
    void gpxkmlClicked();
    void imgClicked();
    void createCSV();
    QString returnIndex();
    QString returnNexIndex();

private:
    Ui::_MainWindow *ui;
    static QString gpxFormat;
    static QString jpgFormat;
    QList<QString3D> _listVec;
    uint _photoIndex;
    QLineSeries *_lineSeries;
    QScatterSeries *_scatterSeries;
    QString d2s(double d);
};
