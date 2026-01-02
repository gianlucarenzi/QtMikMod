#ifndef MIKMODPLAYER_H
#define MIKMODPLAYER_H

#include <QThread>
#include <QString>
#include <mikmod.h>

class MikModPlayer : public QThread {
    Q_OBJECT
public:
    explicit MikModPlayer(QObject *parent = nullptr);
    ~MikModPlayer();
    void loadModule(const QString &fileName);
    void stopPlayback();

protected:
    void run() override;

private:
    MODULE *module = nullptr;
    bool keepPlaying = false;
    bool libLoaded = false;
    bool initLibrary();

#ifdef Q_OS_WIN
    // Definizioni per puntatori a funzioni (Windows DLL)
    typedef void (*v_v)();
    typedef int  (*i_p)(char*);
    typedef int  (*i_v)();
    typedef MODULE* (*m_p)(char*, int, int);
    typedef void (*v_m)(MODULE*);

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

    // Puntatori a variabili globali di configurazione
    uint *p_md_mode = nullptr;
    uint *p_md_mixfreq = nullptr;
    uint *p_md_devicebuffer = nullptr;
#endif
};

#endif

