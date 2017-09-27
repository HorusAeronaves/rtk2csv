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

#include <cmath>

#include "window.h"
#include "ui_window.h"

QString Window::gpxFormat = ".gpx";
QString Window::jpgFormat = ".JPG";

Window::Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::_MainWindow),
    _photoIndex(0),
    _indexQuantity(6)
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

    if(!ui->gpxkmlInput->displayText().endsWith(gpxFormat)) {
        QMessageBox::critical(this, tr("Error"), tr("Wrong format, only .gpx !"));
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
        QString warningMsg;
        if (ui->checkGCP->isChecked()) {
            QMessageBox::warning(this, tr("Warning"),
            tr("No .jpg input, generating GCP file.\n To generate CSV please, deselect it."));
        } else {
            QMessageBox::warning(this, tr("Warning"),
            tr("No .jpg input, generating CSV file.\n To generate GCP please, select it."));
        }
        return true;
    }

    QString fileName = imgFile.fileName().split('.')[0];
    _indexQuantity = extractFileIndex(fileName).size();
    _photoIndex = extractFileIndex(fileName).toInt();
    _imagePrefix = extractFilePrefix(fileName);
    _imageSufix = extractFileSufix(fileName);

    QString indexMask = QString("0").repeated(_indexQuantity);
    _zeroIndexExist = imgFile.absoluteDir().exists(_imagePrefix + indexMask + jpgFormat);

    return true;
}

QString Window::extractFileIndex(QString fileName)
{
    QRegExp rx("[0-9]{4,10}");
    rx.indexIn(fileName);
    return rx.capturedTexts()[0];
}

QString Window::extractFilePrefix(QString fileName)
{
    QRegExp rx("^[A-z]{1,5}");
    rx.indexIn(fileName);
    return rx.capturedTexts()[0];
}

QString Window::extractFileSufix(QString fileName)
{
    QRegExp rx("[A-z]{1,9}$");
    rx.indexIn(fileName);
    return rx.capturedTexts()[0];
}

void Window::updatePlot()
{
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
                //qDebug() << xmlReader.attributes().value("creator").toString();
            }
            if (xmlReader.name() == "wpt") {
                QString3D vec;
                vec.y = xmlReader.attributes().value("lat").toString();
                vec.x = xmlReader.attributes().value("lon").toString();

                while (!(xmlReader.tokenType() == QXmlStreamReader::EndElement && xmlReader.name() == "wpt"))
                {
                    if (xmlReader.name() == "ele") {
                        vec.z = xmlReader.readElementText();
                    }
                    xmlReader.readNext();
                }
                _listVec.append(vec);
                if(qAbs(vec.x.toDouble()) < 10 && qAbs(vec.y.toDouble()) < 10) {
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

    for(auto vec : _listVec){
        _scatterSeries->append(vec.toQPointF());
        if(maxPoint.x() == 0){
                maxPoint = vec.toQVector2D();
                minPoint = maxPoint;
        }
        if(vec.x.toDouble() > maxPoint.x())
            maxPoint.setX(vec.x.toDouble());
        if(vec.y.toDouble() > maxPoint.y())
            maxPoint.setY(vec.y.toDouble());
        if(vec.x.toDouble() < minPoint.x())
            minPoint.setX(vec.x.toDouble());
        if(vec.y.toDouble() < minPoint.y())
            minPoint.setY(vec.y.toDouble());
    }
    _lineSeries->append(_scatterSeries->points());

    if(xmlReader.hasError()) {
        QMessageBox::critical(this, tr("{%1} Parse Error").arg(gpxkmlFile.absoluteFilePath()), xmlReader.errorString(), QMessageBox::Ok);
        return;
    }
    _scatterSeries->setMarkerSize(10);
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select a .gpx file"), QDir::currentPath(), QStringLiteral("*.gpx"));
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
    bool haveImage = ui->imgInput->displayText().endsWith(jpgFormat);
    QString saveFileName = QFileDialog::getSaveFileName(this, tr("Save Log to file"), "rtk2csv_output.csv", "Text Files (*.csv)");
    QFile file(saveFileName);
    file.link(saveFileName, ".csv");
    if (file.open(QIODevice::ReadWrite)) {
        file.resize(0);
        QTextStream stream(&file);
        for (auto vec : _listVec) {
            if (haveImage) {
                stream << _imagePrefix << returnIndex() << _imageSufix << ",";
            } else {
                if (ui->checkGCP->isChecked()) {
                    stream << "GCP_" << returnIndex() << ",";
                }
            }
            stream << vec.toCSV() << "\n";
        }
    }
    file.close();
}

QString Window::returnIndex() {
    QString index = QString("%1").arg(_photoIndex);
    QString indexMask = QString("0").repeated(_indexQuantity);
    index = indexMask.remove(indexMask.size() - index.size(), index.size()) + index;
    _photoIndex += 1;
    if(_photoIndex >= pow(10, _indexQuantity)) {
        _zeroIndexExist ? _photoIndex = 0 : _photoIndex = 1;
    }
    return index;
}

QString Window::d2s(double d) {
    return QString::number(d, 'f', 10);
}

Window::~Window()
{
    delete ui;
}
