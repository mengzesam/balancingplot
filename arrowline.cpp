/****************************************************************************

****************************************************************************/


#include "arrowline.h"

#include <qmath.h>
#include <QPen>
#include <QPainter>
#include <QDebug>

ArrowLine::ArrowLine(const QPointF &start,const QPointF &end,const QString &name,
                     const QColor &color,const bool hiding,QGraphicsItem *parent)
    :QGraphicsLineItem(parent)
    ,m_Start(start)
    ,m_End(end)
    ,m_name(name.left(2))
    ,m_Color(color)
    ,m_hiding(hiding)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setPen(QPen(m_Color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setLine(QLineF(m_Start,m_End));
}

QRectF ArrowLine::boundingRect() const
{
    qreal extra =(pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
    //return QGraphicsLineItem::boundingRect();
}

QPainterPath ArrowLine::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(m_arrowHead);
    return path;
}

void ArrowLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{
    if (int(m_Start.x())==int(m_End.x())
            && int(m_Start.y())==int(m_End.y()))
        return;

    qreal arrowSize = 10;
    QPen mypen=pen();
    mypen.setWidth(1);
    mypen.setColor(m_Color);
    painter->setPen(mypen);
    painter->setBrush(m_Color);

    QTransform transf;
    //transf.translate(0,0);
    transf.rotate(rotation());
    transf.scale(scale(),scale());
    QLineF theline=transf.map(QLineF(mapFromParent(m_Start),mapFromParent(m_End)));
    painter->drawLine(theline);
    //setLine(theline);
    double angle = std::atan2(theline.dy(), -theline.dx());
    if(!m_hiding){
        QPointF arrowP1 =theline.p2() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                        cos(angle + M_PI / 3) * arrowSize);
        QPointF arrowP2 =theline.p2() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                        cos(angle + M_PI - M_PI / 3) * arrowSize);
        m_arrowHead.clear();
        m_arrowHead << theline.p2() << arrowP1 << arrowP2;
        painter->drawPolygon(m_arrowHead);
    }
    painter->save();
    painter->translate(theline.p2().x()-10.0*cos(-angle),theline.p2().y()-10.0*sin(-angle));
    painter->rotate(-rotation());
    painter->drawText(QRect(-10,-5,20,10),Qt::AlignCenter,m_name);
    painter->restore();
    if (isSelected()) {
        painter->setPen(QPen(m_Color, 1, Qt::DashLine));
        QLineF myLine = theline;
        myLine.translate(4.0*sin(angle), 4.0*cos(angle));
        painter->drawLine(myLine);
        myLine.translate(-8.0*sin(angle), -8.0*cos(angle));
        painter->drawLine(myLine);
    }
}
