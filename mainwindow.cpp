#include "mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    player = new MikModPlayer(this);
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    btnOpen = new QPushButton("Seleziona File Tracker (.MOD, .XM, .IT)", this);
    layout->addWidget(btnOpen);
    setCentralWidget(central);

    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::handleOpenFile);
}

MainWindow::~MainWindow() {
    player->stopPlayback();
}

void MainWindow::handleOpenFile() {
    QString fn = QFileDialog::getOpenFileName(this, "Apri Modulo", "", "Moduli (*.mod *.xm *.s3m *.it)");
    if (!fn.isEmpty()) {
        player->stopPlayback();
        player->loadModule(fn);
        player->start();
    }
}

