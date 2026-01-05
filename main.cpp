#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QSettings>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("RetroBitLab");
    QCoreApplication::setApplicationName("QtMikMod");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QApplication a(argc, argv);

#ifdef __APPLE__
    // On macOS, when running from a bundle, the working directory is the user's home dir.
    // We need to change it to the Resources directory inside the bundle, where the assets are.
    QDir::setCurrent(QCoreApplication::applicationDirPath() + "/../Resources");
#endif

    a.setWindowIcon(QIcon(":/bitmaps/icon.ico"));
    int id = -1; // Initialize id outside the ifdef

#ifdef __APPLE__
    // On macOS, when running from a bundle, the working directory is the user's home dir.
    // We need to change it to the Resources directory inside the bundle, where the assets are.
    QDir::setCurrent(QCoreApplication::applicationDirPath() + "/../Resources");
    QString fontPath = QCoreApplication::applicationDirPath() + "/../Resources/fonts/TopazPlus_a500_v1.0.ttf";
    id = QFontDatabase::addApplicationFont(fontPath);
#else
    // For other platforms, use the original relative path
    id = QFontDatabase::addApplicationFont("fonts/TopazPlus_a500_v1.0.ttf");
#endif
    if (id < 0)
    {
        qDebug() << __FUNCTION__ << "Missing font!";
    }
    else
    {
        QStringList fontFamily = QFontDatabase::applicationFontFamilies(id);
        QFont amigaTopaz(fontFamily.at(0));
        a.setFont(amigaTopaz);
    }
    
    // Default MOD to play
    QString fileName = "mod/stardstm.mod";
    int pollTimer = 50;
    int updateTimer = 20;

    if (argc > 3)
    {
        fileName = QString::fromLocal8Bit(argv[1]);
        pollTimer = strtoul(argv[2], NULL, 10);
        updateTimer = strtoul(argv[3], NULL, 10);
    }
    if (argc > 2)
    {
        fileName = QString::fromLocal8Bit(argv[1]);
        pollTimer = strtoul(argv[2], NULL, 10);
    }
    if (argc > 1)
    {
        fileName = QString::fromLocal8Bit(argv[1]);
    }
    qDebug() << "Filename passed " << fileName;
    qDebug() << "pollTimer passed " << pollTimer;
    qDebug() << "updateTimer passed " << updateTimer;

    MainWindow w(nullptr, fileName, updateTimer, pollTimer);

    w.setup();

    w.setWindowTitle("MikMod Player Cross-Platform");
    w.resize(400, 150);
    w.show();
    
    return a.exec();
}
