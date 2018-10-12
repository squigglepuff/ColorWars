#include "include/game.h"

std::map<std::string, ECellColors> g_NameToColorMap = {
    std::pair<std::string, ECellColors>("White", Cell_White),
    std::pair<std::string, ECellColors>("Red", Cell_Red),
    std::pair<std::string, ECellColors>("Orange", Cell_Orange),
    std::pair<std::string, ECellColors>("Yellow", Cell_Yellow),
    std::pair<std::string, ECellColors>("Lime", Cell_Lime),
    std::pair<std::string, ECellColors>("Green", Cell_Green),
    std::pair<std::string, ECellColors>("Cyan", Cell_Cyan),
    std::pair<std::string, ECellColors>("Blue", Cell_Blue),
    std::pair<std::string, ECellColors>("Purple", Cell_Purple),
    std::pair<std::string, ECellColors>("Magenta", Cell_Magenta),
    std::pair<std::string, ECellColors>("Pink", Cell_Pink),
    std::pair<std::string, ECellColors>("Brown", Cell_Brown),
    std::pair<std::string, ECellColors>("Gray", Cell_Gray)
};

// ================================ Begin CDice Implementation ================================ //
CDice::CDice() : muLastRoll{0}
{
    // Seed the RNG.
    srand(static_cast<u32>(clock()));
}

CDice::CDice(const CDice&aCls) : muLastRoll{aCls.muLastRoll}
{
    // Intentionally left blank.
}

CDice::~CDice()
{
    // Intentionally left blank.
}

CDice& CDice::operator=(const CDice& aCls)
{
    muLastRoll = aCls.muLastRoll;
    return *this;
}

u32 CDice::Roll(u32 uMin, u32 uMax)
{
    u32 uRoll = floor(rand());
    if (uRoll > uMax)
    {
        muLastRoll = (uMax > 0) ? (uRoll % uMax) + uMin : static_cast<u32>(uRoll);
    }
    else
    {
        muLastRoll = (uMax > 0) ? (uMax % uRoll) + uMin : static_cast<u32>(uRoll);
    }

    return muLastRoll;
}

u32 CDice::GetLastRoll()
{
    return muLastRoll;
}
// ================================ End CDice Implementation ================================ //


// ================================ Begin CGame Implementation ================================ //
CGame::CGame(QObject *pParent) : QObject{pParent}, mbGamePlaying{false}, mCenter{SPoint(0,0)}, muCellSz{0}, mpDice{nullptr}, mpBoard{nullptr},
    mpCanvas{nullptr}, muDiceMax{0xffffffff}, msTmpFileName{"colorwars_development.png"}, mpNetServer{nullptr}, mpNetClient{nullptr}, mpTicker{nullptr}
{
    if (msTmpFileName.find("_DEBUG") == std::string::npos && g_cfgVars.mbIsDebug)
    {
        msTmpFileName.insert(msTmpFileName.find_last_of("."), "_DEBUG");
    }
}

CGame::~CGame()
{
    if (nullptr != mpDice) { delete mpDice; }
    if (nullptr != mpBoard)
    {
        mpBoard->Destroy();
    }

    if (nullptr != mpNetServer) { delete mpNetServer; }
    if (nullptr != mpNetClient) { delete mpNetClient; }
}

void CGame::SetupGame(u32 iDiceMax, u32 uCellSz, SPoint qCenter)
{
    if (nullptr == mpDice && nullptr == mpBoard)
    {
        muDiceMax = iDiceMax;

        // Set the class properties.
        mCenter = qCenter;
        muCellSz = uCellSz;

        // Instantiate a new CDice object.
        mpDice = new CDice();

        // Instantiate a new board.
        mpBoard = new CBoard();

        qInfo("Game has been successfully setup!");
    }
    else
    {
        qCritical("ERR: Cannot re-setup the game!");
    }
}

