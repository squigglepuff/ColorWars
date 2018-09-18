#include "include/mainwindow.h"
#include <QApplication>

std::map<ECellColors, QString> g_ColorNameMap;
QList<QString> g_LogList;

CGame *mpGame;
QTimer *mpTicker;

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

void HandleQLoggingGUI(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString lMsg = "";
    switch(type)
    {
        case QtDebugMsg:
        {
            lMsg = "[Debug]: ";
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
        case QtInfoMsg:
        {
            lMsg = "[Info]: ";
            lMsg.append(msg.toLocal8Bit());
            break;
        }
        case QtWarningMsg:
        {
            lMsg = "[Warning]: ";
            lMsg.append(msg.toLocal8Bit());
            break;
        }
        case QtCriticalMsg:
        {
            lMsg = "[Error]: ";
            lMsg.append(msg.toLocal8Bit());
            break;
        }
        case QtFatalMsg:
        {
            lMsg = "[FATAL]: ";
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

    fprintf(stdout, "%s\n", lMsg.toStdString().c_str());
    g_LogList.append(lMsg);
}

void HandleQLogging(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch(type)
    {
        case QtDebugMsg: printf("[Debug]: %s <%s:%d @ %s>\n", msg.toStdString().c_str(), context.file, context.line, context.function); break;
        case QtInfoMsg: printf("[Info]: %s\n", msg.toStdString().c_str()); break;
        case QtWarningMsg: printf("[Warning]: %s\n", msg.toStdString().c_str()); break;
        case QtCriticalMsg: printf("[Error]: %s\n", msg.toStdString().c_str()); break;
        case QtFatalMsg: printf("[Debug]: %s <%s:%d @ %s>\n", msg.toStdString().c_str(), context.file, context.line, context.function); break;
    }
}

void TickGame(CMainWindow* pWnd = nullptr)
{
    if (nullptr != mpGame)
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

        mpGame->Play(eAggressor, eVictim);

        if (nullptr != pWnd) { pWnd->repaint(); }
    }
}

int main(int argc, char *argv[])
{
    bool bUseGui = true;
    bool bShowDebug = false;
    bool bShouldRun = true;

    // Iterate over the arguments and see if they're anything we're interested in.
    for (int iIdx = 1; iIdx < argc; ++iIdx)
    {
        if (!strcmp("-d", argv[iIdx]) || !strcmp("--debug", argv[iIdx]))
        {
            bShowDebug = true;
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
        SetupColorNames();

        QApplication a(argc, argv);
        CMainWindow w;

        mpGame = new CGame();

        mpTicker = new QTimer();
        mpTicker->connect(mpTicker, &QTimer::timeout, [&]{ TickGame(&w); });

        if (bUseGui)
        {
            qInstallMessageHandler(HandleQLoggingGUI);
            w.SetGamePtr(mpGame);
            w.SetTickPtr(mpTicker);
            w.Setup();
            w.show();
        }
        else
        {
            qInstallMessageHandler(HandleQLogging);
        }

        iRtnCode = a.exec();
    }

    return iRtnCode;
}
