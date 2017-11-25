/* ************************************************************************ */
/* Copyright 2017 Jiří Fatka. All rights reserved.                          */
/* ************************************************************************ */

#include "MainWindow.hpp"
#include <QApplication>

/* ************************************************************************* */

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("AssemblyExplorer");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

/* ************************************************************************* */
