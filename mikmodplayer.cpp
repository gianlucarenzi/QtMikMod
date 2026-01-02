#include "mikmodplayer.h"
#include <QDebug>
#include <QLibrary>

MikModPlayer::MikModPlayer(QObject *parent) : QThread(parent) {
    libLoaded = initLibrary();
}

bool MikModPlayer::initLibrary() {
#ifdef Q_OS_WIN
    QLibrary lib("libmikmod.dll");
    if (!lib.load()) {
        qCritical() << "Errore: libmikmod.dll non trovata!";
        return false;
    }

    p_MikMod_RegisterAllDrivers = (v_v)lib.resolve("MikMod_RegisterAllDrivers");
    p_MikMod_RegisterAllLoaders = (v_v)lib.resolve("MikMod_RegisterAllLoaders");
    p_MikMod_Init = (i_p)lib.resolve("MikMod_Init");
    p_MikMod_Exit = (v_v)lib.resolve("MikMod_Exit");
    p_Player_Load = (m_p)lib.resolve("Player_Load");
    p_Player_Start = (v_v)lib.resolve("Player_Start");
    p_Player_Stop = (v_v)lib.resolve("Player_Stop");
    p_Player_Active = (i_v)lib.resolve("Player_Active");
    p_Player_Free = (v_m)lib.resolve("Player_Free");
    p_MikMod_Update = (v_v)lib.resolve("MikMod_Update");

    p_md_mode = (uint*)lib.resolve("md_mode");
    p_md_mixfreq = (uint*)lib.resolve("md_mixfreq");
    p_md_devicebuffer = (uint*)lib.resolve("md_devicebuffer");

    if (!p_MikMod_Init || !p_md_mode) return false;

    // Usiamo DMODE_16BITS (con la S) per compatibilità MSVC/DLL
    *p_md_mode = DMODE_SOFT_MUSIC | DMODE_INTERP | DMODE_STEREO | DMODE_16BITS;
    *p_md_mixfreq = 44100;
    
    if(p_md_devicebuffer) 
        *p_md_devicebuffer = 30; 

    p_MikMod_RegisterAllDrivers();
    p_MikMod_RegisterAllLoaders();
    if (p_MikMod_Init((char*)"")) return false;
#else
    // LINUX / MACOS
    // Correzione: DMODE_16BITS
    md_mode = DMODE_SOFT_MUSIC | DMODE_INTERP | DMODE_STEREO | DMODE_16BITS;
    md_mixfreq = 44100;

    // Se md_devicebuffer non esiste nella tua versione, MikMod userà i default del driver (ALSA/CoreAudio)
    // Proviamo a impostarlo solo se la versione della libreria lo supporta. 
    // In caso di errore persistente su Linux, commenta la riga sotto.
    // md_devicebuffer = 30; 

    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();
    if (MikMod_Init((char*)"")) return false;
#endif
    return true;
}

MikModPlayer::~MikModPlayer() {
    stopPlayback();
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

void MikModPlayer::loadModule(const QString &fileName) {
    if (!libLoaded) return;
    if (module) stopPlayback();
    
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
#ifdef Q_OS_WIN
    p_Player_Start(module);
    while (keepPlaying && p_Player_Active()) {
        p_MikMod_Update();
        msleep(15);
    }
    p_Player_Stop();
#else
    Player_Start(module);
    while (keepPlaying && Player_Active()) {
        MikMod_Update();
        msleep(15);
    }
    Player_Stop();
#endif
}

void MikModPlayer::stopPlayback() {
    keepPlaying = false;
    wait();
}

