#include "include/mainwindow.h"
#include <QApplication>
#include <QDateTime>
#include <QSysInfo>

std::map<ECellColors, QString> g_ColorNameMap;
CfgVars g_cfgVars;

CGame *mpGame;
QTimer *mpTicker;
CMainWindow *pMainWnd;

// Logging.
static std::filebuf l_fileBuff;
static std::ostream l_logStream(&l_fileBuff);

void SetupColorNames()
{
    g_ColorNameMap[Cell_White] = "White";
    g_ColorNameMap[Cell_Red] = "Red";
    g_ColorNameMap[Cell_Orange] = "Orange";
    g_ColorNameMap[Cell_Yellow] = "Yellow";
    g_ColorNameMap[Cell_Lime] = "Lime";
    g_ColorNameMap[Cell_Green] = "Green";
    g_ColorNameMap[Cell_Cyan] = "Cyan";
    g_ColorNameMap[Cell_Blue] = "Blue";
    g_ColorNameMap[Cell_Purple] = "Purple";
    g_ColorNameMap[Cell_Magenta] = "Magenta";
    g_ColorNameMap[Cell_Pink] = "Pink";
    g_ColorNameMap[Cell_Brown] = "Brown";
    g_ColorNameMap[Cell_Gray] = "Gray";
}

// -------------------------------- BEGIN LOGGING -------------------------------- //
void HandleQLoggingGUI(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString lMsg = "";
    switch(type)
    {
        case QtDebugMsg:
        {
            if (g_cfgVars.mbIsDebug)
            {
                lMsg.append("[Debug]: ");
                lMsg.append(msg.toLocal8Bit());
                lMsg.append(" <");
                lMsg.append(context.file);
                lMsg.append(":");
                lMsg.append(QString("%1").arg(context.line));
                lMsg.append(" @ ");
                lMsg.append(context.function);
                lMsg.append(">");
            }
            break;
        }
        case QtInfoMsg:
        {
            lMsg.append("[Info]: ");
            lMsg.append(msg.toLocal8Bit());
            break;
        }
        case QtWarningMsg:
        {
            lMsg.append("[Warning]: ");
            lMsg.append(msg.toLocal8Bit());
            break;
        }
        case QtCriticalMsg:
        {
            lMsg.append("[Error]: ");
            lMsg.append(msg.toLocal8Bit());
            break;
        }
        case QtFatalMsg:
        {
            lMsg.append("[FATAL]: ");
            lMsg.append(msg.toLocal8Bit());
            lMsg.append(" <");
            lMsg.append(context.file);
            lMsg.append(":");
            lMsg.append(context.line);
            lMsg.append(" @ ");
            lMsg.append(context.function);
            lMsg.append(">");
            break;
        }
    }

    if (!lMsg.isEmpty())
    {
        std::cout<< lMsg.toStdString()<< std::endl;
        l_logStream<< QDateTime::currentDateTimeUtc().toString("yyyy-dd-MM hh:mm:ss.z t").toStdString()<< " "<< lMsg.toStdString()<< std::endl;

        if (nullptr != pMainWnd)
        {
            pMainWnd->UpdateLog(lMsg);
        }
    }
}

void HandleQLogging(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    char* lMsg = new char[4096];
    memset(lMsg, 0, 4096);
    switch(type)
    {
        case QtDebugMsg: snprintf(lMsg, 4096, "[Debug]: %s <%s:%d @ %s>", msg.toStdString().c_str(), context.file, context.line, context.function); break;
        case QtInfoMsg: snprintf(lMsg, 4096, "[Info]: %s", msg.toStdString().c_str()); break;
        case QtWarningMsg: snprintf(lMsg, 4096, "[Warning]: %s", msg.toStdString().c_str()); break;
        case QtCriticalMsg: snprintf(lMsg, 4096, "[Error]: %s", msg.toStdString().c_str()); break;
        case QtFatalMsg: snprintf(lMsg, 4096, "[Debug]: %s <%s:%d @ %s>", msg.toStdString().c_str(), context.file, context.line, context.function); break;
    }

    if (0 < strlen(lMsg))
    {
        std::cout<< QDateTime::currentDateTimeUtc().toString("yyyy-dd-MM hh:mm:ss.z t").toStdString()<< " "<< lMsg<< std::endl;
        l_logStream<< QDateTime::currentDateTimeUtc().toString("yyyy-dd-MM hh:mm:ss.z t").toStdString()<< " "<< lMsg<< std::endl;
    }
}

