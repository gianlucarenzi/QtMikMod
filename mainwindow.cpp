#include "mainwindow.h"
#include <QDebug> // Added for qDebug()

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

    vuMeter = new VuMeterWidget(this); // Parent is MainWindow
    vuMeter->show(); // Ensure it's visible

    // Directly load the module
    player->loadModule("mod/stardstm.mod");
    player->start();
    player->startLevelPolling(); // Start polling levels

    // Connect signals
    connect(player, &MikModPlayer::audioLevels, vuMeter, &VuMeterWidget::setAudioLevels);
    connect(player, &MikModPlayer::songFinished, this, &MainWindow::handleSongFinished);
}

MainWindow::~MainWindow() {
    player->stopLevelPolling(); // Stop polling levels
    player->stopPlayback();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) { // Example: Spacebar to pause/resume
        if (player->isPlaying()) {
            player->pausePlayback();
            player->stopLevelPolling(); // Stop polling levels when paused
            vuMeter->startDecay(); // Start VU meter decay when paused
        } else {
            player->pausePlayback(); // Resume playback by toggling pause
            player->startLevelPolling(); // Start polling levels when resumed
        }
    }
    QMainWindow::keyPressEvent(event); // Call base class implementation
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    // Position vuMeter at the bottom center
    int vuMeterWidth = vuMeter->width();
    int vuMeterHeight = vuMeter->height(); // Fixed at 48px
    int x = (width() - vuMeterWidth) / 2;
    int y = height() - vuMeterHeight - 5; // 5 pixels from the bottom
    vuMeter->setGeometry(x, y, vuMeterWidth, vuMeterHeight);
}

void MainWindow::handleSongFinished() {
    qDebug() << "Song finished. Pausing for 3 seconds and restarting.";
    player->stopLevelPolling(); // Stop polling levels
    player->stopPlayback(); // Ensure playback is stopped
    vuMeter->startDecay(); // Start VU meter decay
    QThread::sleep(3); // Pause for 3 seconds
    player->loadModule("mod/stardstm.mod"); // Reload the same module
    player->start(); // Start playback again
    player->startLevelPolling(); // Start polling levels
}

