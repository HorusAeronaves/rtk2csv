#include <QtCharts>

#include <QChart>
#include <QChartView>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QVector3D>
#include <QXmlStreamReader>

#include "window.h"
#include "ui_window.h"

QString Window::gpxFormat = ".gpx";
QString Window::kmlFormat = ".kml";
QString Window::jpgFormat = ".JPG";

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::_MainWindow)
{
    ui->setupUi(this);
    ui->gpxkmlInput->setText(QDir::currentPath());
    ui->imgInput->setText(QDir::currentPath());

    connect(ui->gpxkmlInput, &QLineEdit::returnPressed, this, &Window::checkGpxkmlInput);
    connect(ui->imgInput, &QLineEdit::returnPressed, this, &Window::checkImgInput);

    connect(ui->gpxkmlButton, &QPushButton::clicked, this, &Window::gpxkmlClicked);
    connect(ui->imgButton, &QPushButton::clicked, this, &Window::imgClicked);
    connect(ui->convertButton, &QPushButton::clicked, this, &Window::convertClicked);

    _lineSeries = new QLineSeries();
    _scatterSeries = new QScatterSeries();
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(_lineSeries);
    chart->addSeries(_scatterSeries);
    chart->createDefaultAxes();
    chart->setTitle("Survey");
    ui->chart->setChart(chart);
    ui->chart->setRenderHint(QPainter::Antialiasing);
    _lineSeries->clear();
    _scatterSeries->clear();
}

bool Window::checkGpxkmlInput()
{
    QString gpxkmlPath = ui->gpxkmlInput->displayText();
    QFileInfo gpxkmlFile(gpxkmlPath);
    if (!gpxkmlFile.exists()) {
        QMessageBox::critical(this, tr("Error"), tr("{%1} don't exist !").arg(gpxkmlFile.absoluteFilePath()));
        return false;
    }

    if(!ui->gpxkmlInput->displayText().endsWith(gpxFormat) \
            && !ui->gpxkmlInput->displayText().endsWith(kmlFormat)) {
        QMessageBox::critical(this, tr("Error"), tr("Wrong format, only .gpx and .kml !"));
        return false;
    }
    return true;
}

bool Window::checkImgInput()
{
    QString imgPath = ui->imgInput->displayText();
    QFileInfo imgFile(imgPath);
    if (!imgFile.exists()) {
        QMessageBox::critical(this, tr("Error"), tr("{%1} don't exist !").arg(imgFile.absoluteFilePath()));
        return false;
    }

    if(!ui->imgInput->displayText().endsWith(jpgFormat)) {
        QMessageBox::critical(this, tr("Error"), tr("Wrong format, only .jpg !"));
        return false;
    }
    QString fileName = imgFile.fileName();
    _photoIndex = fileName.mid(fileName.lastIndexOf(".") - 4, 4).toInt();
    qDebug() << "fileName" << _photoIndex << fileName.mid(fileName.lastIndexOf(".") - 4, 4);
    return true;
}

