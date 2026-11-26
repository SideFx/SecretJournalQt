/////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp
// Purpose:     Main
// Author:      Jan Buchholz
// Created:     2025-10-13
// Changed:     2026-11-26
/////////////////////////////////////////////////////////////////////////////

#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include "jbcrypto/jbselftest.h"

int main(int argc, char *argv[]) {
    JBSelfTest st;
    if (!st.doAllTests()) return 255;
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));
    MainWindow w;
    w.show();
    return a.exec();
}
