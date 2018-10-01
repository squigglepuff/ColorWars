#include <QtGui>
#include <QMenu>
#include <QHBoxLayout>
#include "include/mainwindow.h"

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent), mpGame{nullptr}, mpTicker{nullptr}, mpCanvas{nullptr}
{
    // Intentionally left blank.
}

CMainWindow::~CMainWindow()
{
//    if (nullptr != mpGame) { mpGame->Destroy(); }
}

void CMainWindow::Setup()
{
    // Setup the game.
//    mpGame = new CGame();

    // Setup the Window GUI.
    SetupUI();

    // Set the window properties.
    char *pWndTitle = new char[4096];
    memset(pWndTitle, 0, 4096);

//#if defined(Q_OS_UNIX)
//    snprintf(pWndTitle, 4095, "Color Wars %s [Version: %d.%d.%d] (Build: %d)", VER_STAGE, VER_MAJOR, VER_MINOR, VER_PATCH, BUILD);
//#else
    snprintf(pWndTitle, 4095, "Color Wars %s [Version: %d.%d.%d] (Build: %d)", VER_STAGE, VER_MAJOR, VER_MINOR, VER_PATCH, 0);
//#endif // #if defined(BUILD)

    setWindowTitle(QString::fromLatin1(pWndTitle));
    resize(1280, 1024);

    if (nullptr != pWndTitle) { delete[] pWndTitle; }

    stopGame(false);
}

void CMainWindow::SetGamePtr(CGame* pGame)
{
    mpGame = pGame;
}

void CMainWindow::SetTickPtr(QTimer* pTimer)
{
    mpTicker = pTimer;
}

void CMainWindow::UpdateLog(QString lMsg)
{
    if (nullptr != mpConsole)
    {
        mpConsole->NewLog(lMsg);
    }
}

