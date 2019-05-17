#include "balancingplot.h"
#include "ui_balancingplot.h"
#include "balancingdelegate.h"
#include "arrowline.h"
#include "angledimension.h"
#include <QtWidgets>
#include <QDebug>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")

#endif

BalancingPlot::BalancingPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BalancingPlot)
    ,m_cyclewise(-1) //-1:counterclockwise,1:clockwise
    ,m_keyProbeAngle(0)
    ,m_XProbeAngle(45)
    ,m_YProbeAngle(135)
    ,m_BearingProbeAngle(0)
    ,m_XWeightAngle(0)
    ,m_YWeightAngle(0)
    ,m_BearingWeightAngle(0)
    ,m_rotatorQuality(0)
    ,m_rpm(0)
    ,m_weightRadius(0)
    ,m_weightQuality(0)
    ,m_unitCentrifugalForce(0)
    ,m_totalCentrifugalForce(0)
    ,m_forceRatio(0)
    ,m_XVib({100,90,200,20})
    ,m_YVib({100,90,110,20})
    ,m_BearingVib({60,50,155,20})
    ,m_originalSide(0)
    ,m_scene(nullptr)
    ,m_circle(nullptr)
    ,m_center(nullptr)
    ,m_rotateline(nullptr)
    ,m_keyPhiLine(nullptr)
    ,m_XVibLine(nullptr)
    ,m_YVibLine(nullptr)
    ,m_BearingVibLine(nullptr)
    ,m_XWeightLine(nullptr)
    ,m_YWeightLine(nullptr)
    ,m_BearingWeightLine(nullptr)
    ,m_XWdim(nullptr)
    ,m_YWdim(nullptr)
    ,m_BWdim(nullptr)
{
    setupUi();
    setupPlotTable();
    setUpGraphics();
    connect(ui->plotTable,&QTableWidget::cellChanged,this,&BalancingPlot::tableInputChange);
    setWindowTitle(tr("balancing wizard"));
    tableInputChange();
}

BalancingPlot::~BalancingPlot()
{
    delete ui;
}

void BalancingPlot::resizeEvent(QResizeEvent *event)
{
    int width=event->size().width();
    int table_width=width/3*0.92;
    ui->plotTable->setColumnWidth(0,table_width/3);
    ui->plotTable->setColumnWidth(1,table_width/6);
    ui->plotTable->setColumnWidth(2,table_width/6);
    ui->plotTable->setColumnWidth(3,table_width/6);
    ui->plotTable->setColumnWidth(4,table_width/6);
    resizeGraphics();
    QWidget::resizeEvent(event);
}

void BalancingPlot::resizeGraphics()
{
    int width=ui->graphicsView->width()*0.9;
    int height=ui->graphicsView->height()*0.9;
    m_scene->setSceneRect(-width/2.0,-height/2.0,width,height);
    qreal side=qMin(width,height);
    if(side<=1) side=100.0;
    qreal ratio=side/m_originalSide;
    if(qgraphicsitem_cast<QGraphicsEllipseItem*>(m_circle)){
        m_circle->setScale(ratio);
        m_keyPhiLine->setScale(ratio);
        m_XVibLine->setScale(ratio);
        m_YVibLine->setScale(ratio);
        m_BearingVibLine->setScale(ratio);
        m_XWeightLine->setScale(ratio);
        m_YWeightLine->setScale(ratio);
        m_BearingWeightLine->setScale(ratio);
        m_XWdim->setScale(ratio);
        m_YWdim->setScale(ratio);
        m_BWdim->setScale(ratio);
        m_rotateline->setScale(ratio);
    }
}

void BalancingPlot::setupUi()
{
    ui->setupUi(this);
    ui->horizontalLayout_3->setStretch(0,1);
    ui->horizontalLayout_3->setStretch(1,1);
    ui->horizontalLayout_3->setStretch(2,1);
    ui->horizontalLayout_3->setStretch(3,1);
    ui->horizontalLayout_3->setStretch(4,1);
}

