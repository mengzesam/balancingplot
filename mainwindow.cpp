#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "balancingplot.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    ,m_tabWidget(nullptr)
{
    setupUi();
    ui->statusBar->showMessage("balancing plot");
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(-10+QApplication::desktop()->availableGeometry().width()
               ,-30+QApplication::desktop()->availableGeometry().height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUi()
{
    ui->setupUi(this);
    m_tabWidget=new QTabWidget(this);
    m_tabWidget->setObjectName(QStringLiteral("tabWidget"));
    BalancingPlot* Plot = new BalancingPlot();
    m_tabWidget->addTab(Plot, QString("Plot"));
    QWidget* SingleDisk = new QWidget();
    SingleDisk->setObjectName(QStringLiteral("SingleDisk"));
    m_tabWidget->addTab(SingleDisk, QString("SingleDisk"));
    m_tabWidget->setCurrentIndex(0);
    ui->verticalLayout->addWidget(m_tabWidget);

}
