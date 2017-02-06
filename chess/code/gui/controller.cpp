/*******************************************************************************
* Copyright (c) 2016-2017, Comrade Andrew A.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/
#include "controller.h"
#include "utilities/chessutilities.h"

// Phrases for translation:
//  Translates into .ts file as translations for QObject
//  Sends to the required class and prints through QObject::tr()
//  Before send use ChessUtilities, see usage in utilities/chessutilities.h
#if 0
QObject::tr("Waiting for player...")
QObject::tr("Connecting to server...")
QObject::tr("Controller::networkEvent(): broken data has been recieved")
QObject::tr("Connection lost")
#endif

//==============================================================
//                      Controller
//==============================================================

Controller::Controller(BoardWidget * _widget)
{
    network = new Network(this);

    board = nullptr;
    widget = _widget;

    m_isConfigRecieved = false;

    connect(network, SIGNAL(eventRecieved(QByteArray)), this, SLOT(networkEvent(QByteArray)));
    connect(network, SIGNAL(networkConnected()), this, SLOT(connected()));
    connect(network, SIGNAL(networkDisconnected()), this, SLOT(disconnected()));
    connect(network, SIGNAL(networkError(const QString &)), this, SLOT(handleError(const QString &)));

    connect(this, SIGNAL(controllerError(const QString &)), widget, SLOT(enableWaiting(const QString&)));
}

Controller::~Controller()
{
    delete board;
}

void Controller::runServer(const GameConfig &config)
{
    network->run(Network::SERVER);

    if (board) { // if board has already been created
        widget->setBoard(nullptr); // safely remove
        board->disconnect(); // signal & slot
        delete board;
        board = nullptr;
    }

    board = new Chessboard(config);
    widget->setBoard(board);
    widget->enableWaiting(ChessUtilities::chessMark("Waiting for player..."));

    connect(this->board, SIGNAL(netMoveDone(const QList<QVariant> &)),
            this, SLOT(pieceMoved(const QList<QVariant> &)));
}

void Controller::connectIP(const QHostAddress &address)
{
    network->run(Network::CLIENT);
    network->connectTo(address);
    widget->enableWaiting(ChessUtilities::chessMark("Connecting to server..."));
}

void Controller::pieceMoved(const QList<QVariant> &moveList)
{
    QByteArray block;
    QDataStream sendStream(&block, QIODevice::WriteOnly);
    QVariant variantChessEvent, variantMove, variantPieceData;

    variantChessEvent.setValue(ChessEvent(MOVE));
    variantMove.setValue(moveList.at(0));
    variantPieceData.setValue(moveList.at(1));

    sendStream << variantChessEvent << variantMove << variantPieceData;

    network->sendData(block);
}

void Controller::networkEvent(QByteArray data)
{
    QDataStream receiveStream(&data, QIODevice::ReadOnly);
    QVariant variantChessEvent;

    receiveStream >> variantChessEvent;

    if (variantChessEvent.canConvert<ChessEvent>()) {
        ChessEvent chessEvent = variantChessEvent.value<ChessEvent>();

        switch (chessEvent.getEventType())
        {
            case INITALIZE:
            {
                if (board) {
                    widget->setBoard(nullptr); // safely remove
                    board->disconnect(); // signal & slot
                    delete board;
                    board = nullptr;
                }

                QVariant   variantConfig;
                GameConfig config;

                receiveStream >> variantConfig;
                config = variantConfig.value<GameConfig>();

                board = new Chessboard(config);
                widget->setBoard(board);

                connect(this->board, SIGNAL(netMoveDone(const QList<QVariant> &)),
                        this, SLOT(pieceMoved(const QList<QVariant> &)));
                break;
            }
            case MOVE:
            {
                QList<QVariant> moveList;
                QVariant   variantMove, variantPieceData;

                receiveStream >> variantMove >> variantPieceData;
                moveList.push_back(variantMove);
                moveList.push_back(variantPieceData);

                board->netPieceMoved(moveList);

                break;
            }

            default:
                emit controllerError(ChessUtilities::chessMark("Controller::networkEvent(): broken data has been recieved"));
                return;
        }

    } else {
        emit controllerError(ChessUtilities::chessMark("CHESS_TR#Controller::networkEvent(): broken data has been recieved"));
        return;
    }
}

void Controller::disconnected()
{
    auto cfg = board->config;

    widget->setBoard(nullptr); // safely remove
    board->disconnect(); // signal & slot
    delete board;
    board = nullptr;

    if (network->netType == Network::SERVER) {
        runServer(cfg);
    }

    widget->enableWaiting(ChessUtilities::chessMark("Connection lost"));
}

void Controller::connected()
{
    if (network->netType == Network::SERVER) {
        QByteArray block;
        QDataStream sendStream(&block, QIODevice::WriteOnly);
        QVariant variantChessEvent, variantConfig;

        // Change userColor parameter for opponent in config file
        // Swapping colors
        GameConfig cfg = board->config;
        if (cfg.userColor == WHITE)
            cfg.userColor = BLACK;
        else
            cfg.userColor = WHITE;

        variantChessEvent.setValue(ChessEvent(INITALIZE));
        variantConfig.setValue(cfg);

        sendStream << variantChessEvent;
        sendStream << variantConfig;

        network->sendData(block);

        widget->disableWaiting();
    }
    if (network->netType == Network::CLIENT) {
        widget->disableWaiting();
    }
}

void Controller::handleError(const QString &str)
{
    emit controllerError(str);
}