void OpenLogAndPrintHeader()
{
    // Open the log.
    std::string sLogPath = g_cfgVars.msRootDir;

    sLogPath += "/logs/";
    QDir lDir(sLogPath.c_str());
    if (!lDir.exists())
    {
        lDir.mkdir(sLogPath.c_str());
    }

    if (g_cfgVars.msLogName.empty())
    {
        g_cfgVars.msLogName = g_cfgVars.msProgName;
    }

    if (g_cfgVars.msLogName.find(".log") == std::string::npos)
    {
        if (g_cfgVars.mbIsDebug) { g_cfgVars.msLogName += "-runtime_DEBUG.log"; }
        else { g_cfgVars.msLogName += "-runtime.log"; }
    }

    l_fileBuff.open(sLogPath + g_cfgVars.msLogName, std::ios::out);

    QString lHeader = "+================\n"
                      ">    BEGIN ColorWars %1 LOG\n"
                      ">    Version: %2.%3.%4\n"
                      ">    Launch Date: %5\n"
                      ">................\n"
                      ">    System Information:\n"
                      ">        Kernal: %6\n"
                      ">        Architecture: %7\n"
                      ">        OS: %8\n"
                      ">        Hostname: %9\n"
                      ">................\n"
                      ">    DEBUG: %10\n"
                      "+================";

    lHeader = lHeader.arg(VER_STAGE).arg(VER_MAJOR).arg(VER_MINOR).arg(VER_PATCH);
    lHeader = lHeader.arg(QDateTime::currentDateTimeUtc().toString("yyyy-dd-MM hh:mm:ss.z t"));
    lHeader = lHeader.arg(QSysInfo::kernelVersion());
    lHeader = lHeader.arg(QSysInfo::currentCpuArchitecture());
    lHeader = lHeader.arg(QSysInfo::prettyProductName());
    lHeader = lHeader.arg(QSysInfo::machineHostName());

    if (g_cfgVars.mbIsDebug) { lHeader = lHeader.arg("TRUE"); }
    else { lHeader = lHeader.arg("FALSE"); }

    std::cout<< lHeader.toStdString()<< std::endl;
    l_logStream<< lHeader.toStdString()<< std::endl;
}
// -------------------------------- END LOGGING -------------------------------- //

char* UpdateProcName(char* pCurrName)
{
    const size_t uMaxSz = 4096;
    const std::string sOurName = "ColorWars";

    std::string sPlatform = QSysInfo::productType().toStdString();
    std::string sVersion = QSysInfo::productVersion().toStdString();
    std::string sArch = QSysInfo::buildCpuArchitecture().toStdString();

    char* pNewName = new char[uMaxSz];
    memset(pNewName, 0, uMaxSz);

    // Example: ColorWars Ubuntu 18.04 [x86_64] (/usr/bin/ColorWars)
    snprintf(pNewName, uMaxSz, "ColorWars %s %s [%s] (%s)", sPlatform.c_str(), sVersion.c_str(), sArch.c_str(), pCurrName);

    return pNewName;
}

// -------------------------------- BEGIN GAME LOOP -------------------------------- //
void RunGame(CMainWindow* pWnd = nullptr)
{
    if (nullptr != mpGame && mpGame->IsPlaying())
    {
        // Here we would POLL any network sockets.

        // Here we would apply results from the POLL to the game as needed.

        if (nullptr != pWnd) { pWnd->repaint(); }
    }
}

void AutoGame(CMainWindow* pWnd = nullptr)
{
    if (nullptr != mpGame && mpGame->IsPlaying())
    {
        std::vector<ECellColors> vRandClr = {Cell_Red, Cell_Orange, Cell_Yellow, Cell_Lime, Cell_Green,
                                             Cell_Cyan, Cell_Blue, Cell_Purple, Cell_Magenta, Cell_Pink,
                                             Cell_Brown, Cell_Gray};

        ECellColors eAggressor = Cell_Red;
        ECellColors eVictim = Cell_White;
        do
        {
            u32 uRand = floor(rand());
            u32 uRandIdx = uRand % vRandClr.size();
            eAggressor = vRandClr[uRandIdx];
        }while (!mpGame->NationExists(eAggressor));

        do
        {
            u32 uRand = floor(rand());
            u32 uRandIdx = uRand % vRandClr.size();
            eVictim = (mpGame->NationExists(Cell_White)) ? Cell_White : vRandClr[uRandIdx];
        }while (!mpGame->NationExists(eVictim));

//        eAggressor = Cell_Green;

        mpGame->Play(eAggressor, eVictim);

        if (nullptr != pWnd) { pWnd->repaint(); }
    }
}

