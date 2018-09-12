#include <QtGui>
#include <QMenu>
#include "include/mainwindow.h"

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent)
{
    const u32 c_iWndSz = 1024;
    QPointF qCenter(c_iWndSz / 2.0, c_iWndSz / 2.0);

    // Setup the board.
    mpBoard = new CBoard();
    mpBoard->SetBoardSize(2);
    mpBoard->Create(64, qCenter);

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
}

CMainWindow::~CMainWindow()
{
    mpBoard->Destroy();
}

void CMainWindow::paintEvent(QPaintEvent *apEvent)
{
    if (nullptr != apEvent)
    {
        QPainter *pPainter = new QPainter();
        pPainter->begin(this);
        mpBoard->Draw(pPainter);
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
