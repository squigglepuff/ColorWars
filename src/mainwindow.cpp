#include <QtGui>
#include <QMenu>
#include "include/mainwindow.h"

CMainWindow::CMainWindow(QWidget *parent) : QMainWindow(parent), mpComb{nullptr}, mpSelectedCell{nullptr}
{
    const u32 c_iWndSz = 768;
    QPointF qCenter(c_iWndSz / 2.0, c_iWndSz / 2.0);

    // Setup the honeycomb.
    mpComb = new CHoneyComb();
    mpComb->SetPosition(qCenter);
    mpComb->SetCellSize(256);

    mpComb->GetCells()[0].SetColor(Cell_White);
    mpComb->GetCells()[1].SetColor(Cell_Red);
    mpComb->GetCells()[2].SetColor(Cell_Orange);
    mpComb->GetCells()[3].SetColor(Cell_Yellow);
    mpComb->GetCells()[4].SetColor(Cell_Lime);
    mpComb->GetCells()[5].SetColor(Cell_Green);
    mpComb->GetCells()[6].SetColor(Cell_Cyan);

    // Add the new entries.
    mlActions.append(new QAction(tr("White")));
    mlActions.append(new QAction(tr("Red")));
    mlActions.append(new QAction(tr("Orange")));
    mlActions.append(new QAction(tr("Yellow")));
    mlActions.append(new QAction(tr("Lime")));
    mlActions.append(new QAction(tr("Green")));
    mlActions.append(new QAction(tr("Cyan")));
    mlActions.append(new QAction(tr("Blue")));
    mlActions.append(new QAction(tr("Purple")));
    mlActions.append(new QAction(tr("Magenta")));
    mlActions.append(new QAction(tr("Pink")));

    // Set the window properties.
    setWindowTitle(tr("Hexagon Draw Test"));
    resize(c_iWndSz, c_iWndSz);
}

CMainWindow::~CMainWindow()
{
    // Intentionally left blank.
}

void CMainWindow::paintEvent(QPaintEvent *apEvent)
{
    if (nullptr != apEvent && nullptr != mpComb)
    {
        QPainter *pPainter = new QPainter();
        pPainter->begin(this);
        mpComb->Draw(pPainter);
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
        if (nullptr != mpComb)
        {
            // See if we're in a hexagon.
            for (CCell* pIter = mpComb->GetCells(); nullptr != pIter; ++pIter)
            {
                if ((*pIter).PointInHex(apEvent->pos()))
                {
                    // Show a context menu!
                    mpSelectedCell = pIter;

                    // Create the context menu.
                    QMenu qMenu;
                    qMenu.addActions(mlActions);

                    // Connect up the menu.
                    connect(&qMenu, &QMenu::triggered, this, &CMainWindow::SetHexColor);

                    // Show the menu.
                    QPoint translatedPos = mapToGlobal(apEvent->pos());
                    qMenu.setGeometry(translatedPos.x(), translatedPos.y(), qMenu.width(), qMenu.height());
                    qMenu.exec();

                    break;
                }
            }
        }
    }
}

void CMainWindow::SetHexColor(QAction *apAction)
{
    if (nullptr != apAction && nullptr != mpComb && nullptr != mpSelectedCell)
    {
        if (0 == apAction->text().compare("White")) { (*mpSelectedCell).SetColor(Cell_White); }
        else if (0 == apAction->text().compare("Red")) { (*mpSelectedCell).SetColor(Cell_Red); }
        else if (0 == apAction->text().compare("Orange")) { (*mpSelectedCell).SetColor(Cell_Orange); }
        else if (0 == apAction->text().compare("Yellow")) { (*mpSelectedCell).SetColor(Cell_Yellow); }
        else if (0 == apAction->text().compare("Lime")) { (*mpSelectedCell).SetColor(Cell_Lime); }
        else if (0 == apAction->text().compare("Green")) { (*mpSelectedCell).SetColor(Cell_Green); }
        else if (0 == apAction->text().compare("Cyan")) { (*mpSelectedCell).SetColor(Cell_Cyan); }
        else if (0 == apAction->text().compare("Blue")) { (*mpSelectedCell).SetColor(Cell_Blue); }
        else if (0 == apAction->text().compare("Purple")) { (*mpSelectedCell).SetColor(Cell_Purple); }
        else if (0 == apAction->text().compare("Magenta")) { (*mpSelectedCell).SetColor(Cell_Magenta); }
        else if (0 == apAction->text().compare("Pink")) { (*mpSelectedCell).SetColor(Cell_Pink); }
        else { (*mpSelectedCell).SetColor(Cell_White); }

        mpSelectedCell = nullptr;

        // Redraw the polys.
        repaint();
    }
}
