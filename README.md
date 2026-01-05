# Lettore Musicale QtMikMod

`QtMikMod` è un semplice lettore musicale cross-platform per file in formato "module" (come `.mod`, `.s3m`, `.xm`, ecc.), costruito utilizzando il framework **Qt** per linterfaccia grafica e la libreria **libmikmod** per la riproduzione audio.

Questo progetto nasce come un'esplorazione di diverse tecniche di sviluppo software in C++ con Qt, tra cui:
- Creazione di widget personalizzati (`VuMeterWidget`).
- Gestione del multithreading per separare linterfaccia grafica (GUI) dal backend audio.
- Integrazione di librerie esterne.
- Creazione di pipeline di Continuous Integration/Continuous Deployment (CI/CD) con GitHub Actions per la compilazione automatica su Linux, Windows e macOS.

## 🌟 Caratteristiche Principali

- **Riproduzione di file MOD**: Carica e riproduce file musicali in formato "module".
- **VU Meter in Tempo Reale**: Visualizza i livelli di volume di ogni canale audio con un widget personalizzato.
- **Architettura Multi-thread**: Un thread dedicato alla riproduzione audio assicura che linterfaccia grafica rimanga sempre reattiva.
- **Cross-Platform**: Grazie a Qt e a CI/CD, il progetto viene compilato per Linux (AppImage), Windows (ZIP portable) e macOS (DMG).

## 🛠️ Compilazione del Progetto

Per compilare il progetto in locale, è necessario avere le dipendenze corrette.

### Dipendenze
- **Qt 5**: Il framework principale per la GUI (`qt5-default` o simile).
- **libmikmod**: La libreria per la decodifica e la riproduzione audio (`libmikmod-dev` o simile).
- Un compilatore C++ (GCC, Clang, MSVC).

### Procedura
1. **Installare le dipendenze** (esempio per sistemi Debian/Ubuntu):
   ```bash
   sudo apt-get update
   sudo apt-get install qt5-default libmikmod-dev
   ```

2. **Clonare il repository**:
   ```bash
   git clone <URL_DEL_REPOSITORY>
   cd QtMikMod
   ```

3. **Generare il Makefile con `qmake`**:
   ```bash
   qmake MikModPlayer.pro
   ```

4. **Compilare con `make`**:
   ```bash
   make
   ```
L'eseguibile `MikModPlayer` verrà creato nella directory corrente.

## 📂 Struttura del Progetto

- `main.cpp`: Punto di ingresso dellapplicazione.
- `mainwindow.h`, `mainwindow.cpp`: Gestisce la finestra principale, linterfaccia utente (widget, layout) e linterazione con lutente.
- `mikmodplayer.h`, `mikmodplayer.cpp`: Classe che gestisce il backend audio in un thread separato. Interagisce direttamente con `libmikmod`.
- `vumeterwidget.h`, `vumeterwidget.cpp`: Widget personalizzato che disegna i VU meter.
- `MikModPlayer.pro`: File di progetto di `qmake`.
- `.github/workflows/`: Contiene le pipeline di CI/CD per la compilazione automatica.
- `3rdparty/`: Contiene le librerie di terze parti (se non fornite dal sistema).
- `mod/`, `fonts/`, `bitmaps/`: Risorse dellapplicazione come file musicali, font e icone.

## 📊 Architettura del Codice

Il diagramma seguente illustra il flusso di dati e linterazione tra i componenti principali dellapplicazione.

