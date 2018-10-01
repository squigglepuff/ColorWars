#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>
#include <QAction>
#include <QLabel>
#include <QTimer>

#include "include/console.h"
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

    void UpdateLog(QString lMsg);

protected:
    void paintEvent(QPaintEvent *apEvent);
    void contextMenuEvent(QContextMenuEvent *apEvent);

public slots:
    void startGame(bool);
    void playGame(bool);
    void pauseGame(bool);
    void stopGame(bool);

    void RunCommand(SCommand aCmd);

private:
    void SetupUI();

    CGame *mpGame;
    QTimer *mpTicker;
    QImage *mpCanvas;

    CConsole *mpConsole;

    // GUI elements.
    QLabel* mpGameCanvas;
    QListWidget *mpChatLog;
    QList<QAction*> mlActions;
};

extern QList<QString> g_LogList;

#endif // MAINWINDOW_H
