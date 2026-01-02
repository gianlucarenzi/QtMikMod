#ifndef MIKMODPLAYER_H
#define MIKMODPLAYER_H

#include <QThread>
#include <QString>
#include <QVector>
#include <QTimer> // Added for audio level polling
#include <mikmod.h>

class MikModPlayer : public QThread {
    Q_OBJECT
public:
    explicit MikModPlayer(QObject *parent = nullptr);
    ~MikModPlayer();
    void loadModule(const QString &fileName);
    void stopPlayback();
    void pausePlayback(); // New method to pause playback
    bool isPlaying() const; // New method to check if playing
    void startLevelPolling(); // New method to start the level polling timer
    void stopLevelPolling();  // New method to stop the level polling timer

signals:
    void audioLevels(const QVector<float>& levels); // Signal to emit audio levels
    void songFinished(); // Signal emitted when a song finishes

    // Signals to request actions in the MikModPlayer thread
    void requestStartLevelPolling();
    void requestStopLevelPolling();
    void requestStartUpdateTimer();
    void requestStopUpdateTimer();
    void requestTogglePause();
    void requestStopPlayback();

protected:
    void run() override;

private:
    MODULE *module = nullptr;
    bool keepPlaying = false;
    bool libLoaded = false;
    bool m_paused = false; // New member to track pause state
    QTimer *m_levelTimer = nullptr; // Timer for polling audio levels
    QTimer *m_updateTimer = nullptr; // Timer for MikMod updates and playback checks
    bool initLibrary();
    void pollAudioLevels(); // New method to poll audio levels

private slots:
    void updateMikMod(); // New slot for periodic MikMod updates and playback checks
    // Private slots to handle requests from other threads
    void m_startLevelPolling();
    void m_stopLevelPolling();
    void m_startUpdateTimer();
    void m_stopUpdateTimer();
    void m_togglePause();
    void m_stopPlayback();

#ifdef Q_OS_WIN
    // Definizioni per puntatori a funzioni (Windows DLL)
    typedef void (*v_v)();
    typedef int  (*i_p)(char*);
    typedef int  (*i_v)();
    typedef MODULE* (*m_p)(char*, int, int);
    typedef void (*v_m)(MODULE*);
    typedef void (*v_i)(int); // For Player_SetVolume
    typedef int (*i_uw_vp)(UWORD, VOICEINFO*); // For Player_QueryVoices
    typedef ULONG (*ul_sb)(SBYTE); // For Voice_RealVolume
    typedef SBYTE (*sb_ub)(UBYTE); // For Player_GetChannelVoice

    v_v p_MikMod_RegisterAllDrivers = nullptr;
    v_v p_MikMod_RegisterAllLoaders = nullptr;
    i_p p_MikMod_Init = nullptr;
    v_v p_MikMod_Exit = nullptr;
    m_p p_Player_Load = nullptr;
    v_v p_Player_Start = nullptr;
    v_v p_Player_Stop = nullptr;
    i_v p_Player_Active = nullptr;
    v_m p_Player_Free = nullptr;
    v_v p_MikMod_Update = nullptr;
    v_i p_Player_SetVolume = nullptr; // New: Player_SetVolume
    v_v p_Player_TogglePause = nullptr; // New: Player_TogglePause
    i_uw_vp p_Player_QueryVoices = nullptr; // New: Player_QueryVoices
    ul_sb p_Voice_RealVolume = nullptr; // New: Voice_RealVolume
    sb_ub p_Player_GetChannelVoice = nullptr; // New: Player_GetChannelVoice

    // Puntatori a variabili globali di configurazione
    uint *p_md_mode = nullptr;
    uint *p_md_mixfreq = nullptr;
    uint *p_md_devicebuffer = nullptr;
    int *p_Player_Volume = nullptr; // New: Player_Volume
#endif
};

#endif

