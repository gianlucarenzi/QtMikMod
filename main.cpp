#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    MainWindow w;
    w.setWindowTitle("MikMod Player Cross-Platform");
    w.resize(400, 150);
    w.show();
    
    return a.exec();
}
