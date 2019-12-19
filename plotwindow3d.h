#ifndef PLOTWINDOW3D_H
#define PLOTWINDOW3D_H

#include <QDialog>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>

namespace Ui {
class PlotWindow3D;
}

class PlotWindow3D : public QDialog
{
    Q_OBJECT

public:
    explicit PlotWindow3D(QWidget *parent = nullptr);
    ~PlotWindow3D();

private:
    Ui::PlotWindow3D *ui;
//    void drawLine(const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity *_rootEntity);
//    void drawTriangles(const QPolygonF polygon, QColor color, Qt3DCore::QEntity *_rootEntity);
};

#endif // PLOTWINDOW3D_H