void StopGameTimer()
{
    if (nullptr != mpTicker)
    {
        mpTicker->stop();
    }
}
// -------------------------------- END GAME LOOP -------------------------------- //

int main(int argc, char *argv[])
{
    bool bUseGui = true;
    bool bShouldRun = true;

    // Setup the directory.
    g_cfgVars.msRootDir = argv[0];
    size_t iLastIdx = g_cfgVars.msRootDir.find_last_of('/');
    if (std::string::npos == iLastIdx)
    {
        iLastIdx = g_cfgVars.msRootDir.find_last_of('\\');
        if (std::string::npos == iLastIdx)
        {
            fprintf(stderr, "ERR: Unable to locate program root! Defaulting to \"./\"!\n");
        }
    }
    g_cfgVars.msRootDir.erase(g_cfgVars.msRootDir.begin() + iLastIdx, g_cfgVars.msRootDir.end());

    // Attempt to update the process name.
    argv[0] = UpdateProcName(argv[0]);

    // Iterate over the arguments and see if they're anything we're interested in.
    for (int iIdx = 1; iIdx < argc; ++iIdx)
    {
        if (!strcmp("-d", argv[iIdx]) || !strcmp("--debug", argv[iIdx]))
        {
            g_cfgVars.mbIsDebug = true;
        }
        else if (!strcmp("-h", argv[iIdx]) || !strcmp("--help", argv[iIdx]))
        {
            printf("ColorWars [%s]\n"
                   "A game for Discord!\n\n"
                   "Usage:\n\tColorWars [switches]\n\n"
                   "Switches:\n\t"
                   "-d,--debug\t-\tShow debugging messages.\n\t"
                   "-h,--help\t-\tShow this help\n\t"
                   "-n,--nogui\t-\tDon't show a GUI (for servers).\n\n"
                   "(c) 2018 SquigglePuff Jr.\n"
                   "Version: %d.%d.%d\n", VER_STAGE, VER_MAJOR, VER_MINOR, VER_PATCH);
            bShouldRun = false;
        }
        else if (!strcmp("-n", argv[iIdx]) || !strcmp("--nogui", argv[iIdx]))
        {
            bUseGui = false;
        }
    }

    int iRtnCode = 0;
    if (bShouldRun)
    {
        OpenLogAndPrintHeader();

        SetupColorNames();

        QApplication a(argc, argv);
        pMainWnd = new CMainWindow();

        // Canvas properties.
        u32 uCellSz = 128;
        SPoint lCenter(1024, 1024);

        mpGame = new CGame();
        mpGame->SetDiceMax(0xff);
        mpGame->SetCellSize(uCellSz);
        mpGame->SetCanvasCenter(lCenter);

        mpTicker = new QTimer();
        mpTicker->connect(mpTicker, &QTimer::timeout, [&]{ AutoGame(pMainWnd); });

        // Setup the game callbacks.
        mpGame->SetStoppedCallback(&StopGameTimer);

        if (bUseGui)
        {
            qInstallMessageHandler(HandleQLoggingGUI);
            pMainWnd->SetGamePtr(mpGame);
            pMainWnd->SetTickPtr(mpTicker);
            pMainWnd->Setup();
            pMainWnd->show();
        }
        else
        {
            qInstallMessageHandler(HandleQLogging);
        }

        if (g_cfgVars.mbIsDebug) { qDebug("Debugging enabled!"); }

        iRtnCode = a.exec();

        // -------------------------------- BEGIN LOGGING -------------------------------- //
        std::string sCloseMsg = "+================\n"
                                "> CLOSED ColorWars LOG\n"
                                "+================";
        std::cout<< sCloseMsg<< std::endl;
        l_logStream<< sCloseMsg<< std::endl;

        l_fileBuff.close();
        // -------------------------------- END LOGGING -------------------------------- //
    }

    return iRtnCode;
}
