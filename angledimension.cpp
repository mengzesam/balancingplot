#include "angledimension.h"
#include <qmath.h>
#include <QPainter>
#include <QDebug>

AngleDimension::AngleDimension(const double angle, const QPointF &cross, const QPointF &start,
                               int cyclewise,QColor color, QGraphicsPathItem *parent)
    :QGraphicsPathItem(parent)
    ,m_angle(fmod(angle+720,360.0))
    ,m_cross(cross)
    ,m_start(start)
{
    m_cyclewise=cyclewise>0?1:-1;//-1:counterclockwise,1:clockwise
    QLineF tmpline=QLineF(cross,start);
    qreal startAngle=tmpline.angle();
    tmpline.setAngle(-cyclewise*angle+startAngle);
    m_end=tmpline.p2();
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(color, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    setBrush(Qt::NoBrush);
}

AngleDimension::AngleDimension(const QPointF &cross, const QPointF &start, const QPointF &end,
                               int cyclewise,QColor color, QGraphicsPathItem *parent)
    :QGraphicsPathItem(parent)
    ,m_cross(cross)
    ,m_start(start)
    ,m_end(end)
{
    m_cyclewise=cyclewise>0?1:-1;//-1:counterclockwise,1:clockwise
    QLineF tmpline=QLineF(cross,start);
    m_angle=QLineF(cross,end).angleTo(tmpline);
    if(cyclewise==1){
        m_angle=fmod(1080.0-m_angle,360.0);
    }
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(color, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
    setBrush(Qt::NoBrush);
}

void AngleDimension::updatePoints(const double angle, const QPointF &cross,
                                  const QPointF &start,int cyclewise)
{
    m_cyclewise=cyclewise>0?1:-1;//-1:counterclockwise,1:clockwise
    m_angle=angle;
    m_cross=cross;
    m_start=start;
    QLineF tmpline=QLineF(cross,start);
    qreal startAngle=tmpline.angle();
    tmpline.setAngle(-cyclewise*angle+startAngle);
    m_end=tmpline.p2();
    update();
}

void AngleDimension::setColor(const QColor &color)
{
    QPen mypen=pen();
    mypen.setColor(color);
    setPen(mypen);
}


void AngleDimension::paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    painter->setPen(pen());
    QTransform transf;
    transf.translate(0,0);
    transf.rotate(rotation());
    transf.scale(scale(),scale());
    QPointF cross=mapFromParent(m_cross);
    QPointF start=m_start;
    qreal startAngle=QLineF(m_cross,QPointF(m_cross.x()+10.0,m_cross.y()))
           .angleTo(QLineF(m_cross,m_start));
    qreal dx=cross.x()-start.x();
    qreal dy=cross.y()-start.y();
    qreal side=sqrt(dx*dx+dy*dy);
    QRectF rect=QRectF(cross.x()-side,cross.y()-side,2.0*side,2.0*side);    
    //painter->drawPolygon(QPolygonF(rect));
    painter->drawArc(rect,startAngle*16,-m_cyclewise*m_angle*16);
    //painter->drawLine(transf.map(QLineF(start,end)));
    qreal midAngle=(startAngle-m_cyclewise*m_angle/2)*M_PI/180.0;
    QPointF pt=mapFromParent(m_cross.x()+(side+15)*cos(midAngle),
                           m_cross.x()+-(side+15)*sin(midAngle));
    qreal xt,yt;
    transf.map(pt.x(),pt.y(),&xt,&yt);
    painter->save();
    painter->translate(xt,yt);
    painter->rotate(-rotation());
    QString ss=QObject::tr("%1°").arg(m_angle);
    painter->drawText(QRect(-20,-6,40,12),Qt::AlignCenter,ss);
    painter->restore();
}
