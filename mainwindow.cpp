#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    player = new MikModPlayer(this);
    
    titleLabel = new QLabel("QtMikMod Player", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    setCentralWidget(titleLabel);

    // Directly load the module
    player->loadModule("mod/stardstm.mod");
    player->start();
}

MainWindow::~MainWindow() {
    player->stopPlayback();
}

