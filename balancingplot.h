#ifndef BALANCINGPLOT_H
#define BALANCINGPLOT_H

#include <QWidget>


QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsEllipseItem;
class ArrowLine;
class AngleDimension;
class QGraphicsLineItem;
class QGraphicsPathItem;
QT_END_NAMESPACE

namespace Ui {
class BalancingPlot;
}

class BalancingPlot : public QWidget
{
    Q_OBJECT

public:
    struct VibDataType{
        double vibVal;//um
        double baseAmpl;//um
        double basePhase;//°
        double lagAngle;//°
    };
    explicit BalancingPlot(QWidget *parent = nullptr);
    ~BalancingPlot();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void tableInputChange();
    void on_AXcheckBox_clicked(bool checked);
    void on_QXcheckBox_clicked(bool checked);
    void on_AYcheckBox_clicked(bool checked);
    void on_QYcheckBox_clicked(bool checked);
    void on_AVcheckBox_clicked(bool checked);
    void on_QVcheckBox_clicked(bool checked);
    void on_AXQXcheckBox_clicked(bool checked);
    void on_AYQYcheckBox_clicked(bool checked);
    void on_AVQVcheckBox_clicked(bool checked);

private:
    void setupUi();
    void setUpGraphics();
    void setupPlotTable();
    void calcBalancing();
    void plot();
    void resizeGraphics();
    void updateRotatoLine(QColor color=Qt::red);
    void updateGraphicsPos();
    void updateInputs();
    void updateTable();

private:
    Ui::BalancingPlot *ui;

    int m_cyclewise;//-1:counterclockwise,1:clockwise
    double m_keyProbeAngle;//°
    double m_XProbeAngle;//°
    double m_YProbeAngle;//°
    double m_BearingProbeAngle;//°
    double m_XWeightAngle;//°,相对于键相，逆转向为正
    double m_YWeightAngle;//°,相对于键相，逆转向为正
    double m_BearingWeightAngle;//°,相对于键相，逆转向为正
    double m_rotatorQuality;//kg
    double m_rpm;//r/min
    double m_weightRadius;//mm
    double m_weightQuality;//g
    double m_unitCentrifugalForce;//kgf/g
    double m_totalCentrifugalForce;//kgf
    double m_forceRatio;//%
    VibDataType m_XVib;
    VibDataType m_YVib;
    VibDataType m_BearingVib;

    double m_originalSide;
    QGraphicsScene* m_scene;
    QGraphicsEllipseItem* m_circle;
    QGraphicsEllipseItem* m_center;
    QGraphicsPathItem* m_rotateline;
    ArrowLine* m_keyPhiLine;
    ArrowLine* m_XVibLine;
    ArrowLine* m_YVibLine;
    ArrowLine* m_BearingVibLine;
    ArrowLine* m_XWeightLine;
    ArrowLine* m_YWeightLine;
    ArrowLine* m_BearingWeightLine;
    AngleDimension* m_XWdim;
    AngleDimension* m_YWdim;
    AngleDimension* m_BWdim;

private://const
    const static int PLOTTABLE_COLS=5;
    const static int PLOTTABLE_ROWS=15;
    constexpr static double G=9.8;
};




//const double BalancingPlot::G=9.8;

#endif // BALANCINGPLOT_H
