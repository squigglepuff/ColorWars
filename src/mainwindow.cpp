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

void CMainWindow::paintEvent(QPaintEvent *apEvent)
{
    if (nullptr != apEvent)
    {
        if (nullptr != mpGame && nullptr != mpGame->GetCanvas())
        {
            QImage qImg = mpGame->GetCanvas()->scaled(mpGameCanvas->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            mpGameCanvas->setPixmap(QPixmap::fromImage(qImg));
        }

        for (int iIdx = mpChatLog->count(); iIdx < g_LogList.size(); ++iIdx)
        {
            if (g_LogList[iIdx].contains("[Debug]: "))
            {
                QListWidgetItem *pItem = new QListWidgetItem(QIcon(tr(":/ICO_DEBUG")), g_LogList[iIdx].remove("[Debug]: "));
                mpChatLog->addItem(pItem);
            }
            else if (g_LogList[iIdx].contains("[Info]: "))
            {
                QListWidgetItem *pItem = new QListWidgetItem(QIcon(tr(":/ICO_INFO")), g_LogList[iIdx].remove("[Info]: "));
                mpChatLog->addItem(pItem);
            }
            else if (g_LogList[iIdx].contains("[Warning]: "))
            {
                QListWidgetItem *pItem = new QListWidgetItem(QIcon(tr(":/ICO_WARN")), g_LogList[iIdx].remove("[Warning]: "));
                mpChatLog->addItem(pItem);
            }
            else if (g_LogList[iIdx].contains("[Error]: "))
            {
                QListWidgetItem *pItem = new QListWidgetItem(QIcon(tr(":/ICO_ERR")), g_LogList[iIdx].remove("[Error]: "));
                mpChatLog->addItem(pItem);
            }
            mpChatLog->scrollToBottom();
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

void CMainWindow::playGame(bool)
{
    mpTicker->start(500);
    mlActions.at(0)->setText(tr("Start Game"));
    mlActions.at(0)->setEnabled(false);
    mlActions.at(1)->setEnabled(true);
    mlActions.at(2)->setEnabled(true);
}

void CMainWindow::pauseGame(bool)
{
    mpTicker->stop();
    mlActions.at(0)->setText(tr("Resume Game"));
    mlActions.at(0)->setEnabled(true);
    mlActions.at(1)->setEnabled(false);
    mlActions.at(2)->setEnabled(true);
}

void CMainWindow::stopGame(bool)
{
    const u32 c_iCanvasSz = 2048;
    SPoint qCenter(c_iCanvasSz / 2.0, c_iCanvasSz / 2.0);

    mpTicker->stop();
    mlActions.at(0)->setText(tr("Start Game"));
    mlActions.at(0)->setEnabled(true);
    mlActions.at(1)->setEnabled(false);
    mlActions.at(2)->setEnabled(false);

    mpGame->EndGame();
    mpGame->NewGame(0xff, 128, qCenter);
}

void CMainWindow::SetupUI()
{
    // Setup the central widget first.
    QWidget *pCentralWidget = new QWidget(this);
    setCentralWidget(pCentralWidget);

    // Setup the log widget.
    mpChatLog = new QListWidget();
    mpChatLog->setMaximumWidth(rect().width() / 2.5);
    mpChatLog->setMinimumWidth(rect().width() / 8);
    mpChatLog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mpChatLog->setToolTip(tr("Console Log"));
    mpChatLog->setAutoScroll(true);

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
    pLay->addWidget(mpChatLog);

    pCentralWidget->setLayout(pLay);

    // Setup the menu stuff.
    mlActions.append(new QAction(tr("Play Game")));
    mlActions[0]->setText(tr("Start Game"));
    mlActions[0]->setEnabled(true);
    connect(mlActions[0], &QAction::triggered, this, &CMainWindow::playGame);

    mlActions.append(new QAction(tr("Pause Game")));
    mlActions[1]->setText(tr("Pause Game"));
    mlActions[1]->setEnabled(false);
    connect(mlActions[1], &QAction::triggered, this, &CMainWindow::pauseGame);

    mlActions.append(new QAction(tr("Stop Game")));
    mlActions[2]->setText(tr("Stop Game"));
    mlActions[2]->setEnabled(false);
    connect(mlActions[2], &QAction::triggered, this, &CMainWindow::stopGame);

    // Set the window icon.
    setWindowIcon(QIcon(tr(":/ICO_APP")));
}