void CGame::NewGame()
{
    if (!mbGamePlaying)
    {
        if (nullptr != mpDice && nullptr != mpBoard)
        {
            // Are we even able to generate a board?
            if (0 < muCellSz && 0 < mCenter.x() && 0 < mCenter.y())
            {
                // Yes! First clear the board and then (re)generate it.
                mpBoard->Destroy();

                mpBoard->Create(muCellSz, mCenter);
                mvNations = mpBoard->GetNationList();

                // Update the canvas.
                int iCanvasSz = static_cast<int>(mCenter.y() * 2);
                mpCanvas = new QImage(iCanvasSz, iCanvasSz, QImage::Format_ARGB32);
                mpCanvas->fill(Qt::transparent); // Fills the canvas with transparency.

                // Update!
                Draw();

                mbGamePlaying = true;
                qInfo("Game has started!");

                // Fire up the ticker.
                mpTicker = new QTimer();
                connect(mpTicker, &QTimer::timeout, [&]{
                    qApp->processEvents();
                    if (nullptr != mpNetServer)
                    {
                        mpNetServer->Broadcast(Heartbeat_Packet, new QByteArray("~$$HEARTBEAT"));
                    }
                });
                mpTicker->start(c_miTickRate);
            }
            else
            {
                qCritical("Unable to create the board! Invalid parameters! (Did you forget to run SetupGame()?)");
            }
        }
        else
        {
            SetupGame(muDiceMax, muCellSz, mCenter);
            NewGame();
        }
    }
    else
    {
        qCritical("Refusing to start a new game, game is already running!");
    }
}

void CGame::EndGame()
{
    mbGamePlaying = false;
    qInfo("Game has ended!");
}

void CGame::Play(ECellColors eAggressor, ECellColors eVictim)
{
    if (nullptr != mpDice && nullptr != mpBoard && nullptr == mpNetClient)
    {
        /* Determine the ranges we need to be in for movement amounts.
         * These are:
         *  [x - y](25% of range){center of range}  --->  Move 3 spaces
         *  [x - y](10% of range){center of range}  --->  Move 6 spaces
         *  [x - y](5% of range){center of range}   --->  Move 9 spaces
         *  [x - y](1% of range){center of range}   --->  Overtake
         */
        const u32 uSmallMvRange = static_cast<u32>(muDiceMax * 0.25f);
        const u32 uMedMvRange = static_cast<u32>(muDiceMax * 0.10f);
        const u32 uLrgMvRange = static_cast<u32>(muDiceMax * 0.05f);
        const u32 uMidOfRange = static_cast<u32>(muDiceMax / 2.0f);

        // Roll the dice!
        u32 uRoll = mpDice->Roll(3, muDiceMax);

        // Check to see if we should move.
        if ((uSmallMvRange + uMidOfRange) >= uRoll && (uMidOfRange - uSmallMvRange) <= uRoll)
        {
            std::pair<bool, QString> rtnData;
            if ((uMedMvRange + uMidOfRange) >= uRoll && (uMidOfRange - uMedMvRange) <= uRoll)
            {
                if ((uLrgMvRange + uMidOfRange) >= uRoll && (uMidOfRange - uLrgMvRange) <= uRoll)
                {
                    if (uMidOfRange == uRoll)
                    {
                        // Overtake! Move HUGE amount!
                        rtnData = MoveColor(eAggressor, eVictim, 0);
                    }
                    else
                    {
                        // Move large amount!
                        rtnData = MoveColor(eAggressor, eVictim, 7);
                    }
                }
                else
                {
                    // Move medium amount!
                    rtnData = MoveColor(eAggressor, eVictim, 5);
                }
            }
            else
            {
                // Move small amount!
                rtnData = MoveColor(eAggressor, eVictim, 3);
            }

            if (rtnData.first)
            {
                qInfo("%s", rtnData.second.toStdString().c_str());

                rtnData.second.prepend("[Info]: ");
                if (nullptr != mpNetServer)
                {
                    // Update the client boards.
                    std::map<u64, ECellColors> mBoardMap;
                    std::map<u64, CCell*> mCellMap = mpBoard->GetCellMap();

                    for (std::map<u64,CCell*>::iterator pIter = mCellMap.begin(); pIter != mCellMap.end(); ++pIter)
                    {
                        std::pair<u64,CCell*> lMappedCell = (*pIter);

                        if (nullptr != lMappedCell.second && mmOldBoardMap[lMappedCell.first] != lMappedCell.second->GetColor())
                        {
                            mBoardMap.insert(std::pair<u64, ECellColors>(lMappedCell.first, lMappedCell.second->GetColor()));
                            mmOldBoardMap[lMappedCell.first] = lMappedCell.second->GetColor();
                        }
                    }

                    mpNetServer->Broadcast(Update_Packet, PackBoardMap(mBoardMap));
                    mpNetServer->Broadcast(Log_Packet, new QByteArray(rtnData.second.toLatin1()));
                }
            }
            else
            {
                qCritical("%s", rtnData.second.toStdString().c_str());
                if (nullptr != mpNetServer)
                {
                    rtnData.second.prepend("[Error]: ");
                    mpNetServer->Broadcast(Log_Packet, new QByteArray(rtnData.second.toLatin1()));
                }
            }
        }

        const size_t ciBuffSz = 4096;
        char* pMsg = new char[ciBuffSz];
        memset(pMsg, 0, ciBuffSz);
        snprintf(pMsg, ciBuffSz, "You rolled a %u! [needed %u - %u].", uRoll, (uMidOfRange - uSmallMvRange), (uSmallMvRange + uMidOfRange));
        qInfo("%s", pMsg);
        delete[] pMsg;

        // Check if there is only 1 color.
        if (1 == mvNations.size())
        {
            std::string sMsg = g_ColorNameMap[mvNations[0]->GetNationColor()].toStdString();
            sMsg.append(" has won!");

            qInfo("%s", sMsg.c_str());

            sMsg.insert(0, "[Info]: ");
            if (nullptr != mpNetServer)
            {
                mpNetServer->Broadcast(Log_Packet, new QByteArray(sMsg.c_str()));
            }

            Draw();
            EndGame();
        }
    }
}

