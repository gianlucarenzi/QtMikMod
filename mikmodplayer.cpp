#include "mikmodplayer.h"
#include <QDebug>
#include <QLibrary>
#include <QVector>
#include <algorithm> // For std::max

MikModPlayer::MikModPlayer(QObject *parent)
: QThread(parent)
{
    libLoaded = initLibrary();

    m_levelTimer = new QTimer(this);
    connect(m_levelTimer, &QTimer::timeout, this, &MikModPlayer::pollAudioLevels);
    m_levelTimer->setInterval(50); // Poll every 50ms

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &MikModPlayer::updateMikMod);
    m_updateTimer->setInterval(15); // MikMod update interval

    // Connect signals to private slots for thread-safe operations
    connect(this, &MikModPlayer::requestStartLevelPolling, this, &MikModPlayer::m_startLevelPolling);
    connect(this, &MikModPlayer::requestStopLevelPolling, this, &MikModPlayer::m_stopLevelPolling);
    connect(this, &MikModPlayer::requestStartUpdateTimer, this, &MikModPlayer::m_startUpdateTimer);
    connect(this, &MikModPlayer::requestStopUpdateTimer, this, &MikModPlayer::m_stopUpdateTimer);
    connect(this, &MikModPlayer::requestTogglePause, this, &MikModPlayer::m_togglePause);
    connect(this, &MikModPlayer::requestStopPlayback, this, &MikModPlayer::m_stopPlayback);
}

bool MikModPlayer::initLibrary()
{
#ifdef Q_OS_WIN
    QLibrary lib("libmikmod.dll");
    if (!lib.load()) {
        qCritical() << "Errore: libmikmod.dll non trovata!";
        return false;
    }

	// Corrected function's casts
    p_MikMod_RegisterAllDrivers = (v_v)lib.resolve("MikMod_RegisterAllDrivers");
    p_MikMod_RegisterAllLoaders = (v_v)lib.resolve("MikMod_RegisterAllLoaders");
    p_MikMod_Init = (i_p)lib.resolve("MikMod_Init");
    p_MikMod_Exit = (v_v)lib.resolve("MikMod_Exit");
    p_Player_Load = (m_p)lib.resolve("Player_Load");
    p_Player_Start = (v_m_v)lib.resolve("Player_Start");
    p_Player_Stop = (v_v)lib.resolve("Player_Stop");
    p_Player_Active = (i_v)lib.resolve("Player_Active");
    p_Player_Free = (v_m)lib.resolve("Player_Free");
    p_MikMod_Update = (v_v)lib.resolve("MikMod_Update");
    p_Player_SetVolume = (v_i)lib.resolve("Player_SetVolume");
    p_Player_TogglePause = (v_v)lib.resolve("Player_TogglePause");
    p_Player_QueryVoices = (i_uw_vp)lib.resolve("Player_QueryVoices");
    p_Voice_RealVolume = (ul_sb)lib.resolve("Voice_RealVolume");
    p_Player_GetChannelVoice = (sb_ub)lib.resolve("Player_GetChannelVoice");

    p_md_mode = (uint*)lib.resolve("md_mode");
    p_md_mixfreq = (uint*)lib.resolve("md_mixfreq");
    p_md_devicebuffer = (uint*)lib.resolve("md_devicebuffer");
    p_Player_Volume = (int*)lib.resolve("Player_Volume");

    // Check critical functions
    if (!p_MikMod_Init || !p_md_mode || !p_Player_SetVolume || !p_Player_TogglePause ||
        !p_Player_QueryVoices || !p_Voice_RealVolume || !p_Player_GetChannelVoice)
        return false;

    // Audio Mode Configuration
    *p_md_mode = DMODE_SOFT_MUSIC | DMODE_INTERP | DMODE_STEREO | DMODE_16BITS;
    *p_md_mixfreq = 44100;
    if (p_md_devicebuffer)
        *p_md_devicebuffer = 30;

    // Init Library
    p_MikMod_RegisterAllDrivers();
    p_MikMod_RegisterAllLoaders();
    if (p_MikMod_Init((char*)"")) return false;

#else
    // Linux/macOS
    md_mode = DMODE_SOFT_MUSIC | DMODE_INTERP | DMODE_STEREO | DMODE_16BITS;
    md_mixfreq = 44100;
    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();
    if (MikMod_Init((char*)"")) return false;
#endif

    return true;
}

MikModPlayer::~MikModPlayer()
{
    m_stopPlayback(); // Ensure everything is stopped before destruction
    if (m_levelTimer) {
        m_levelTimer->stop();
        delete m_levelTimer;
    }
    if (m_updateTimer) {
        m_updateTimer->stop();
        delete m_updateTimer;
    }
    if (module) {
#ifdef Q_OS_WIN
        if(p_Player_Free) p_Player_Free(module);
        if(p_MikMod_Exit) p_MikMod_Exit();
#else
        Player_Free(module);
        MikMod_Exit();
#endif
    }
}

