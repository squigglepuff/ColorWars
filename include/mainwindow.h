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

    void UpdateLog(QString lMsg);

protected:
    void paintEvent(QPaintEvent *apEvent);

public slots:
    void RunCommand(const char* sCmd);

private:
    void SetupUI();

    CGame *mpGame;
    QLabel* mpGameCanvas;
    CConsole *mpConsole;
};

#endif // MAINWINDOW_H