void CGame::Destroy()
{
    // Delete the dice and board.
    if (nullptr != mpDice) { delete mpDice; }
    if (nullptr != mpBoard)
    {
        mpBoard->Destroy();
        delete mpBoard;
    }
}

bool CGame::ConnectToGame(QString lAddr, u16 lPort)
{
    bool bSuccess = false;

    if (nullptr == mpNetServer)
    {
        if (nullptr == mpNetClient)
        {
            mpNetClient = new CClient(this);
            bSuccess = mpNetClient->Connect(lAddr.toStdString(), lPort);

            if (bSuccess) { connect(mpNetClient, &CClient::UpdateBoard, this, &CGame::Net_UpdateBoard); }
            else { qCritical("Connection FAILED!"); }
        }
        else
        {
            qCritical("You're currently connected to a server! You need to disconnect before connecting to a new one.");
        }
    }
    else
    {
        qCritical("You're currently running a server! You need to close it before connecting to an external one.");
    }

    return bSuccess;
}

bool CGame::LaunchServer(QString lAddr, u16 lPort)
{
    bool bSuccess = false;

    if (IsSetup())
    {
        if (nullptr == mpNetClient)
        {
            if (nullptr == mpNetServer)
            {
                mpNetServer = new CServer(this);
                bSuccess = mpNetServer->Setup(lAddr.toStdString(), lPort);

                if (bSuccess)
                {
                    connect(mpNetServer, &CServer::NewClientVerified, [&](u32 uClient){
                        std::map<u64, ECellColors> mBoardMap;
                        std::map<u64, CCell*> mCellMap = mpBoard->GetCellMap();

                        for (std::map<u64,CCell*>::iterator pIter = mCellMap.begin(); pIter != mCellMap.end(); ++pIter)
                        {
                            std::pair<u64,CCell*> lMappedCell = (*pIter);
                            mBoardMap.insert(std::pair<u64, ECellColors>(lMappedCell.first, lMappedCell.second->GetColor()));
                        }

                        mpNetServer->Transmit(uClient, Update_Packet, PackBoardMap(mBoardMap));
                        mmOldBoardMap = mBoardMap;
                    });

                    connect(mpNetServer, &CServer::SendCommand, this, &CGame::ProcessCommand);
                }
                else
                {
                    qCritical("Unable to start server!");
                }
            }
            else
            {
                qCritical("You're currently running a server! You need to close it before relaunching one.");
            }
        }
        else
        {
            qCritical("You're currently connected to a server! You need to disconnect before launching your own.");
        }
    }
    else
    {
        qCritical("Refusing to launch a server on a non-existant game!");
    }

    return bSuccess;
}

