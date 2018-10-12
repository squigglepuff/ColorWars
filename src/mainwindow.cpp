#include <QtGui>
#include <QMenu>
#include <QHBoxLayout>
#include "include/mainwindow.h"

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent), mpGame{nullptr}, mpGameCanvas{nullptr}, mpConsole{nullptr}
{
    // Intentionally left blank.
}

CMainWindow::~CMainWindow()
{
    // Intentionally left blank.
}

void CMainWindow::Setup()
{
    // Setup the Window GUI.
    SetupUI();

    // Set the window properties.
    char *pWndTitle = new char[4096];
    memset(pWndTitle, 0, 4096);

    snprintf(pWndTitle, 4095, "Color Wars %s [Version: %d.%d.%d] (Build: %d)", VER_STAGE, VER_MAJOR, VER_MINOR, VER_PATCH, 0);

    setWindowTitle(QString::fromLatin1(pWndTitle));
    resize(1280, 1024);

    if (nullptr != pWndTitle) { delete[] pWndTitle; }
}

void CMainWindow::SetGamePtr(CGame* pGame)
{
    mpGame = pGame;

    if (nullptr != mpGame && nullptr != mpConsole)
    {
        static const QMetaMethod c_CmdSignal = QMetaMethod::fromSignal(&CConsole::Command);
        if (!isSignalConnected(c_CmdSignal))
        {
            connect(mpConsole, &CConsole::Command, mpGame, &CGame::ProcessCommand);
        }

        connect(mpGame, &CGame::SendGUI_Command, this, &CMainWindow::RunCommand);
        connect(mpGame, &CGame::SendGUI_Redraw, [&]{ repaint(); });
        connect(mpGame, &CGame::SendGUI_Quit, this, &CMainWindow::close);
    }
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

void CMainWindow::RunCommand(const char *sCmd)
{
    if (nullptr != mpConsole)
    {
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

    mpGameCanvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mpGameCanvas->setMaximumSize(1024, 1024);

    // Setup the layout.
    QHBoxLayout *pLay = new QHBoxLayout();
    pLay->addWidget(mpGameCanvas);

    pCentralWidget->setLayout(pLay);

    // Set the window icon.
    setWindowIcon(QIcon(tr(":/ICO_APP")));

    // Setup the console window.
    mpConsole = new CConsole(this);
    mpConsole->Setup();
    mpConsole->show();

    if (nullptr != mpGame)
    {
        connect(mpConsole, &CConsole::Command, mpGame, &CGame::ProcessCommand);
        connect(mpGame, &CGame::SendGUI_Command, this, &CMainWindow::RunCommand);
        connect(mpGame, &CGame::SendGUI_Redraw, [&]{ repaint(); });
        connect(mpGame, &CGame::SendGUI_Quit, this, &CMainWindow::close);
    }
}
