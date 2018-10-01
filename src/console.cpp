#include <QVBoxLayout>
#include <QKeyEvent>
#include "include/console.h"

CConsole::CConsole(QWidget *pParent) : QDialog{pParent}, mpLog{nullptr}, mpCmdPrompt{nullptr}
{
    // Intentionally left blank.
}

CConsole::~CConsole()
{
    // Intentionally left blank.
}

void CConsole::keyReleaseEvent(QKeyEvent *event)
{
    if (nullptr != event && (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return))
    {
        if (nullptr != mpCmdPrompt && mpCmdPrompt->isActiveWindow())
        {
            QString lStr = mpCmdPrompt->text();
            mpCmdPrompt->clear();

            // Chop off the front element as it's the actual command.
            SCommand lCmd;

            QString lCmdStr = lStr;
            if (lStr.contains(' '))
            {
                lCmdStr = lStr.mid(0, lStr.indexOf(' '));
                lStr.remove(0, lStr.indexOf(' ')+1);

                while (!lStr.isEmpty())
                {
                    if (lStr.contains(' '))
                    {
                        lCmd.mvArgs.push_back(lStr.mid(0, lStr.indexOf(' ')).toStdString());
                        lStr.remove(0, lStr.indexOf(' ')+1);
                    }
                    else
                    {
                        lCmd.mvArgs.push_back(lStr.toStdString());
                        break;
                    }
                }
            }

            if (0 == lCmdStr.compare("!new", Qt::CaseInsensitive))
            {
                lCmd.meCmd = Cmd_NewGame;
            }
            else if (0 == lCmdStr.compare("/auto", Qt::CaseInsensitive))
            {
                lCmd.meCmd = Cmd_PlayGame;
            }
            else if (0 == lCmdStr.compare("/pause", Qt::CaseInsensitive))
            {
                lCmd.meCmd = Cmd_PauseGame;
            }
            else if (0 == lCmdStr.compare("/stop", Qt::CaseInsensitive))
            {
                lCmd.meCmd = Cmd_StopGame;
            }
            else if (0 == lCmdStr.compare("!move", Qt::CaseInsensitive))
            {
                lCmd.meCmd = Cmd_Move;
            }
            else if (0 == lCmdStr.compare("!redraw", Qt::CaseInsensitive))
            {
                lCmd.meCmd = Cmd_Redraw;
            }
            else if (0 == lCmdStr.compare("/help", Qt::CaseInsensitive))
            {
                lCmd.meCmd = Cmd_Help;
            }
            else if (0 == lCmdStr.compare("/quit", Qt::CaseInsensitive))
            {
                lCmd.meCmd = Cmd_Quit;
            }
            else if (0 == lCmdStr.compare("!stats", Qt::CaseInsensitive))
            {
                lCmd.meCmd = Cmd_Stats;
            }
            else
            {
                lCmd.meCmd = Cmd_Unknown;
            }

            emit Command(lCmd);
        }
    }
}

void CConsole::Setup()
{
    // The setup is VERY simply. It's just a QListWidget with a QLineEdit beneath it, nothing else!
    mpLog = new QListWidget(this);
    mpLog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mpLog->setToolTip(tr("Console Log"));
    mpLog->setAutoScroll(true);

    mpCmdPrompt = new QLineEdit(this);
    mpCmdPrompt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mpCmdPrompt->setAlignment(Qt::AlignLeft);
    mpCmdPrompt->setPlaceholderText(tr("> Command"));

    QVBoxLayout *pLay = new QVBoxLayout();
    pLay->addWidget(mpLog);
    pLay->addWidget(mpCmdPrompt);

    setLayout(pLay);

    setWindowTitle(tr("Color Wars Console"));
    setWindowIcon(QIcon(":/ICO_APP"));

    resize(800, 600);
}

int CConsole::GetLines()
{
    return (nullptr != mpLog) ? mpLog->count() : 0;
}

void CConsole::NewLog(QString sLine)
{
    if (nullptr != mpLog)
    {
        // Append the new line.
        if (sLine.contains("[Debug]: "))
        {
            QListWidgetItem *pItem = new QListWidgetItem(QIcon(tr(":/ICO_DEBUG")), sLine.remove("[Debug]: "));
            mpLog->addItem(pItem);
        }
        else if (sLine.contains("[Info]: "))
        {
            QListWidgetItem *pItem = new QListWidgetItem(QIcon(tr(":/ICO_INFO")), sLine.remove("[Info]: "));
            mpLog->addItem(pItem);
        }
        else if (sLine.contains("[Warning]: "))
        {
            QListWidgetItem *pItem = new QListWidgetItem(QIcon(tr(":/ICO_WARN")), sLine.remove("[Warning]: "));
            mpLog->addItem(pItem);
        }
        else if (sLine.contains("[Error]: "))
        {
            QListWidgetItem *pItem = new QListWidgetItem(QIcon(tr(":/ICO_ERR")), sLine.remove("[Error]: "));
            mpLog->addItem(pItem);
        }
        else if (sLine.contains("[CMD]: "))
        {
            QListWidgetItem *pItem = new QListWidgetItem(QIcon(tr(":/ICO_CMD")), sLine.remove("[CMD]: "));
            mpLog->addItem(pItem);
        }
        mpLog->scrollToBottom();
    }
}