std::pair<bool, QString> CGame::MoveColor(ECellColors eAggressor, ECellColors eVictim, u32 uMvAmnt)
{
    std::pair<bool, QString> rtnData = std::pair<bool, QString>(false, "No move made!");
    if (nullptr != mpDice && nullptr != mpBoard)
    {
        // Check to see if these colors currently have live nations.
        CNation* pAggrNation = nullptr;
        CNation* pVictimNation = nullptr;
        for (std::vector<CNation*>::iterator pNatIter = mvNations.begin(); pNatIter != mvNations.end(); ++pNatIter)
        {
            CNation *pTmpNat = (*pNatIter);
            if (nullptr != pTmpNat)
            {
                if (pTmpNat->GetNationColor() == eAggressor)
                {
                    pAggrNation = pTmpNat;
                }
                else if (pTmpNat->GetNationColor() == eVictim)
                {
                    pVictimNation = pTmpNat;
                }
            }
        }

        if (nullptr != pAggrNation && nullptr != pVictimNation)
        {
            if (0 >= uMvAmnt)
            {
                uMvAmnt = pVictimNation->GetNationSize();
            }

            u32 uCellTaken = 0;
            QString lRtnStr = "No cells taken!";
            if (nullptr != pAggrNation && nullptr != pVictimNation)
            {
                uCellTaken = DoFloodFill(pAggrNation, pVictimNation, uMvAmnt);

                if (0 < uCellTaken)
                {
                    lRtnStr = QString("%1 Took %2 cells from %3!").arg(pAggrNation->GetNationName()).arg(uCellTaken).arg(pVictimNation->GetNationName());
                    if (0 >= pVictimNation->GetNationSize())
                    {
                        mvNations.erase(std::find(mvNations.begin(), mvNations.end(), pVictimNation));
                        lRtnStr = QString("%1 has conquered %2!").arg(pAggrNation->GetNationName()).arg(pVictimNation->GetNationName());
                    }
                }
                else
                {
                    lRtnStr = QString("%1 doesn't border %2!").arg(pAggrNation->GetNationName()).arg(pVictimNation->GetNationName());
                }
            }
            else if (nullptr == pAggrNation && nullptr != pVictimNation)
            {
                lRtnStr = "Your nation doesn't exist!";
            }
            else if (nullptr != pAggrNation && nullptr == pVictimNation)
            {
                lRtnStr = "Their nation doesn't exist!";
            }
            else
            {
                lRtnStr = "Neither nation exists!";
            }

            rtnData = std::pair<bool, QString>(true, lRtnStr);
        }
        else
        {
            rtnData = std::pair<bool, QString>(false, "You can't attack yourself :/");
        }
    }

    return rtnData;
}

void CGame::Draw()
{
    if (nullptr != mpBoard)
    {
        QPainter *pPainter = new QPainter();
        pPainter->begin(mpCanvas);

        pPainter->setBackgroundMode(Qt::TransparentMode);
        pPainter->setBackground(Qt::transparent);

        mpBoard->Draw(pPainter);
        pPainter->end();

        if (nullptr != pPainter) { delete pPainter; }

        if (nullptr == mpNetClient)
        {
            // Save the image.
            mpCanvas->save(QString::fromStdString(msTmpFileName));
        }
    }
}

u32 CGame::DummyRoll()
{
    return (nullptr != mpDice) ? mpDice->Roll(3, muDiceMax) : 0;
}

void CGame::PrintNationStats(ECellColors eClr)
{
    if (NationExists(eClr))
    {
        const size_t iBuffSz = 4096;
        char* pStats = new char[iBuffSz];
        memset(pStats, 0, iBuffSz);

        for (std::vector<CNation*>::iterator pNatIter = mvNations.begin(); pNatIter != mvNations.end(); ++pNatIter)
        {
            CNation* pTmpNat = (*pNatIter);
            if (pTmpNat->GetNationColor() == eClr)
            {
                snprintf(pStats, iBuffSz, "Nation Statistics\n"
                                          "Nation Name: %s\n"
                                          "Nation Color: %s\n"
                                          "Cells Owned: %u",
                         pTmpNat->GetNationName().toStdString().c_str(), g_ColorNameMap[pTmpNat->GetNationColor()].toStdString().c_str(), pTmpNat->GetNationSize());
                break;
            }
        }

        if (nullptr != pStats && 0 < strlen(pStats))
        {
            qInfo(pStats);
            delete[] pStats;
        }
    }
    else
    {
        qCritical(QString("Nation %1 = DIED!").arg(g_ColorNameMap[eClr]).toStdString().c_str());
    }
}

