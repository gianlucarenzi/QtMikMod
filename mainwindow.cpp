#include "mainwindow.h"
// #include <QFileDialog> // No longer needed

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    player = new MikModPlayer(this);
    // QWidget *central = new QWidget(this); // No longer needed
    // QVBoxLayout *layout = new QVBoxLayout(central); // No longer needed

    // btnOpen = new QPushButton("Seleziona File Tracker (.MOD, .XM, .IT)", this); // No longer needed
    // layout->addWidget(btnOpen); // No longer needed
    // setCentralWidget(central); // No longer needed

    // connect(btnOpen, &QPushButton::clicked, this, &MainWindow::handleOpenFile); // No longer needed

    // Directly load the module
    player->loadModule("mod/stardstm.mod");
    player->start();
}

MainWindow::~MainWindow() {
    player->stopPlayback();
}

// void MainWindow::handleOpenFile() { // No longer needed
//     QString fn = QFileDialog::getOpenFileName(this, "Apri Modulo", "", "Moduli (*.mod *.xm *.s3m *.it)");
//     if (!fn.isEmpty()) {
//         player->stopPlayback();
//         player->loadModule(fn);
//         player->start();
//     }
// }

