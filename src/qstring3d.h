#include <QDebug>
#include <QObject>

class QString3D
{
public:
    QString3D();
    ~QString3D();

    const QPointF toQPointF();
    const QVector2D toQVector2D();
    const QVector3D toQVector3D();
    const QString toCSV();
    operator QString() const;

    QString x, y, z;
};
