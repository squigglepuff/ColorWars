#include <QtGui>
#include <QMenu>
#include <QTimer>
#include "include/mainwindow.h"

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent), mpGame{nullptr}, mpTicker{nullptr}
{
    const u32 c_iWndSz = 1024;
    SPoint qCenter(c_iWndSz / 2.0, c_iWndSz / 2.0);

    // Setup the game.
    mpGame = new CGame();
    mpGame->NewGame(0xffffffff, 64, qCenter);

    // Set the window properties.
    char *pWndTitle = new char[4096];
    memset(pWndTitle, 0, 4096);

#if defined(Q_OS_UNIX)
    snprintf(pWndTitle, 4095, "Color Wars %s [Version: %d.%d.%d] (Build: %d)", VER_STAGE, VER_MAJOR, VER_MINOR, VER_PATCH, BUILD);
#else
    snprintf(pWndTitle, 4095, "Color Wars %s [Version: %d.%d.%d] (Build: %d)", VER_STAGE, VER_MAJOR, VER_MINOR, VER_PATCH, 0);
#endif // #if defined(BUILD)

    setWindowTitle(QString::fromLatin1(pWndTitle));
    resize(c_iWndSz, c_iWndSz);

    if (nullptr != pWndTitle) { delete[] pWndTitle; }

    // Setup the timer.
    mpTicker = new QTimer();
    connect(mpTicker, &QTimer::timeout, this, &CMainWindow::tick);
    mpTicker->start(1000);
}

CMainWindow::~CMainWindow()
{
    if (nullptr != mpGame) { mpGame->Destroy(); }
}

void CMainWindow::paintEvent(QPaintEvent *apEvent)
{
    if (nullptr != apEvent)
    {
        QPainter *pPainter = new QPainter();
        pPainter->begin(this);
        mpGame->Draw(pPainter);
        pPainter->end();

        if (nullptr != pPainter) { delete pPainter; }
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
        // code
    }
}

void CMainWindow::tick()
{
    if (nullptr != mpGame)
    {
        mpGame->Play(Cell_Red, Cell_White);
        repaint();
    }
}
