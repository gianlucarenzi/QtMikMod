#include "mainwindow.h"

#ifdef GIT_COMMIT_ID
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    player = new MikModPlayer(this);
    
    // Set dark gray background for the main window
    this->setStyleSheet("background-color: #555555;");

    // Title Label
    titleLabel = new QLabel("QtMikMod Player", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");

    // Commit ID Label
    commitIdLabel = new QLabel(this);
    commitIdLabel->setAlignment(Qt::AlignCenter);
    commitIdLabel->setStyleSheet("color: white; font-size: 12px;");
#ifdef GIT_COMMIT_ID
    commitIdLabel->setText(QString("Commit ID: %1").arg(TOSTRING(GIT_COMMIT_ID)));
#else
    commitIdLabel->setText("Commit ID: N/A");
#endif

    // Layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addStretch(); // Add stretch to push content to center
    layout->addWidget(titleLabel);
    layout->addWidget(commitIdLabel);
    layout->addStretch(); // Add stretch to push content to center

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Directly load the module
    player->loadModule("mod/stardstm.mod");
    player->start();
}

MainWindow::~MainWindow() {
    player->stopPlayback();
}