void MikModPlayer::loadModule(const QString &fileName)
{
    qDebug() << "Module to load: " << fileName;
    if (!libLoaded) return;
    if (module) stopPlayback(); // Stop current playback before loading new module
    
    QByteArray ba = fileName.toLocal8Bit();
#ifdef Q_OS_WIN
    module = p_Player_Load(ba.data(), 64, 0);
#else
    module = Player_Load(ba.data(), 64, 0);
#endif
}

void MikModPlayer::run() {
    if (!module) return;
    
    keepPlaying = true;
    m_paused = false; // Ensure not paused when starting new song

#ifdef Q_OS_WIN
    if (p_Player_Start) p_Player_Start(module);
#else
    Player_Start(module);
#endif
    emit requestStartUpdateTimer(); // Request to start the update timer in this thread
    emit requestStartLevelPolling(); // Request to start the level polling timer in this thread
    exec(); // Start the event loop for this thread
}

void MikModPlayer::updateMikMod() {
    if (!keepPlaying) { // Should not happen if timer is stopped correctly
        m_stopUpdateTimer();
        return;
    }

    if (!m_paused) {
#ifdef Q_OS_WIN
        if (p_Player_Active && p_Player_Active()) {
            if (p_MikMod_Update) p_MikMod_Update();
        } else {
            // Song finished
            keepPlaying = false;
            m_stopUpdateTimer();
            emit songFinished();
        }
#else
        if (Player_Active()) {
            MikMod_Update();
        } else {
            // Song finished
            keepPlaying = false;
            m_stopUpdateTimer();
            emit songFinished();
        }
#endif
    }
}

void MikModPlayer::stopPlayback() {
    emit requestStopPlayback(); // Request to stop playback in this thread
}

void MikModPlayer::pausePlayback() {
    emit requestTogglePause(); // Request to toggle pause in this thread
}

bool MikModPlayer::isPlaying() const {
#ifdef Q_OS_WIN
    return keepPlaying && !m_paused && p_Player_Active && p_Player_Active();
#else
    return keepPlaying && !m_paused && Player_Active();
#endif
}

void MikModPlayer::startLevelPolling() {
    emit requestStartLevelPolling(); // Request to start level polling in this thread
}

void MikModPlayer::stopLevelPolling() {
    emit requestStopLevelPolling(); // Request to stop level polling in this thread
}

void MikModPlayer::pollAudioLevels() {
    if (!module || m_paused) {
        emit audioLevels(QVector<float>()); // Empty if paused/stopped
        return;
    }

    QVector<float> levels(module->numchn, 0.0f);
    float maxVolume = 65535.0f; 
    float peakLevel = 0.0f;

    for (UBYTE channel = 0; channel < module->numchn; ++channel) {
        SBYTE voice = -1;
#ifdef Q_OS_WIN
        if (p_Player_GetChannelVoice) {
            voice = p_Player_GetChannelVoice(channel);
        }
#else
        voice = Player_GetChannelVoice(channel);
#endif
        // qDebug() << "  Logical Channel:" << channel << "Physical Voice:" << voice;

        if (voice != -1) { // If a voice is assigned to this channel
            ULONG volume = 0;
#ifdef Q_OS_WIN
            if (p_Voice_RealVolume) {
                volume = p_Voice_RealVolume(voice);
            }
#else
            volume = Voice_RealVolume(voice);
#endif
            float normalizedVolume = static_cast<float>(volume) / maxVolume;
            levels[channel] = normalizedVolume;
			peakLevel = std::max(peakLevel, normalizedVolume);
			// qDebug() << "    Volume:" << volume << "Normalized:" << normalizedVolume;
        }
    }

    // Debug del picco massimo (facoltativo)
    // qDebug() << "Peak level:" << peakLevel;

    emit audioLevels(levels);
}

// Private slots implementations
void MikModPlayer::m_startLevelPolling() {
    if (m_levelTimer) {
        m_levelTimer->start();
    }
}

void MikModPlayer::m_stopLevelPolling() {
    if (m_levelTimer) {
        m_levelTimer->stop();
    }
}

void MikModPlayer::m_startUpdateTimer() {
    if (m_updateTimer) {
        m_updateTimer->start();
    }
}

void MikModPlayer::m_stopUpdateTimer() {
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void MikModPlayer::m_togglePause() {
    m_paused = !m_paused; // Toggle pause state
#ifdef Q_OS_WIN
    if (p_Player_TogglePause) p_Player_TogglePause();
#else
    Player_TogglePause();
#endif
    if (m_paused) {
        m_stopUpdateTimer();
    } else {
        m_startUpdateTimer();
    }
}

void MikModPlayer::m_stopPlayback() {
    keepPlaying = false;
    m_paused = false; // Reset pause state
    m_stopUpdateTimer();
    m_stopLevelPolling(); // Also stop level polling
#ifdef Q_OS_WIN
    if (p_Player_Stop) p_Player_Stop();
#else
    Player_Stop();
#endif
    quit(); // Exit the event loop
    wait(); // Wait for the thread to finish
}