bool CGame::NationExists(ECellColors eColor)
{
    bool bFoundNation = false;
    for (std::vector<CNation*>::iterator pNatIter = mvNations.begin(); pNatIter != mvNations.end(); ++pNatIter)
    {
        if ((*pNatIter)->GetNationColor() == eColor && 0 < (*pNatIter)->GetNationSize())
        {
            bFoundNation = true;
            break;
        }
    }

    return bFoundNation;
}

bool CGame::IsPlaying()
{
    return mbGamePlaying;
}

bool CGame::IsSetup()
{
    return (nullptr != mpDice && nullptr != mpBoard && 0 < mpBoard->GetCellMap().size());
}

u32 CGame::GetDiceMax()
{
    return muDiceMax;
}

CBoard* CGame::GetBoard()
{
    return mpBoard;
}

QImage* CGame::GetCanvas()
{
    return mpCanvas;
}

void CGame::SetDiceMax(u32 iMaxium)
{
    muDiceMax = iMaxium;
}

void CGame::SetCellSize(u32 uCellSz)
{
    muCellSz = uCellSz;
}

void CGame::SetCanvasCenter(SPoint aPt)
{
    mCenter = aPt;
}

u32 CGame::DoFloodFill(CNation* aAggrNation, CNation* aVictimNation, u32 uMvAmnt)
{
    u32 uCellsTaken = 0;
    u32 uNewCells = 0;
    if (nullptr != aAggrNation && nullptr != aVictimNation)
    {
        // Get the nation colors.
        ECellColors eAggressor = aAggrNation->GetNationColor();
        ECellColors eVictim = aVictimNation->GetNationColor();

        do
        {
            uNewCells = 0;

            // Iterate over the aggressor's cells.
            std::vector<u64> vAggrCells = aAggrNation->GetCellIDs();
            for (std::vector<u64>::iterator iAggrCellIter = vAggrCells.begin(); iAggrCellIter != vAggrCells.end(); ++iAggrCellIter)
            {
                u64 uCellID = (*iAggrCellIter);

                // Grab the neighbors of the cell.
                std::vector<CCell*> vNeighbors = mpBoard->GetCellNeighbors(uCellID);

                QString lMsg("Got %1 neighbors");
                lMsg = lMsg.arg(vNeighbors.size());
                qDebug(lMsg.toStdString().c_str());

                // Iterate over the owned cells and check the neighbors.
                for (std::vector<CCell*>::iterator iNeighIter = vNeighbors.begin(); iNeighIter != vNeighbors.end(); ++iNeighIter)
                {
                    CCell* pCell = (*iNeighIter);
                    if (nullptr != pCell && pCell->GetColor() == eVictim)
                    {
                        // Yoink!
                        u64 uNewCellID = static_cast<u32>(pCell->GetPosition().mX);
                        uNewCellID = uNewCellID << 32;
                        uNewCellID += static_cast<u32>(pCell->GetPosition().mY);

                        QString qMsg("Taking cell %1 from %2 nation...");
                        qMsg = qMsg.arg(uNewCellID).arg(g_ColorNameMap[eVictim]);
                        qDebug(qMsg.toStdString().c_str());

                        pCell->SetColor(eAggressor);

                        aAggrNation->Add(uNewCellID);
                        aVictimNation->Remove(uNewCellID);
                        ++uCellsTaken;
                        uNewCells = 1;

                        if (uCellsTaken >= uMvAmnt) { break; }
                    }

                    if (uCellsTaken >= uMvAmnt) { break; }
                }

                if (uCellsTaken >= uMvAmnt) { break; }
            }
        } while(uCellsTaken < uMvAmnt && 0 < uNewCells);
    }

    return uCellsTaken;
}

u32 CGame::DoInfectionFill(CNation* aAggrNation, CNation* aVictimNation, u32 uMvAmnt)
{
    u32 uCellsTaken = 0;

    if (nullptr != aAggrNation && nullptr != aVictimNation)
    {
        // code
    }

    return uCellsTaken;
}