void BalancingPlot::setupPlotTable()
{
    QStringList heads;
    heads<<"名称"<<"数据来源"<<"值/基频值"<<"角度位置°"<<"滞后角°";
    ui->plotTable->setColumnCount(PLOTTABLE_COLS);
    ui->plotTable->setRowCount(PLOTTABLE_ROWS);
    ui->plotTable->setSizeAdjustPolicy(QTableWidget::AdjustToContents);
    ui->plotTable->verticalHeader()->hide();
    for(int c=0;c<PLOTTABLE_COLS;c++){
        ui->plotTable->setHorizontalHeaderItem(c,
                                           new QTableWidgetItem(heads.at(c)));
    }
    QFont headFont=ui->plotTable->horizontalHeader()->font();
    headFont.setBold(true);
    ui->plotTable->horizontalHeader()->setFont(headFont);
    ui->plotTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
    for(int r=0;r<PLOTTABLE_ROWS;r++)
        for(int c=0;c<PLOTTABLE_COLS;c++){
            ui->plotTable->setItem(r,c,new QTableWidgetItem(""));
        }
    QStringList names;
    names<<tr("转向")<<tr("键相位置")
        <<tr("X向振动")<<tr("Y向振动")<<tr("瓦振")
        <<tr("按X向加重位置")<<tr("按Y向加重位置")
        <<tr("按瓦振加重位置")<<tr("转子质量kg")<<tr("转速rpm")
        <<tr("加重半径mm")<<tr("试加重质量g")<<tr("加重1g产生激振力kgf")
        <<tr("试加重产生激振力kgf")<<tr("激振力与转子重力比%");
    QStringList values;
    values<<tr("顺时针")<<tr("/")
         <<tr("100∠200°")<<tr("100∠100°")<<tr("50∠150°");
    for(int r=0;r<names.count();r++){
        ui->plotTable->item(r,0)->setData(Qt::EditRole,names.at(r));
        ui->plotTable->item(r,1)->setData(Qt::EditRole,tr("手动输入"));
        ui->plotTable->item(r,2)->setData(Qt::EditRole,tr("0"));
        ui->plotTable->item(r,3)->setData(Qt::EditRole,tr("/"));
        ui->plotTable->item(r,4)->setData(Qt::EditRole,tr("/"));
    }
    for(int r=0;r<values.count();r++){
        ui->plotTable->item(r,2)->setData(Qt::EditRole,values.at(r));
    }
    ui->plotTable->item(1,3)->setData(Qt::EditRole,tr("0"));
    ui->plotTable->item(2,3)->setData(Qt::EditRole,tr("45"));
    ui->plotTable->item(2,4)->setData(Qt::EditRole,tr("20"));
    ui->plotTable->item(3,3)->setData(Qt::EditRole,tr("135"));
    ui->plotTable->item(3,4)->setData(Qt::EditRole,tr("20"));
    ui->plotTable->item(4,3)->setData(Qt::EditRole,tr("90"));
    ui->plotTable->item(4,4)->setData(Qt::EditRole,tr("20"));
    ui->plotTable->item(5,1)->setData(Qt::EditRole,tr("计算输出"));
    ui->plotTable->item(5,2)->setData(Qt::EditRole,tr("/"));
    ui->plotTable->item(5,3)->setData(Qt::EditRole,tr("0"));
    ui->plotTable->item(6,1)->setData(Qt::EditRole,tr("计算输出"));
    ui->plotTable->item(6,2)->setData(Qt::EditRole,tr("/"));
    ui->plotTable->item(6,3)->setData(Qt::EditRole,tr("0"));
    ui->plotTable->item(7,1)->setData(Qt::EditRole,tr("计算输出"));
    ui->plotTable->item(7,2)->setData(Qt::EditRole,tr("/"));
    ui->plotTable->item(7,3)->setData(Qt::EditRole,tr("0"));
    ui->plotTable->item(12,1)->setData(Qt::EditRole,tr("计算输出"));
    ui->plotTable->item(13,1)->setData(Qt::EditRole,tr("计算输出"));
    ui->plotTable->item(14,1)->setData(Qt::EditRole,tr("计算输出"));
    for(int r=0;r<names.count();r++){
        QTableWidgetItem* item=ui->plotTable->item(r,0);
        item->setFlags((item->flags()) ^ (Qt::ItemIsEditable));
        bool isOut=false;
        if(ui->plotTable->item(r,1)->text()==tr("计算输出"))
            isOut=true;
        for(int c=2;c<5;c++){
            item=ui->plotTable->item(r,c);
            if(isOut || item->text()=="/")
                item->setFlags((item->flags()) ^ (Qt::ItemIsEditable));
        }
    }
    //ui->plotTable->item(4,4)->setFlags((item->flags()) | (Qt::ItemIsEditable));
    ui->plotTable->setItemDelegate(new BalancingDelegate());
}

