#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setOrganizationName("AXON");
    app.setApplicationName("AXON_HMS");

    MainWindow loginWin;
    loginWin.show();
    return app.exec();
}
