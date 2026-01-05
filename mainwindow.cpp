#include "mainwindow.h"
#include <QDebug> // Added for qDebug()

#ifdef GIT_COMMIT_ID
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#endif

MainWindow::MainWindow(QWidget *parent, QString filename, int updateTimer, int pollTimer)
: QMainWindow(parent),
  player(new MikModPlayer(this, updateTimer, pollTimer)),
  m_fileName(filename)
{
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

    // Setup GUI timer
    m_guiTimer = new QTimer(this);
    connect(m_guiTimer, &QTimer::timeout, this, &MainWindow::updateVuMeter);
    m_guiTimer->setInterval(pollTimer);

    // Connect signals
    connect(player, &MikModPlayer::songFinished, this, &MainWindow::handleSongFinished);
}

MainWindow::~MainWindow() {
    m_guiTimer->stop();
    player->stopPlayback();
}

void MainWindow::setup()
{
    // Directly load the module
    player->loadModule(m_fileName);
    player->start();
    m_guiTimer->start();
}
void MainWindow::setFilename(QString filename)
{
    m_fileName = filename;
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) { // Example: Spacebar to pause/resume
        player->pausePlayback();
        if (player->isPlaying()) {
            m_guiTimer->start();
        } else {
            m_guiTimer->stop();
            vuMeter->startDecay(); // Start VU meter decay when paused
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
    m_guiTimer->stop();
    player->stopPlayback(); // Ensure playback is stopped
    vuMeter->startDecay(); // Start VU meter decay
    QThread::sleep(3); // Pause for 3 seconds
    player->loadModule(m_fileName); // Reload the same module
    player->start(); // Start playback again
    m_guiTimer->start();
}

void MainWindow::updateVuMeter()
{
    if (player && player->isPlaying()) {
        vuMeter->setAudioLevels(player->getCurrentLevels());
    }
}
