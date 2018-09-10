#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>
#include <QAction>

#include "include/board.h"

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

protected:
    void paintEvent(QPaintEvent *apEvent);
    void contextMenuEvent(QContextMenuEvent *apEvent);

protected slots:
    void SetHexColor(QAction *apAction);

private:
    CHoneyComb *mpComb;
    QList<QAction*> mlActions;
    QMenu *mpCtxMenu;
    CCell* mpSelectedCell;
};

#endif // MAINWINDOW_H