void CGame::ProcessCommand(SCommand lCmd)
{
    std::string sCmd = "";
    switch (lCmd.meCmd)
    {
        case Cmd_NewGame:
        {
            if (!IsPlaying())
            {
                NewGame();
            }
            else
            {
                qCritical("Refusing to start an already playing game!");
            }
            sCmd = "New Game";
            break;
        }
        case Cmd_PlayGame:
        {
            if (IsSetup())
            {
                mbGamePlaying = true;
            }
            else
            {
                qCritical("Unable to play an empty game! Please run \"!new\".");
            }
            sCmd = "Play Game";
            break;
        }
        case Cmd_StopGame:
        {
            if (IsPlaying())
            {
                mbGamePlaying = false;
                EndGame();
            }
            sCmd = "Stop Game";
            break;
        }
        case Cmd_Move:
        {
            // Does nothing for now!
            if (0 < lCmd.mvArgs.size())
            {
                if (nullptr != mpNetClient)
                {
                    QByteArray *pCmdStr = new QByteArray("!move ");

                    for (size_t iIdx = 0; lCmd.mvArgs.size() > iIdx; ++iIdx)
                    {
                        pCmdStr->append(lCmd.mvArgs[iIdx].c_str());
                        pCmdStr->append(" ");
                    }

                    mpNetClient->Transmit(Command_Packet, pCmdStr);

                    delete pCmdStr;
                }
                else
                {
                    if (lCmd.msSender.empty() && nullptr != mpNetServer)
                    {
                        lCmd.msSender = QHostInfo().hostName().toStdString();
                    }
                    else if (lCmd.msSender.empty() && nullptr == mpNetServer)
                    {
                        lCmd.msSender = "You";
                    }

                    // Grab the two arguments and make sure they're valid.
                    QString sTmp = QString::fromStdString(lCmd.mvArgs[0]);
                    sTmp = sTmp.toLower(); // Lower-case it.
                    sTmp[0] = sTmp[0].toLatin1() - ' '; // Capitalize the first letter.
                    std::string lAggr = sTmp.toStdString();

                    std::string lVictim = "White";
                    if (!NationExists(Cell_White))
                    {
                        sTmp = QString::fromStdString(lCmd.mvArgs[1]);
                        sTmp = sTmp.toLower(); // Lower-case it.
                        sTmp[0] = sTmp[0].toLatin1() - ' '; // Capitalize the first letter.
                        lVictim = sTmp.toStdString();
                    }
                    else
                    {
                        qWarning("White still exists, so we're gonna attack them instead.");
                    }

                    if (g_NameToColorMap.end() != g_NameToColorMap.find(lAggr) && g_NameToColorMap.end() != g_NameToColorMap.find(lVictim))
                    {
                        Play(g_NameToColorMap[lAggr], g_NameToColorMap[lVictim]);
                        if (nullptr != mpNetServer)
                        {
                            std::string sMsg = "[Info]: ";
                            sMsg.append(lCmd.msSender);
                            sMsg.append(" attempted to move ");
                            sMsg.append(lAggr);
                            sMsg.append(", attacking ");
                            sMsg.append(lVictim);

                            mpNetServer->Broadcast(Log_Packet, new QByteArray(sMsg.c_str()));
                        }
                    }
                    else if (g_NameToColorMap.end() == g_NameToColorMap.find(lAggr) && g_NameToColorMap.end() != g_NameToColorMap.find(lVictim))
                    {
                        std::string lMsg = "Your nation (";
                        lMsg.append(lAggr);
                        lMsg.append(") doesn't exist!");
                        qCritical("%s", lMsg.c_str());

                        if (nullptr != mpNetServer) { lMsg.insert(0, "[Error]: "); mpNetServer->Transmit(lCmd.muSenderID, Log_Packet, new QByteArray(lMsg.c_str())); }
                    }
                    else if (g_NameToColorMap.end() != g_NameToColorMap.find(lAggr) && g_NameToColorMap.end() == g_NameToColorMap.find(lVictim))
                    {
                        std::string lMsg = "Their nation (";
                        lMsg.append(lVictim);
                        lMsg.append(") doesn't exist!");
                        qCritical("%s", lMsg.c_str());

                        if (nullptr != mpNetServer) { lMsg.insert(0, "[Error]: "); mpNetServer->Transmit(lCmd.muSenderID, Log_Packet, new QByteArray(lMsg.c_str())); }
                    }
                    else
                    {
                        qCritical("Neither nation exists!");

                        if (nullptr != mpNetServer) { mpNetServer->Transmit(lCmd.muSenderID, Log_Packet, new QByteArray("[Error]: Neither nation exists!")); }
                    }

                    // Send a redraw command.
                    ProcessCommand(SCommand(Cmd_Redraw));
                }
            }

            sCmd = "Move";
            break;
        }
        case Cmd_Redraw:
        {
            Draw();
            emit SendGUI_Redraw();

            sCmd = "Redraw";
            break;
        }
        case Cmd_Help:
        {
            qInfo("Discord Commands:\n"
                  "!move <color1> <color2>  -  Move a color1 to color2.\n"
                  "!new     -  Run a new game.\n"
                  "!redraw  -  Redraw the board.\n"
                  "!stats <color>  -  Give a color nation's stats.\n\n"
                  "CLI Commands:\n"
                  "/auto   -  Auto-Play the current game.\n"
                  "/connect <ip> <port> -  Connect to a server.\n"
                  "/help   -  Show this help.\n"
                  "/quit   -  Quits the application.\n"
                  "/server -  Setup a LAN server. (Can take a binding address and port)\n"
                  "/stop   -  Stop/End the current game.");
            sCmd = "Help";
            break;
        }
        case Cmd_Quit:
        {
            emit SendGUI_Quit();
            sCmd = "Close Application";
            break;
        }
        case Cmd_Stats:
        {
            if (0 < lCmd.mvArgs.size())
            {
                // Locate the nation's color.
                // g_NameToColorMap
                QString sClr = QString::fromStdString(lCmd.mvArgs[0]);
                sClr = sClr.toLower(); // Lower-case it.
                sClr[0] = sClr[0].toLatin1() - ' '; // Capitalize the first letter.

                PrintNationStats(g_NameToColorMap[sClr.toStdString()]);
            }
            sCmd = "Nation Stats";
            break;
        }
        case Cmd_ConnectToServer:
        {
            if (1 <= lCmd.mvArgs.size())
            {
                std::string sSvrAddr = lCmd.mvArgs[0];
                u16 uSvrPort = 30113;

                if (2 <= lCmd.mvArgs.size()) { uSvrPort = QString::fromStdString(lCmd.mvArgs[1]).toUShort(); }

                ConnectToGame(QString::fromStdString(sSvrAddr), uSvrPort);
            }
            else
            {
                qCritical("Not enough arguments for /connect! Check /help for more info.");
            }

            sCmd = "Connect to Server";
            break;
        }
        case Cmd_SetupServer:
        {
            std::string sSvrAddr = "0.0.0.0";
            u16 uSvrPort = 30113;

            if (1 <= lCmd.mvArgs.size()) { sSvrAddr = lCmd.mvArgs[0]; }
            if (2 <= lCmd.mvArgs.size()) { uSvrPort = QString::fromStdString(lCmd.mvArgs[1]).toUShort(); }

            LaunchServer(QString::fromStdString(sSvrAddr), uSvrPort);

            sCmd = "Server CMD";
            break;
        }
        default:
        {
            qCritical("Unknown or Invalid command! See \"/help\".");
            sCmd = "UNKNOWN/INVLAID";
            break;
        }
    }

    emit SendGUI_Command(sCmd.c_str());
}