void BalancingPlot::setUpGraphics()
{
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setGeometry(0,0,640,360);

    m_scene=new QGraphicsScene(this);
    ui->graphicsView->setScene(m_scene);
    int width=ui->graphicsView->width()*0.9;
    int height=ui->graphicsView->height()*0.9;
    m_scene->setSceneRect(-width/2.0,-height/2.0,width,height);
    m_scene->setBackgroundBrush(QBrush(QColor(Qt::gray)));
    m_originalSide=qMin(width,height);
    qreal diameter=m_originalSide*0.75;
    m_circle=new QGraphicsEllipseItem(0,0,diameter,diameter);
    m_circle->setPos(QPointF(-diameter/2.0,-diameter/2.0));
    QPen pen {QColor(Qt::blue)};
    pen.setWidth(2);
    m_circle->setPen(pen);
    m_scene->addItem(m_circle);
    m_circle->setTransformOriginPoint(diameter/2.0,diameter/2.0);
    m_center=new QGraphicsEllipseItem(0,0,6,6);
    m_center->setPos(QPointF(-3.0,-3.0));
    QBrush brush {QColor(Qt::red)};
    m_center->setBrush(brush);
    m_center->setPen(QPen(Qt::NoPen));
    m_scene->addItem(m_center);

    double angle=m_keyProbeAngle/180.0*M_PI;
    m_keyPhiLine=new ArrowLine(QPointF(0,0),
                               QPointF(diameter/2.0*cos(angle),
                               -diameter/2.0*sin(angle)),
                               tr("Φ"),QColor(Qt::darkBlue),true);
    QPen mypen=m_keyPhiLine->pen();
    mypen.setStyle(Qt::DashLine);
    m_keyPhiLine->setPen(mypen);
    m_scene->addItem(m_keyPhiLine);
    QList<QPointF> dimStarts;
    dimStarts<<QPointF(diameter/8*cos(angle),-diameter/8*sin(angle))
         <<QPointF(diameter/4*cos(angle),-diameter/4*sin(angle))
         <<QPointF(diameter/8*3*cos(angle),-diameter/8*3*sin(angle));

    angle=(m_XProbeAngle+m_cyclewise*m_XVib.basePhase)/180.0*M_PI;
    m_XVibLine=new ArrowLine(QPointF(0,0),
                   QPointF(diameter/2.0*cos(angle),
                   -diameter/2.0*sin(angle)),
                   "AX",Qt::red);
    m_scene->addItem(m_XVibLine);
    double angle_wt=(angle-m_cyclewise*(m_XVib.lagAngle/180.0*M_PI))+M_PI;
    m_XWeightLine=new ArrowLine(QPointF(0,0),
                                QPointF(diameter/2.0*cos(angle_wt),
                                -diameter/2.0*sin(angle_wt)),
                                "QX",Qt::black);
    m_scene->addItem(m_XWeightLine);
    qreal angle2key=m_XWeightLine->line().angleTo(m_keyPhiLine->line());
    angle2key=fmod(-m_cyclewise*angle2key+720,360.0);
    m_XWdim=new AngleDimension(angle2key,QPointF(0,0),dimStarts.at(0),-m_cyclewise,Qt::black);
    m_scene->addItem(m_XWdim);


    angle=(m_YProbeAngle+m_cyclewise*m_YVib.basePhase)/180.0*M_PI;
    m_YVibLine=new ArrowLine(QPointF(0,0),
                   QPointF(diameter/2.0*cos(angle),
                   -diameter/2.0*sin(angle)),
                   "AY",Qt::yellow);
    m_scene->addItem(m_YVibLine);
    angle_wt=(angle-m_cyclewise*(m_YVib.lagAngle/180.0*M_PI))+M_PI;
    m_YWeightLine=new ArrowLine(QPointF(0,0),
                                QPointF(diameter/2.0*cos(angle_wt),
                                -diameter/2.0*sin(angle_wt)),
                                "QY",Qt::yellow);
    m_scene->addItem(m_YWeightLine);
    angle2key=m_YWeightLine->line().angleTo(m_keyPhiLine->line());
    angle2key=fmod(-m_cyclewise*angle2key+720,360.0);
    m_YWdim=new AngleDimension(angle2key,QPointF(0,0),dimStarts.at(1),-m_cyclewise,Qt::yellow);
    m_scene->addItem(m_YWdim);

    angle=(m_BearingProbeAngle+m_cyclewise*m_BearingVib.basePhase)/180.0*M_PI;
    m_BearingVibLine=new ArrowLine(QPointF(0,0),
                   QPointF(diameter/2.0*cos(angle),
                   -diameter/2.0*sin(angle)),
                   "AV",Qt::green);
    m_scene->addItem(m_BearingVibLine);
    angle_wt=(angle-m_cyclewise*(m_BearingVib.lagAngle/180.0*M_PI))+M_PI;
    m_BearingWeightLine=new ArrowLine(QPointF(0,0),
                                QPointF(diameter/2.0*cos(angle_wt),
                                -diameter/2.0*sin(angle_wt)),
                                "QV",Qt::green);
    m_scene->addItem(m_BearingWeightLine);
    angle2key=m_BearingWeightLine->line().angleTo(m_keyPhiLine->line());
    angle2key=fmod(-m_cyclewise*angle2key+720,360.0);
    m_BWdim=new AngleDimension(angle2key,QPointF(0,0),dimStarts.at(2),-m_cyclewise,Qt::green);
    m_scene->addItem(m_BWdim);
    updateRotatoLine();
}

