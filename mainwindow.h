#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// #include <QPushButton> // No longer needed
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel> // Added for displaying text
#include "mikmodplayer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
// private slots: // No longer needed
//    void handleOpenFile(); // No longer needed
private:
    MikModPlayer *player;
    // QPushButton *btnOpen; // No longer needed
    QLabel *titleLabel; // Added QLabel member
    QLabel *commitIdLabel; // Added QLabel for commit ID
};
#endif