void CMainWindow::paintEvent(QPaintEvent *apEvent)
{
    if (nullptr != apEvent)
    {
        if (nullptr != mpGame && nullptr != mpGame->GetCanvas())
        {
            QImage qImg = mpGame->GetCanvas()->scaled(mpGameCanvas->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            mpGameCanvas->setPixmap(QPixmap::fromImage(qImg));
        }
    }
    else
    {
        qCritical("apEvent for paintEvent is a nullptr! Aborting paint!");
    }
}

void CMainWindow::contextMenuEvent(QContextMenuEvent *apEvent)
{
    if (nullptr != apEvent && apEvent->reason() == QContextMenuEvent::Mouse)
    {
        QPoint qTransPos = mapToGlobal(apEvent->pos());

        QMenu qMenu;
        qMenu.addActions(mlActions);
        qMenu.setGeometry(qTransPos.x(), qTransPos.y(), qMenu.width(), qMenu.height());
        qMenu.exec();
    }
}

void CMainWindow::startGame(bool)
{
    if (!mpGame->IsPlaying())
    {
        mpGame->NewGame();
    }

    mlActions.at(1)->setText(tr("Play Game"));
    mlActions.at(0)->setEnabled(false);
    mlActions.at(1)->setEnabled(true);
    mlActions.at(2)->setEnabled(false);
    mlActions.at(3)->setEnabled(false);

    repaint();
}

void CMainWindow::playGame(bool)
{
    mpTicker->start(250);
    mlActions.at(0)->setEnabled(false);
    mlActions.at(1)->setEnabled(false);
    mlActions.at(2)->setEnabled(true);
    mlActions.at(3)->setEnabled(true);

    repaint();
}

void CMainWindow::pauseGame(bool)
{
    mpTicker->stop();
    mlActions.at(1)->setText(tr("Resume Game"));
    mlActions.at(0)->setEnabled(false);
    mlActions.at(1)->setEnabled(true);
    mlActions.at(2)->setEnabled(false);
    mlActions.at(3)->setEnabled(true);
}

void CMainWindow::stopGame(bool)
{
    mpTicker->stop();
    mlActions.at(1)->setText(tr("Play Game"));
    mlActions.at(0)->setEnabled(true);
    mlActions.at(1)->setEnabled(false);
    mlActions.at(2)->setEnabled(false);
    mlActions.at(3)->setEnabled(false);

    if (mpGame->IsPlaying())
    {
        mpGame->EndGame();
    }

    repaint();
}

void CMainWindow::RunCommand(SCommand aCmd)
{
    if (nullptr != mpConsole)
    {
        QString sCmd = "";
        switch (aCmd.meCmd)
        {
            case Cmd_NewGame:
            {
                if (!mpGame->IsPlaying())
                {
                    startGame(false);
                }
                else
                {
                    qCritical("Refusing to start an already playing game!");
                }
                sCmd = "New Game";
                break;
            }
            case Cmd_PlayGame:
            {
                if (mpGame->IsSetup())
                {
                    playGame(false);
                }
                else
                {
                    qCritical("Unable to play an empty game! Please run \"!new\".");
                }
                sCmd = "Play Game";
                break;
            }
            case Cmd_PauseGame:
            {
                if (mpGame->IsPlaying())
                {
                    pauseGame(false);
                }
                else
                {
                    qCritical("Unable to pause a stopped game!");
                }
                sCmd = "Pause Game";
                break;
            }
            case Cmd_StopGame:
            {
                stopGame(false);
                sCmd = "Stop Game";
                break;
            }
            case Cmd_Move:
            {
                // Does nothing for now!
                qWarning("This isn't implemented yet!");
                sCmd = "Move";
                break;
            }
            case Cmd_Redraw:
            {
                if (nullptr != mpGame) { mpGame->Draw(); }
                sCmd = "Redraw";
                break;
            }
            case Cmd_Help:
            {
                qInfo("Discord Commands:\n"
                      "!move <color1> <color2>  -  Move a color1 to color2.\n"
                      "!new     -  Run a new game.\n"
                      "!redraw  -  Redraw the board.\n"
                      "!stats <color>  -  Give a color nation's stats.\n\n"
                      "CLI Commands:\n"
                      "/auto  -  Play the current game.\n"
                      "/help  -  Show this help.\n"
                      "/pause -  Pause the current game.\n"
                      "/quit  -  Quits the application.\n"
                      "/stop  -  Stop/End the current game.");
                sCmd = "Help";
                break;
            }
            case Cmd_Quit:
            {
                close();
                sCmd = "Close Application";
                break;
            }
            case Cmd_Stats:
            {
                if (0 < aCmd.mvArgs.size())
                {
                    // Locate the nation's color.
                    // g_NameToColorMap
                    QString sClr = QString::fromStdString(aCmd.mvArgs[0]);
                    sClr = sClr.toLower(); // Lower-case it.
                    sClr[0] = sClr[0].toLatin1() - ' '; // Capitalize the first letter.

                    mpGame->PrintNationStats(g_NameToColorMap[sClr.toStdString()]);
                }
                sCmd = "Nation Stats";
                break;
            }
            default:
            {
                qCritical("Unknown or Invalid command! See \"/help\".");
                sCmd = "UNKNOWN/INVLAID";
                break;
            }
        }

        mpConsole->NewLog(tr("[CMD]: <<< %1").arg(sCmd));
        repaint();
    }
}

void CMainWindow::SetupUI()
{
    // Setup the central widget first.
    QWidget *pCentralWidget = new QWidget(this);
    setCentralWidget(pCentralWidget);

    mpGameCanvas = new QLabel();
    if (nullptr != mpGame && nullptr != mpGame->GetCanvas())
    {
        QImage qImg = mpGame->GetCanvas()->scaled(mpGameCanvas->size(), Qt::KeepAspectRatio);
        mpGameCanvas->setPixmap(QPixmap::fromImage(qImg));
    }
    else
    {
        mpGameCanvas->setPixmap(QPixmap());
    }

//    mpGameCanvas->setScaledContents(true);
    mpGameCanvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mpGameCanvas->setMaximumSize(1024, 1024);

    // Setup the layout.
    QHBoxLayout *pLay = new QHBoxLayout();
    pLay->addWidget(mpGameCanvas);

    pCentralWidget->setLayout(pLay);

    // Setup the menu stuff.
    mlActions.append(new QAction(tr("New Game")));
    mlActions[0]->setText(tr("New Game"));
    mlActions[0]->setEnabled(true);
    connect(mlActions[0], &QAction::triggered, this, &CMainWindow::startGame);

    mlActions.append(new QAction(tr("Play Game")));
    mlActions[1]->setText(tr("Play Game"));
    mlActions[1]->setEnabled(false);
    connect(mlActions[1], &QAction::triggered, this, &CMainWindow::playGame);

    mlActions.append(new QAction(tr("Pause Game")));
    mlActions[2]->setText(tr("Pause Game"));
    mlActions[2]->setEnabled(false);
    connect(mlActions[2], &QAction::triggered, this, &CMainWindow::pauseGame);

    mlActions.append(new QAction(tr("Stop Game")));
    mlActions[3]->setText(tr("Stop Game"));
    mlActions[3]->setEnabled(false);
    connect(mlActions[3], &QAction::triggered, this, &CMainWindow::stopGame);

    // Set the window icon.
    setWindowIcon(QIcon(tr(":/ICO_APP")));

    // Setup the console window.
    mpConsole = new CConsole(this);
    mpConsole->Setup();
    mpConsole->show();

    connect(mpConsole, &CConsole::Command, this, &CMainWindow::RunCommand);
}
