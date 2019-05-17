#ifndef ANGLEDIMENSION_H
#define ANGLEDIMENSION_H

#include <QGraphicsPathItem>

QT_BEGIN_NAMESPACE
class QGraphicsPolygonItem;
class QGraphicsLineItem;
class QGraphicsScene;
class QRectF;
class QGraphicsSceneMouseEvent;
class QPainterPath;
QT_END_NAMESPACE


class AngleDimension: public QGraphicsPathItem
{
public:
    enum { Type = UserType + 2 };
    AngleDimension(const double angle,const QPointF &cross,const QPointF &start,
                   int cyclewise=1,QColor color=Qt::red,QGraphicsPathItem *parent = 0);
    AngleDimension(const QPointF &cross,const QPointF &start,const QPointF &end,
                   int cyclewise=1,QColor color=Qt::red,QGraphicsPathItem *parent = 0);
    void setColor(const QColor &color);
    void updatePoints(const double angle,const QPointF &cross,
                      const QPointF &start,int cyclewise);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0) override;

private:
    int m_cyclewise;//-1:counterclockwise,1:clockwise
    double m_angle;
    QPointF m_cross;
    QPointF m_start;
    QPointF m_end;
};






#endif // ANGLEDIMENSION_H
