#include "mainwindow.h"
#include "search_server.h"

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

#include <memory>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
