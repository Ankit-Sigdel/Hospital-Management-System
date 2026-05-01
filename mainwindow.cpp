#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
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


//forgot password qmessagebox setup
void MainWindow::on_forgotPasswordLabel_linkActivated(const QString &/*link*/)
{
    QMessageBox::information(this,
                             "Account Recovery",
                             "Please contact the System Administrator to reset your password.");
}



//logic for authentication from csv
void MainWindow::on_loginButton_clicked()
{
    QString enteredUser = ui->usernameInput->text();
    QString enteredPass = ui->passwordInput->text();
    QString selectedRole = ui->roleComboBox->currentText();

    if (enteredUser.isEmpty() || enteredPass.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please fill in all fields.");
        return;
    }

    QFile file(":/database/users.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Database file not found.");
        return;
    }

    QTextStream in(&file);
    bool authenticated = false;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        // CSV format: username, password, role
        if (fields.size() >= 3) {
            QString csvUser = fields[0].trimmed();
            QString csvPass = fields[1].trimmed();
            QString csvRole = fields[2].trimmed();

            if (enteredUser == csvUser && enteredPass == csvPass && selectedRole == csvRole) {
                authenticated = true;
                break;
            }
        }
    }
    file.close();

    if (authenticated) {
        QMessageBox::information(this, "Success", "Welcome, " + enteredUser + "!");
        // Proceed to the next window based on role
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username, password, or role.");
    }
}