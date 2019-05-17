#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTabWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:

private:
    void setupUi();

private:
    Ui::MainWindow *ui;
    QTabWidget* m_tabWidget;
};

#endif // MAINWINDOW_H