void BalancingPlot::tableInputChange()
{
    updateInputs();
    calcBalancing();
    updateTable();
    updateGraphicsPos();
    m_scene->update();
}

void BalancingPlot::updateInputs()
{
    QString itemstring=ui->plotTable->item(0,2)->data(Qt::DisplayRole).toString();
    if(itemstring==tr("顺时针"))
       m_cyclewise=1;
    else {
       m_cyclewise=-1;
    }
    m_keyProbeAngle=ui->plotTable->item(1,3)->data(Qt::DisplayRole).toDouble();
    m_XProbeAngle=ui->plotTable->item(2,3)->data(Qt::DisplayRole).toDouble();
    m_YProbeAngle=ui->plotTable->item(3,3)->data(Qt::DisplayRole).toDouble();
    m_BearingProbeAngle=ui->plotTable->item(4,3)->data(Qt::DisplayRole).toDouble();
    m_XVib.lagAngle=ui->plotTable->item(2,4)->data(Qt::DisplayRole).toDouble();
    m_YVib.lagAngle=ui->plotTable->item(3,4)->data(Qt::DisplayRole).toDouble();
    m_BearingVib.lagAngle=ui->plotTable->item(4,4)->data(Qt::DisplayRole).toDouble();
    itemstring=ui->plotTable->item(2,2)->data(Qt::DisplayRole).toString();
    QStringList vals=itemstring.split(tr("∠"));
    if(vals.count()==2){
        m_XVib.baseAmpl=vals.at(0).toDouble();
        m_XVib.basePhase=((QString)vals.at(1)).replace(tr("°"),"").toDouble();
    }
    itemstring=ui->plotTable->item(3,2)->data(Qt::DisplayRole).toString();
    vals=itemstring.split(tr("∠"));
    if(vals.count()==2){
        m_YVib.baseAmpl=vals.at(0).toDouble();
        m_YVib.basePhase=((QString)vals.at(1)).replace(tr("°"),"").toDouble();
    }
    itemstring=ui->plotTable->item(4,2)->data(Qt::DisplayRole).toString();
    vals=itemstring.split(tr("∠"));
    if(vals.count()==2){
        m_BearingVib.baseAmpl=vals.at(0).toDouble();
        m_BearingVib.basePhase=((QString)vals.at(1)).replace(tr("°"),"").toDouble();
    }
    m_rotatorQuality=ui->plotTable->item(8,2)->data(Qt::DisplayRole).toDouble();
    m_rpm=ui->plotTable->item(9,2)->data(Qt::DisplayRole).toDouble();
    m_weightRadius=ui->plotTable->item(10,2)->data(Qt::DisplayRole).toDouble();
    m_weightQuality=ui->plotTable->item(11,2)->data(Qt::DisplayRole).toDouble();
}

