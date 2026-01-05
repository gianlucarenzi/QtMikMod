#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel> // Added for displaying text
#include <QKeyEvent> // Added for keyPressEvent
#include "mikmodplayer.h"
#include "vumeterwidget.h" // Include for VuMeterWidget

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr, QString fileName = "mod/stardstm.mod", int updateTimer = 20, int pollTimer = 50);
    ~MainWindow();
    void setFilename(QString filename);
    void setup();

private:
    MikModPlayer *player;
    QLabel *titleLabel; // Added QLabel member
    QLabel *commitIdLabel; // Added QLabel for commit ID
    VuMeterWidget *vuMeter; // Added VuMeterWidget member
    QTimer *m_guiTimer; // Timer for GUI updates
    QString m_fileName;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override; // New: Override resizeEvent

private slots:
    void handleSongFinished(); // New slot for handling song finished event
    void updateVuMeter(); // Slot for GUI timer
};
#endif

