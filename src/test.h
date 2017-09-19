#include <QtTest/QtTest>

class Test: public QObject
{
    Q_OBJECT
private slots:
    void fileIndex();
    void filePrefix();
};
