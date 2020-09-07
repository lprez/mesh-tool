#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->menuFile->addAction(new QAction(QIcon(), QString::fromUtf8("test"), this));
}

MainWindow::~MainWindow()
{
    delete ui;
}
