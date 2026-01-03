#ifndef MIKMODPLAYER_H
#define MIKMODPLAYER_H

#include <QThread>
#include <QString>
#include <QVector>
#include <QTimer>

#ifndef Q_MOC_RUN
#include <mikmod.h>
#endif

class MikModPlayer : public QThread
{
    Q_OBJECT

public:
    explicit MikModPlayer(QObject *parent = nullptr);
    ~MikModPlayer();

    void loadModule(const QString &fileName);
    void stopPlayback();
    void pausePlayback();
    bool isPlaying() const;

    void startLevelPolling();
    void stopLevelPolling();

signals:
    void audioLevels(const QVector<float>& levels);
    void songFinished();

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
    bool m_paused = false;

    QTimer *m_levelTimer = nullptr;
    QTimer *m_updateTimer = nullptr;

    bool initLibrary();
    void pollAudioLevels();

private slots:
    void updateMikMod();

    // Private slots to handle requests from other threads
    void m_startLevelPolling();
    void m_stopLevelPolling();
    void m_startUpdateTimer();
    void m_stopUpdateTimer();
    void m_togglePause();
    void m_stopPlayback();

#ifdef Q_OS_WIN
#ifndef Q_MOC_RUN
    // -------------------------------------------------
    // Windows: function pointer typedefs (libmikmod)
    // -------------------------------------------------
    typedef void   (*v_v)(void);
    typedef int    (*i_p)(char*);
    typedef int    (*i_v)(void);
    typedef MODULE*(*m_p)(char*, int, int);
    typedef void   (*v_m)(MODULE*);
    typedef void   (*v_m_v)(MODULE*);   // Player_Start
    typedef void   (*v_i)(int);
    typedef int    (*i_uw_vp)(UWORD, VOICEINFO*);
    typedef ULONG  (*ul_sb)(SBYTE);
    typedef SBYTE  (*sb_ub)(UBYTE);

    // Solo dichiarazioni
    v_v     p_MikMod_RegisterAllDrivers;
    v_v     p_MikMod_RegisterAllLoaders;
    i_p     p_MikMod_Init;
    v_v     p_MikMod_Exit;

    m_p     p_Player_Load;
    v_m_v   p_Player_Start;
    v_v     p_Player_Stop;
    i_v     p_Player_Active;
    v_m     p_Player_Free;
    v_v     p_MikMod_Update;

    v_i     p_Player_SetVolume;
    v_v     p_Player_TogglePause;
    i_uw_vp p_Player_QueryVoices;
    ul_sb   p_Voice_RealVolume;
    sb_ub   p_Player_GetChannelVoice;

    uint *p_md_mode;
    uint *p_md_mixfreq;
    uint *p_md_devicebuffer;
    int  *p_Player_Volume;
#endif
#endif
};

#endif // MIKMODPLAYER_H
