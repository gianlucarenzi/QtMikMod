#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/bitmaps/icon.ico"));

    // Load custom font
    int fontId = QFontDatabase::addApplicationFont(":/fonts/TopazPlus_a500_v1.0.ttf");
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            QFont customFont(fontFamilies.at(0));
            a.setFont(customFont);
        }
    }
    
    MainWindow w;
    w.setWindowTitle("MikMod Player Cross-Platform");
    w.resize(400, 150);
    w.show();
    
    return a.exec();
}