void CGame::Net_UpdateBoard(std::map<u64, ECellColors> lClrMap)
{
    if (IsSetup())
    {
        qInfo("Received color map from server, updating board...");
        std::map<u64, CCell*> mCellMap = mpBoard->GetCellMap();
        for (std::map<u64,ECellColors>::iterator pIter = lClrMap.begin(); pIter != lClrMap.end(); ++pIter)
        {
            std::pair<u64,ECellColors> lMappedCell = (*pIter);
            if (nullptr != mCellMap[lMappedCell.first])
            {
                CNation* pCurrent = mpBoard->ColorToNation(mCellMap[lMappedCell.first]->GetColor());
                CNation* pNew = mpBoard->ColorToNation(lMappedCell.second);

                if (nullptr != pCurrent && nullptr != pNew && pCurrent != pNew)
                {
                    pCurrent->Remove(lMappedCell.first);
                    pNew->Add(lMappedCell.first);
                    mCellMap[lMappedCell.first]->SetColor(lMappedCell.second);
                }
            }
        }
        qInfo("Successfully updated board!");
    }
    else
    {
        qInfo("Setting up board [server]...");
        NewGame();
        qInfo("Successfully setup board!");

        Net_UpdateBoard(lClrMap);
    }

    // Send a redraw command.
    ProcessCommand(SCommand(Cmd_Redraw));
}

// ================================ End CGame Implementation ================================ //
