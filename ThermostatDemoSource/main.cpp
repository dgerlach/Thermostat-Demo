#include <QtGui>

#include "mainwindow.h"
#include "weatherwidget.h"
#include "thermostatwidget.h"
#include "optionswidget.h"
#include "webdata.h"

#include <QtDebug>

int main(int argc, char *argv[])
{

    // If you're looking for where to start reading this code,
    // check out STARTHERE.h
    QApplication app(argc, argv);

    // read style sheet and apply pararmeters
    QFile file(":/qss/thermostatdemo.qss");
    file.open(QFile::ReadOnly);
    app.setStyleSheet(file.readAll());
    file.close();
    // create main window and show
    MainWindow *w = new MainWindow;
    w->resize(app.desktop()->size());
    w->showFullScreen();

    // enter application loop
    return app.exec();
}

