#ifndef BALANCINGDELEGATE_H
#define BALANCINGDELEGATE_H
//#include <QItemDelegate>
//#include <QStyledItemDelegate>
//#include <QWidget>
//#include <QLabel>
//#include <QLineEdit>
//#include <QSpinBox>
//#include <QDoubleSpinBox>
//#include <QComboBox>
//#include <QDateTimeEdit>
#include <QtWidgets>



class VectorInput:public QWidget
{
    Q_OBJECT

public:
    struct Vector{
        double imabs;
        double imargument;
    };
    explicit VectorInput(QWidget *parent = 0);
    QString text() const;
    void setData();
    void setText(const QString& intext);
    void setChildFocus();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    void setupUi();

private:
    Vector m_data;
    QDoubleSpinBox* DSBoxABS;
    QDoubleSpinBox* DSBoxArgument;
    QLabel* label;
    QLabel* label_2;
};





//class BalancingDelegate : public QItemDelegate
class BalancingDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    enum EditorType { LineEdit=0,SpinBox=1,DoubleSpinBox=2,SrcComboBox=3,
                      RotateComboBox=4,DateTimeEdit=5,ColorDialog=6,
                      Vectorinput=7,CheckBox=8,ColorSet=9
                    };
    Q_ENUM(EditorType)
    BalancingDelegate();
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

protected:
    bool eventFilter(QObject *editor, QEvent *event);

private slots:
    void commitAndCloseEditor();
    void onClickOfColorEditor();

signals:

private:
    enum EditorType evalEditorType(const QModelIndex & index) const;
    QColor m_color;

private://const
    const static int CHANNELS=32;

};







#endif // BALANCINGDELEGATE_H
