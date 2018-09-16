#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>
#include <QAction>
#include <QLabel>
#include <QListWidget>
#include <QTimer>

#include "include/game.h"

#define NUM_MENU_ITEMS 4

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

    void Setup();

    void SetGamePtr(CGame* pGame = nullptr);
    void SetTickPtr(QTimer* pTimer = nullptr);

protected:
    void paintEvent(QPaintEvent *apEvent);
    void contextMenuEvent(QContextMenuEvent *apEvent);

private slots:
    void playGame(bool);
    void pauseGame(bool);
    void stopGame(bool);

private:
    void SetupUI();

    CGame *mpGame;
    QTimer *mpTicker;
    QImage *mpCanvas;

    // GUI elements.
    QLabel* mpGameCanvas;
    QListWidget *mpChatLog;
    QList<QAction*> mlActions;
};

extern QList<QString> g_LogList;

#endif // MAINWINDOW_H
