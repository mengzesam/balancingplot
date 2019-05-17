#include "balancingdelegate.h"
#include <QKeyEvent>
#include <QDebug>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")

#endif

BalancingDelegate::BalancingDelegate()
{

}

BalancingDelegate::EditorType BalancingDelegate::evalEditorType(const QModelIndex &index) const
{
    int r=index.row();
    int c=index.column();
    if(c==1){
        return SrcComboBox;
    }
    if(r==0 && c==2)
        return RotateComboBox;
    if(c==2 && r>=2 && r<5)
        return Vectorinput;
    if(c>=2)
        return DoubleSpinBox;
    //else
        return LineEdit;
}

QWidget *BalancingDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    EditorType editorType=evalEditorType(index);
    if(editorType==SpinBox){
        QSpinBox* editor=new QSpinBox(parent);
        editor->setMinimum(-720);
        editor->setMaximum(720);
        return editor;
    }
    if(editorType==DoubleSpinBox){
        QDoubleSpinBox* editor=new QDoubleSpinBox(parent);
        editor->setMaximum(0.0);
        editor->setMaximum(100000.0);
        return editor;
    }
    if(editorType==RotateComboBox){
        QComboBox* editor=new QComboBox(parent);
        QStringList items;
        items<<tr("顺时针")<<tr("逆时针");
        editor->addItems(items);
        return editor;
    }
    if(editorType==SrcComboBox){
        QComboBox* editor=new QComboBox(parent);
        QStringList items;
        items<<tr("手动输入")<<tr("计算输出");
        for(int i=0;i<CHANNELS;i++)
            items<<tr("通道%1").arg(i+1);
        editor->addItems(items);
        return editor;
    }
    if(editorType==CheckBox){
        QCheckBox* editor=new QCheckBox(parent);
        return editor;
    }
    if(editorType==Vectorinput){
        VectorInput* editor=new VectorInput(parent);
        return editor;
    }
    if(editorType==ColorSet){
        QPushButton* editor=new QPushButton(parent);
        editor->setText("");
        connect(editor, &QPushButton::clicked, this, &BalancingDelegate::onClickOfColorEditor);
        return editor;
    }
    if(editorType==DateTimeEdit){
        QDateTimeEdit *editor = new QDateTimeEdit(parent);
        editor->setDisplayFormat("dd/M/yyyy");
        editor->setCalendarPopup(true);
        return editor;
    }
    //else if(editorType==LineEdit){
        QLineEdit* editor=new QLineEdit(parent);
        return editor;
    //}
}

void BalancingDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if (edit) {
          edit->setText(index.model()->data(index, Qt::EditRole).toString());
          return;
    }
    QSpinBox* spinbox=qobject_cast<QSpinBox*>(editor);
    if(spinbox){
        spinbox->setValue(index.model()->data(index, Qt::EditRole).toInt());
        return;
    }
    QDoubleSpinBox* dspinbox=qobject_cast<QDoubleSpinBox*>(editor);
    if(dspinbox){
        dspinbox->setValue(index.model()->data(index, Qt::EditRole).toDouble());
        return;
    }
    QComboBox* combobox=qobject_cast<QComboBox*>(editor);
    if(combobox){
        combobox->setCurrentText(index.model()->data(index, Qt::EditRole).toString());
        return;
    }
    QCheckBox* checkboxEditor=qobject_cast<QCheckBox*>(editor);
    if(checkboxEditor){
        bool checked=index.model()->data(index,Qt::CheckStateRole).toBool();
        checkboxEditor->setChecked(checked);
        return;
    }
    VectorInput* vectorEditor= qobject_cast<VectorInput*>(editor);
    if(vectorEditor){
        vectorEditor->setText(index.model()->data(index, Qt::EditRole).toString());
        return;
    }
    QPushButton *buttonEditor = qobject_cast<QPushButton*>(editor);
    if(buttonEditor->text()==""){
        buttonEditor->click();
        if(!m_color.isValid()){
            QColor color=(QColor)index.model()->data(index,Qt::BackgroundRole).toString();
            buttonEditor->setStyleSheet(QString("QPushButton {background-color : %1;color : %2;}")
                               .arg(color.name()).arg(color.name()));
            buttonEditor->setText(color.name());
        }else{
            buttonEditor->setStyleSheet(QString("QPushButton {background-color : %1;color : %2;}")
                               .arg(m_color.name()).arg(m_color.name()));
            buttonEditor->setText(m_color.name());
        }
        qDebug()<<m_color.name()<<"color "<<buttonEditor->text();
        return;
    }
    QDateTimeEdit *dateEditor = qobject_cast<QDateTimeEdit *>(editor);
    if (dateEditor) {
          dateEditor->setDate(QDate::fromString(
                                  index.model()->data(index, Qt::EditRole).toString(),
                                  "d/M/yyyy"));
          return;
    }
}

void BalancingDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
    if(edit){
        model->setData(index, edit->text());
        return;
    }
    QSpinBox* spinbox=qobject_cast<QSpinBox*>(editor);
    if(spinbox){
        model->setData(index,spinbox->value());
        return;
    }
    QDoubleSpinBox* dspinbox=qobject_cast<QDoubleSpinBox*>(editor);
    if(dspinbox){
        model->setData(index,dspinbox->value());
        return;
    }
    QComboBox* combobox=qobject_cast<QComboBox*>(editor);
    if(combobox){
        model->setData(index,combobox->currentText());
        return;
    }
    QCheckBox* checkboxEditor=qobject_cast<QCheckBox*>(editor);
    if(checkboxEditor){
        model->setData(index,checkboxEditor->checkState(),
                              Qt::CheckStateRole );
        return;
    }
    VectorInput* vectorEditor= qobject_cast<VectorInput*>(editor);
    if(vectorEditor){
        model->setData(index,vectorEditor->text());
        return;
    }
    QPushButton *buttonEditor = qobject_cast<QPushButton*>(editor);
    if(buttonEditor){
        QString value=buttonEditor->text();
        model->setData(index, QVariant(QColor(value)), Qt::BackgroundRole);
    }
    QDateTimeEdit* dateEditor = qobject_cast<QDateTimeEdit*>(editor);
    if (dateEditor){
        model->setData(index, dateEditor->date().toString("dd/M/yyyy"));
        return;
    }
}

void BalancingDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    EditorType editorType=evalEditorType(index);
    VectorInput* vectorEditor=qobject_cast<VectorInput*>(editor);
    if(editorType==Vectorinput){
        QRect rect=option.rect;
        QRect newRect=QRect(rect.x(),rect.y()-15,151, 45);
        editor->setGeometry(newRect);
        vectorEditor->setChildFocus();
        return;
    }
    QCheckBox* checkboxEditor=qobject_cast<QCheckBox*>(editor);
    if(checkboxEditor){
        QRect rect=option.rect;
        QRect newRect=QRect(rect.x()+2,rect.y(),rect.width(), rect.height());
        editor->setGeometry(newRect);
        return;
    }
    QComboBox* comboboxEditor=qobject_cast<QComboBox*>(editor);
    if(comboboxEditor){
        QRect rect=option.rect;
        QRect newRect=QRect(rect.x(),rect.y(),rect.width()+15, rect.height());
        editor->setGeometry(newRect);
        return;
    }
    //else
       editor->setGeometry(option.rect);
       // QStyledItemDelegate::updateEditorGeometry(editor,option,index);
}

bool BalancingDelegate::eventFilter(QObject *editor, QEvent *event)
{
    return QStyledItemDelegate::eventFilter(editor,event);
//    QKeyEvent* keyevent=dynamic_cast<QKeyEvent*>(event);
//    //if(event->type()==QEvent::KeyPress){
//    if(keyevent){
//        VectorInput* vectorEditor=qobject_cast<VectorInput*>(editor);
//        if(keyevent->key()==Qt::Key_Tab && vectorEditor){
//            qDebug()<<"press key_tab\n";
//            return true;
//        }
//        else{
//            return QStyledItemDelegate::eventFilter(editor,event);
//        }
//    }else{
//        return QStyledItemDelegate::eventFilter(editor,event);
//    }
}


