#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_forgotPasswordLabel_linkActivated(const QString &/*link*/)
{
    QMessageBox::information(this,
                             "Account Recovery",
                             "Please contact the System Administrator to reset your password.");
}

