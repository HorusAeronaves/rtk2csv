#include <QPointF>
#include <QVector2D>
#include <QVector3D>
#include <QString>

#include "qstring3d.h"

QString3D::QString3D():
    x("0.0"),
    y("0.0"),
    z("0.0")
{
}

const QVector2D QString3D::toQVector2D()
{
    return QVector2D(x.toDouble(), y.toDouble());
}

const QVector3D QString3D::toQVector3D()
{
    return QVector3D(x.toDouble(), y.toDouble(), z.toDouble());
}

const QPointF QString3D::toQPointF()
{
    return toQVector2D().toPointF();
}

const QString QString3D::toCSV()
{
    return QString(x + "," + y + "," + z);
}

QString3D::operator QString() const
{
    return QStringLiteral("(x: %1, y: %2, z: %3)").arg(x, y, z);
}

QString3D::~QString3D()
{
}