void BalancingDelegate::commitAndCloseEditor()
{

}

void BalancingDelegate::onClickOfColorEditor()
{
    QPushButton* buttonEditor=qobject_cast<QPushButton *>(sender());
    if(buttonEditor){
         m_color=QColorDialog::getColor(Qt::yellow,0,"select color",
                                             QColorDialog::DontUseNativeDialog);
    }
}


//******************VectorInput class implement
VectorInput::VectorInput(QWidget *parent)
    :QWidget(parent)
    ,DSBoxABS(0)
    ,DSBoxArgument(0)
    ,label(0)
    ,label_2(0)
{
    m_data.imabs=0.0;
    m_data.imargument=0.0;
    setupUi();
    setChildFocus();
    DSBoxABS->installEventFilter(this);
    DSBoxArgument->installEventFilter(this);
}

void VectorInput::setupUi()
{
      //setGeometry(QRect(240, 170, 151, 35));
      DSBoxABS = new QDoubleSpinBox(this);
      DSBoxABS->setGeometry(QRect(0, 20, 62, 22));
      DSBoxABS->setDecimals(1);
      DSBoxABS->setMinimum(-2000);
      DSBoxABS->setMaximum(2000);
      DSBoxABS->setSingleStep(5);
      DSBoxArgument = new QDoubleSpinBox(this);
      DSBoxArgument->setGeometry(QRect(80, 20, 62, 22));
      DSBoxArgument->setDecimals(1);
      DSBoxArgument->setMaximum(360);
      DSBoxArgument->setSingleStep(5);
      label = new QLabel(this);
      label->setGeometry(QRect(140, 20, 21, 16));
      QFont font;
      font.setPointSize(12);
      font.setBold(true);
      font.setWeight(75);
      label->setFont(font);
      label_2 = new QLabel(this);
      label_2->setGeometry(QRect(60, 20, 21, 16));
      label_2->setFont(font);
      //setWindowTitle(tr("Vetor Input"));
      label->setText(tr("\302\260"));
      label_2->setText(tr("\342\210\240"));
}

void VectorInput::setChildFocus()
{
    setFocusPolicy(Qt::StrongFocus);
    setTabOrder(DSBoxABS,DSBoxArgument);
    //setTabOrder(DSBoxArgument,DSBoxABS);
    setFocusProxy(DSBoxABS);
}

QString VectorInput::text() const
{
   QString ret=tr("%1∠%2°")
           .arg(DSBoxABS->value())
           .arg(DSBoxArgument->value());
   return ret;
}

void VectorInput::setData()
{
    ;
}

void VectorInput::setText(const QString& intext)
{
    QString innew=intext;
    innew.replace("@","∠");
    innew.replace("°","");
    QStringList fields=innew.split("∠");
    if(fields.count()==2){
        DSBoxABS->setValue(fields.at(0).toDouble());
        DSBoxArgument->setValue(fields.at(1).toDouble());
    }else{
        DSBoxABS->setValue(1.0);
        DSBoxArgument->setValue(90.0);
    }
}

bool VectorInput::eventFilter(QObject *watched, QEvent *event)
{
    if((event->type() == QEvent::KeyPress)
            && (DSBoxABS==watched || DSBoxArgument==watched)){
        QKeyEvent* keyevent=dynamic_cast<QKeyEvent*>(event);
        if(keyevent->key()==Qt::Key_Tab
                || keyevent->key()==Qt::Key_Backtab){
            if(DSBoxABS==watched){
                DSBoxArgument->selectAll();
                DSBoxArgument->setFocus();
            }else if(DSBoxArgument==watched){
                DSBoxABS->setFocus();
                DSBoxABS->selectAll();
            }
            //focusNextChild();
            return true;
        }else
            return QWidget::eventFilter(watched,event);;

    }else{
        return QWidget::eventFilter(watched,event);
    }
}

