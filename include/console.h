#ifndef CONSOLE_H
#define CONSOLE_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>

#include "include/globals.h"

class CConsole : public QDialog
{
    Q_OBJECT
public:
    explicit CConsole(QWidget *pParent = nullptr);
    virtual ~CConsole();

    void Setup();

    int GetLines();

protected:
    void keyReleaseEvent(QKeyEvent *event);

public slots:
    void NewLog(QString sLine);

signals:
    void Command(SCommand aCmd);

private:
    QListWidget *mpLog; //!< The list widget that contains the game log.
    QLineEdit *mpCmdPrompt; //!< The prompt to type commands into.
};

#endif // CONSOLE_H