void BalancingPlot::updateTable()
{
    ui->plotTable->item(5,3)->setData(Qt::EditRole,m_XWeightAngle);
    ui->plotTable->item(6,3)->setData(Qt::EditRole,m_YWeightAngle);
    ui->plotTable->item(7,3)->setData(Qt::EditRole,m_BearingWeightAngle);
    ui->plotTable->item(12,2)->setData(Qt::EditRole,m_unitCentrifugalForce);
    ui->plotTable->item(13,2)->setData(Qt::EditRole,m_totalCentrifugalForce);
    ui->plotTable->item(14,2)->setData(Qt::EditRole,m_forceRatio);
}

void BalancingPlot::updateRotatoLine(QColor color)
{
    if(!m_rotateline){
        m_rotateline=new QGraphicsPathItem();
        m_scene->addItem(m_rotateline);
    }
    QPainterPath path;
    qreal side=qMin(m_circle->boundingRect().width(),m_circle->boundingRect().height())*1.4;
    if(side<200) side=200;
    QRectF rect=QRectF(-side/2,-side/2,side,side);
    qreal angle=60.0;
    path.moveTo(side/2*cos(angle/180*M_PI),-side/2*sin(angle/180*M_PI));
    path.arcTo(rect,angle,2*(90-angle));
    QPointF arrow1;
    qreal angleArrow=angle/180*M_PI;
    if(m_cyclewise==1){
        arrow1=path.pointAtPercent(0);
        angleArrow=M_PI-angleArrow;
    }
    else
        arrow1=path.pointAtPercent(1);
    qreal arrowSize = 15;
    QPointF arrow0=arrow1+QPointF(arrowSize*cos(angleArrow),-arrowSize*sin(angleArrow));
    if(m_cyclewise==1)
        angleArrow+=M_PI/3.0;
    else
        angleArrow-=M_PI/3.0;
    QPointF arrow2=arrow1+QPointF(arrowSize*cos(angleArrow),-arrowSize*sin(angleArrow));
    path.addPolygon(QPolygonF()<<arrow0<<arrow1<<arrow2<<arrow0);
    m_rotateline->setPen(QPen(color, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    m_rotateline->setBrush(Qt::NoBrush);
    m_rotateline->setPath(path);
}

void BalancingPlot::updateGraphicsPos()
{

    qreal diameter=m_originalSide*0.75;

    double angle=m_keyProbeAngle/180.0*M_PI;
    if(m_keyPhiLine){
        m_keyPhiLine->updatePoints(QPointF(0,0),
                                   QPointF(diameter/2.0*cos(angle),
                                   -diameter/2.0*sin(angle)));
    }

    QList<QPointF> dimStarts;
    dimStarts<<QPointF(diameter/8*cos(angle),-diameter/8*sin(angle))
         <<QPointF(diameter/4*cos(angle),-diameter/4*sin(angle))
         <<QPointF(diameter/8*3*cos(angle),-diameter/8*3*sin(angle));

    if(m_XVibLine){
        angle=(m_XProbeAngle+m_cyclewise*m_XVib.basePhase)/180.0*M_PI;
        m_XVibLine->updatePoints(QPointF(0,0),
                       QPointF(diameter/2.0*cos(angle),
                       -diameter/2.0*sin(angle)));
    }

    double angle_wt=(angle-m_cyclewise*(m_XVib.lagAngle/180.0*M_PI))+M_PI;
    if(m_XWeightLine){
        m_XWeightLine->updatePoints(QPointF(0,0),
                                    QPointF(diameter/2.0*cos(angle_wt),
                                    -diameter/2.0*sin(angle_wt)));
    }

    qreal angle2key=m_XWeightLine->line().angleTo(m_keyPhiLine->line());
    if(m_XWdim){
        angle2key=fmod(-m_cyclewise*angle2key+720,360.0);
        m_XWdim->updatePoints(angle2key,QPointF(0,0),dimStarts.at(0),-m_cyclewise);
    }

    if(m_YVibLine){
        angle=(m_YProbeAngle+m_cyclewise*m_YVib.basePhase)/180.0*M_PI;
        m_YVibLine->updatePoints(QPointF(0,0),
                       QPointF(diameter/2.0*cos(angle),
                       -diameter/2.0*sin(angle)));
    }

    if(m_YWeightLine){
        angle_wt=(angle-m_cyclewise*(m_YVib.lagAngle/180.0*M_PI))+M_PI;
        m_YWeightLine->updatePoints(QPointF(0,0),
                                    QPointF(diameter/2.0*cos(angle_wt),
                                    -diameter/2.0*sin(angle_wt)));
    }

    if(m_YWdim){
        angle2key=m_YWeightLine->line().angleTo(m_keyPhiLine->line());
        angle2key=fmod(-m_cyclewise*angle2key+720,360.0);
        m_YWdim->updatePoints(angle2key,QPointF(0,0),dimStarts.at(1),-m_cyclewise);
    }

    if(m_BearingVibLine){
        angle=(m_BearingProbeAngle+m_cyclewise*m_BearingVib.basePhase)/180.0*M_PI;
        m_BearingVibLine->updatePoints(QPointF(0,0),
                       QPointF(diameter/2.0*cos(angle),
                       -diameter/2.0*sin(angle)));
    }
    if(m_BearingWeightLine){
        angle_wt=(angle-m_cyclewise*(m_BearingVib.lagAngle/180.0*M_PI))+M_PI;
        m_BearingWeightLine->updatePoints(QPointF(0,0),
                                    QPointF(diameter/2.0*cos(angle_wt),
                                    -diameter/2.0*sin(angle_wt)));
    }

    if(m_BWdim){
        angle2key=m_BearingWeightLine->line().angleTo(m_keyPhiLine->line());
        angle2key=fmod(-m_cyclewise*angle2key+720,360.0);
        m_BWdim->updatePoints(angle2key,QPointF(0,0),dimStarts.at(2),-m_cyclewise);
    }

    updateRotatoLine();
}

void BalancingPlot::calcBalancing()
{
    m_XWeightAngle=m_XProbeAngle+m_cyclewise*(m_XVib.basePhase-m_XVib.lagAngle)+180;
    m_XWeightAngle=m_cyclewise*(m_XWeightAngle-m_keyProbeAngle);
    m_XWeightAngle=m_XWeightAngle-int(m_XWeightAngle/360)*360;
    if(m_XWeightAngle<0)m_XWeightAngle+=360;

    m_YWeightAngle=m_YProbeAngle+m_cyclewise*(m_YVib.basePhase-m_YVib.lagAngle)+180;
    m_YWeightAngle=m_cyclewise*(m_YWeightAngle-m_keyProbeAngle);
    m_YWeightAngle=m_YWeightAngle-int(m_YWeightAngle/360)*360;
    if(m_YWeightAngle<0)m_YWeightAngle+=360;

    m_BearingWeightAngle=m_BearingProbeAngle+m_cyclewise*
            (m_BearingVib.basePhase-m_BearingVib.lagAngle)+180;
    m_BearingWeightAngle=m_cyclewise*(m_BearingWeightAngle-m_keyProbeAngle);
    m_BearingWeightAngle=m_BearingWeightAngle-int(m_BearingWeightAngle/360)*360;
    if(m_BearingWeightAngle<0)m_BearingWeightAngle+=360;

    m_unitCentrifugalForce=m_weightRadius/1000.0*
            (m_rpm/60.0*2*M_PI)*(m_rpm/60.0*2*M_PI)/(1000.0*G);
    m_totalCentrifugalForce=m_weightQuality*m_unitCentrifugalForce;
    if(fabs(m_rotatorQuality)>1E-6)
        m_forceRatio=m_totalCentrifugalForce/m_rotatorQuality*100.0;
    else {
        m_forceRatio=0;
    }
}

void BalancingPlot::on_AXcheckBox_clicked(bool checked)
{
    m_XVibLine->setVisible(checked);
    m_scene->update();
}

void BalancingPlot::on_QXcheckBox_clicked(bool checked)
{
    m_XWeightLine->setVisible(checked);
    m_XWdim->setVisible(checked);
    m_scene->update();
}

void BalancingPlot::on_AYcheckBox_clicked(bool checked)
{
    m_YVibLine->setVisible(checked);
    m_scene->update();
}

void BalancingPlot::on_QYcheckBox_clicked(bool checked)
{
    m_YWeightLine->setVisible(checked);
    m_YWdim->setVisible(checked);
    m_scene->update();
}

void BalancingPlot::on_AVcheckBox_clicked(bool checked)
{
    m_BearingVibLine->setVisible(checked);
    m_scene->update();
}

void BalancingPlot::on_QVcheckBox_clicked(bool checked)
{
    m_BearingWeightLine->setVisible(checked);
    m_BWdim->setVisible(checked);
    m_scene->update();
}

void BalancingPlot::on_AXQXcheckBox_clicked(bool checked)
{
    m_XVibLine->setVisible(checked);
    m_XWeightLine->setVisible(checked);
    m_XWdim->setVisible(checked);
    ui->AXcheckBox->setChecked(checked);
    ui->QXcheckBox->setChecked(checked);
    m_scene->update();
}

void BalancingPlot::on_AYQYcheckBox_clicked(bool checked)
{
     m_YVibLine->setVisible(checked);
     m_YWeightLine->setVisible(checked);
     m_YWdim->setVisible(checked);
     ui->AYcheckBox->setChecked(checked);
     ui->QYcheckBox->setChecked(checked);
     m_scene->update();
}

void BalancingPlot::on_AVQVcheckBox_clicked(bool checked)
{
    m_BearingVibLine->setVisible(checked);
    m_BearingWeightLine->setVisible(checked);
    m_BWdim->setVisible(checked);
    ui->AVcheckBox->setChecked(checked);
    ui->QVcheckBox->setChecked(checked);
    m_scene->update();
}

