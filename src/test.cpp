#include <QStringList>

#include "test.h"
#include "window.h"

void Test::fileIndex()
{

    QStringList input;
    QStringList output;
    Window window;

    auto add = [&](QString in, QString out){
        input << in;
        output << out;
    };

    add(window.extractFileIndex("IMG_7045"), "7045");
    add(window.extractFilePrefix("IMG_7045"), "IMG_");
    add(window.extractFileIndex("IMG_7045"), "7045");
    add(window.extractFilePrefix("IMG_7045"), "IMG_");
    add(window.extractFileSufix("IMG_07045_geotag"), "_geotag");
    add(window.extractFileIndex("IMG_5817"), "5817");
    add(window.extractFilePrefix("IMG_5817"), "IMG_");
    add(window.extractFileSufix("IMG_5817_geotag"), "_geotag");
    //IMG_8103_geotag

    //DJI
    add(window.extractFileIndex("DJI_0036"), "0036");
    add(window.extractFilePrefix("DJI_0036"), "DJI_");
    add(window.extractFileIndex("DJI_00002"), "00002");
    add(window.extractFilePrefix("DJI_00002"), "DJI_");
    add(window.extractFileIndex("DJI00001"), "00001");
    add(window.extractFilePrefix("DJI00001"), "DJI");

    //Sony
    add(window.extractFileIndex("DSC01645"), "01645");
    add(window.extractFilePrefix("DSC01645"), "DSC");
    add(window.extractFileIndex("DSC01645_geotag"), "01645");
    add(window.extractFilePrefix("DSC01645_geotag"), "DSC");
    add(window.extractFileSufix("DSC01645_geotag"), "_geotag");

    //Gopro
    add(window.extractFileIndex("G0025683"), "0025683");
    add(window.extractFilePrefix("G0025683"), "G");

    for(int i = 0; i < input.size(); i++) {
        QCOMPARE(input[i], output[i]);
    }
}

void Test::filePrefix()
{
}

QTEST_MAIN(Test)
