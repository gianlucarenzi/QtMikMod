#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// #include <QPushButton> // No longer needed
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel> // Added for displaying text
#include <QKeyEvent> // Added for keyPressEvent
#include "mikmodplayer.h"
#include "vumeterwidget.h" // Include for VuMeterWidget

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
    VuMeterWidget *vuMeter; // Added VuMeterWidget member

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override; // New: Override resizeEvent

private slots:
    void handleSongFinished(); // New slot for handling song finished event
};
#endif