```mermaid
graph TD
    subgraph Main Thread (GUI)
        A[Avvio Applicazione] --> B(Crea MainWindow)
        B --> C{Crea MikModPlayer player
Crea VuMeterWidget vuMeter
Crea QTimer m_guiTimer}
        C --> D[player.start()]
        C --> E[m_guiTimer.start()]

        subgraph "Ciclo di Update Grafico"
            E -- timeout --> F[updateVuMeter()]
            F --> G{levels = player.getCurrentLevels()}
            G --> H[vuMeter.setAudioLevels(levels)]
            H --> I[vuMeter.repaint()]
        end

        J[Input Utente (es. Spazio)] --> K{Pausa/Riprendi}
        K --> L[player.pausePlayback()]
        K --> M[m_guiTimer.stop()/start()]
    end

    subgraph "Worker Thread (Audio)"
        D -- avvia --> N[run()]
        N --> O{Crea e avvia QTimer m_updateTimer}
        subgraph "Ciclo di Update Audio"
            O -- timeout --> P[updateMikMod()]
            P --> Q[libmikmod: MikMod_Update()]
            Q --> R((Scheda Audio))
        end
    end

    style R fill:#f9f,stroke:#333,stroke-width:2px
```
**Spiegazione del Diagramma:**
1.  **Thread Principale (GUI)**:
    - Allavvio, `MainWindow` crea il player audio, il VU meter e un timer (`m_guiTimer`) per gli aggiornamenti grafici.
    - Il `m_guiTimer` scatta a intervalli regolari e chiama `updateVuMeter()`.
    - `updateVuMeter()` chiede i livelli audio correnti al thread del player (`player.getCurrentLevels()`) e li passa al `VuMeterWidget`, che si ridisegna.
    - Gestisce anche linput dellutente.
2.  **Thread Secondario (Audio)**:
    - Allavvio (`player.start()`), viene creato un nuovo thread.
    - Allinterno di questo thread, un altro timer (`m_updateTimer`) si occupa di chiamare periodicamente `MikMod_Update()`.
    - Questa funzione riempie il buffer della scheda audio, garantendo una riproduzione fluida e senza interruzioni, indipendentemente da ciò che fa linterfaccia grafica.
    
Questa architettura garantisce che uninterfaccia grafica pesante o bloccata non interrompa mai la musica.

## 🚀 Workflow di CI/CD (GitHub Actions)

Il progetto utilizza GitHub Actions per automatizzare la compilazione, il test e il rilascio per diverse piattaforme.

### `build-linux.yml`
- **Scopo**: Compila lapplicazione per Linux e crea un `AppImage`.
- **Trigger**: Si attiva ad ogni `push` o `pull_request` sui rami `main`/`master`, e quando viene creata una nuova release.
- **Processo**:
    1. Esegue uno script `build.sh` (non presente nel listato, ma dedotto dal workflow) che si occupa di installare le dipendenze e creare un pacchetto `AppImage`.
    2. Carica lartefatto `QtMikMod-x86_64.AppImage` per ogni build.
    3. Se la build è stata attivata da una release, carica lAppImage come asset di quella release su GitHub.

### `build-windows.yml`
- **Scopo**: Compila lapplicazione per Windows a 64 bit.
- **Trigger**: Come per Linux.
- **Processo**:
    1. Utilizza `jurplel/install-qt-action` per installare lambiente Qt (MSVC 2019).
    2. Installa `vcpkg` per gestire le dipendenze C++.
    3. Installa `libmikmod` tramite `vcpkg`.
    4. Compila il progetto usando `qmake` e `nmake`.
    5. Utilizza `windeployqt` per raccogliere tutte le DLL di Qt necessarie.
    6. Copia manualmente le DLL di `libmikmod` e del runtime di Visual C++.
    7. Crea un pacchetto `.zip` contenente leseguibile e tutte le dipendenze, pronto per luso.
    8. Carica lo `.zip` come artefatto e, in caso di release, come asset.

### `build-macos.yml`
- **Scopo**: Compila lapplicazione per macOS.
- **Trigger**: Come per Linux.
- **Processo**:
    1. Installa Qt usando `jurplel/install-qt-action`.
    2. Installa `libmikmod` tramite `Homebrew`.
    3. Compila il progetto con `qmake` e `make`.
    4. Utilizza `macdeployqt` per creare un bundle `.app` autonomo, copiando al suo interno le risorse necessarie (font, mod, etc.).
    5. Crea un file immagine `.dmg` contenente lapplicazione, pronto per la distribuzione.
    6. Carica il `.dmg` come artefatto.