void Window::updatePlot()
{
    qDebug() << "Updating Plot";
    QString gpxkmlPath = ui->gpxkmlInput->displayText();
    QFileInfo gpxkmlFile(gpxkmlPath);
    QFile* file = new QFile(gpxkmlFile.absoluteFilePath());
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this,tr("Load File Problem"),
        tr("Couldn't open {%1} to load settings for download").arg(gpxkmlFile.absoluteFilePath()),
        QMessageBox::Ok);
        return;
    }
    _listVec.clear();
    QXmlStreamReader xmlReader(file);
    //Parse the XML until we reach end of it
    while(!xmlReader.atEnd()) {
        if(xmlReader.tokenType() == QXmlStreamReader::StartDocument) {
            xmlReader.readNext();
            continue;
        }
        if (xmlReader.tokenType() == QXmlStreamReader::StartElement) {
            if (xmlReader.name() == "gpx") {
                qDebug() << xmlReader.attributes().value("creator").toString();
            }
            if (xmlReader.name() == "wpt") {
                QVector3D vec;
                vec.setY(xmlReader.attributes().value("lat").toDouble());
                vec.setX(xmlReader.attributes().value("lon").toDouble());

                while (!(xmlReader.tokenType() == QXmlStreamReader::EndElement && xmlReader.name() == "wpt"))
                {
                    if (xmlReader.name() == "ele") {
                        vec.setZ(xmlReader.readElementText().toDouble());
                    }
                    xmlReader.readNext();
                }
                qDebug() << vec;
                _listVec.append(vec);
                if(qAbs(vec.x()) < 10 && qAbs(vec.y()) < 10) {
                    _listVec.removeLast();
                }
            }
        }
        xmlReader.readNext();
    }
    // Remove base
    _listVec.removeLast();

    _lineSeries->clear();
    _scatterSeries->clear();
    QVector2D maxPoint(0, 0);
    QVector2D minPoint(0, 0);

    for(const auto vec : _listVec){
        _scatterSeries->append(vec.x(), vec.y());
        if(maxPoint.x() == 0){
                maxPoint = QVector2D(vec);
                minPoint = maxPoint;
        }
        if(vec.x() > maxPoint.x())
            maxPoint.setX(vec.x());
        if(vec.y() > maxPoint.y())
            maxPoint.setY(vec.y());
        if(vec.x() < minPoint.x())
            minPoint.setX(vec.x());
        if(vec.y() < minPoint.y())
            minPoint.setY(vec.y());
    }
    _lineSeries->append(_scatterSeries->points());
    qDebug() << _listVec;

    if(xmlReader.hasError()) {
        QMessageBox::critical(this, tr("{%1} Parse Error").arg(gpxkmlFile.absoluteFilePath()), xmlReader.errorString(), QMessageBox::Ok);
        return;
    }
    _scatterSeries->setMarkerSize(10);
    qDebug() << "serie" << _scatterSeries->points();
    qDebug() << "point" << maxPoint << minPoint;
    ui->chart->chart()->setTitle(QString("Survey (%0 points)").arg(_scatterSeries->points().size()));
    ui->chart->update();
    ui->chart->chart()->createDefaultAxes();
    ui->chart->chart()->axisY()->setRange(minPoint.y(), maxPoint.y());
    ui->chart->chart()->axisX()->setRange(minPoint.x(), maxPoint.x());
}

void Window::convertClicked()
{
    if(!checkGpxkmlInput() || !checkImgInput()) {
        return;
    }

    createCSV();
}

void Window::gpxkmlClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select a .gpx or .kml file"), QDir::currentPath(), QStringLiteral("*.gpx *.kml"));
    if (fileName.isNull()) {
        QMessageBox::critical(this, tr("Error"), tr("No file selected !"));
    } else {
        ui->gpxkmlInput->setText(fileName);
        updatePlot();
    }
}

void Window::imgClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select a .JPG file"), QDir::currentPath(), QStringLiteral("*.JPG"));
    if (fileName.isNull()) {
        QMessageBox::critical(this, tr("Error"), tr("No file selected !"));
    } else {
        ui->imgInput->setText(fileName);
    }
}

void Window::createCSV() {
    QStringList row;
    row << "img" << "lat" << "lon" << "alt";

    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save Log to file"), "rtk2csv_output.csv");
    QFile file(saveFileName);
    if (file.open(QIODevice::ReadWrite)) {
        file.resize(0);
        QTextStream stream(&file);
        for (int index = 0; index < 4; index++) {
            stream << row[index];
            if(index == 3) {
                stream << "\n";
            } else {
                stream << ",";
            }
        }
        for (const auto vec : _listVec) {
            stream << "img_" + returnIndex() << "," << vec.x() << "," << vec.y() << "," << vec.z() << "\n";
        }
    }
    file.close();
}

QString Window::returnIndex() {
    QString index = "%1";
    if(_photoIndex < 1e1) {
        index = "000" + index.arg(_photoIndex);
    } else if(_photoIndex < 1e2) {
        index = "00" + index.arg(_photoIndex);
    } else if(_photoIndex < 1e3) {
        index = "0" + index.arg(_photoIndex);
    }
    index = index.arg(_photoIndex);
    _photoIndex += 1;
    return index;
}

QString Window::returnNexIndex() {
    QString index = "{%1}";
    _photoIndex += 1;
    if(_photoIndex < 1e1) {
        return "000" + index.arg(_photoIndex);
    }
    if(_photoIndex < 1e2) {
        return "00" + index.arg(_photoIndex);
    }
    if(_photoIndex < 1e3) {
        return "0" + index.arg(_photoIndex);
    }
    return index.arg(_photoIndex);
}

Window::~Window()
{
    delete ui;
}
