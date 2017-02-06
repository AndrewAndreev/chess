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
#include "network.h"

Network::Network(QObject * parent) : QObject(parent)
{
    netType = EMPTY;
    m_tcpserver = new QTcpServer(this);
    m_tcpsocket = nullptr;
    m_isConnected = false;
}

void Network::sendData(QByteArray rawData)
{
    if (m_isConnected) {
        QByteArray block;
        QDataStream sendStream(&block, QIODevice::ReadWrite);

        sendStream << quint16(0) << rawData;
        // Size of data
        sendStream.device()->seek(0);
        sendStream << (quint16)(block.size() - sizeof(quint16));

        m_tcpsocket->write(block);
    }
}

void Network::run(NetType _netType)
{
    m_resetConnections();

    netType = _netType;
    if (netType == SERVER) {
        if (m_tcpserver->listen(QHostAddress::Any, 9000) == false) return;
        connect(m_tcpserver, SIGNAL(newConnection()), this, SLOT(connected()), Qt::UniqueConnection);
    }
    if (netType == CLIENT) {
        if (m_tcpsocket == nullptr)
            m_tcpsocket = new QTcpSocket(this);
        connect(m_tcpsocket, SIGNAL(connected()), this, SLOT(connected()), Qt::UniqueConnection);
        connect(m_tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(handleSocketError(QAbstractSocket::SocketError)), Qt::UniqueConnection);
    }
}

void Network::disconnected()
{
    m_resetConnections();
    emit networkDisconnected();
}

void Network::connected()
{
    if (netType == EMPTY) return;
    if (netType == SERVER) {
        if (m_tcpsocket) {
            m_tcpsocket->disconnect(); // signal & slot disconnect
            m_tcpsocket->close();
            m_tcpsocket->deleteLater();
        }
        m_tcpsocket = m_tcpserver->nextPendingConnection();
        connect(m_tcpsocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(handleSocketError(QAbstractSocket::SocketError)), Qt::UniqueConnection);
    }

    connect(m_tcpsocket, SIGNAL(readyRead()), this, SLOT(recieveEvent()), Qt::UniqueConnection);
    connect(m_tcpsocket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::UniqueConnection);

    m_isConnected = true;
    emit networkConnected();
}

void Network::connectTo(QHostAddress address)
{
    if (m_tcpsocket == nullptr || m_isConnected || netType != CLIENT)
    {
        m_resetConnections();
        run(CLIENT);
    }
    m_tcpsocket->connectToHost(address, 9000);
}

void Network::recieveEvent()
{
    QDataStream receiveStream(m_tcpsocket);
    quint16     blockSize;
    QByteArray  data;

    receiveStream >> blockSize;
    if (m_tcpsocket->bytesAvailable() < blockSize) { // Server returned incorrect data
        emit networkError(tr(u8"Network::recieveEvent(): error on data exchange"));
        return;
    }

    receiveStream >> data;

    emit eventRecieved(data);
}

void Network::handleSocketError(QAbstractSocket::SocketError)
{
    QString str = m_tcpsocket->errorString();
    m_resetConnections();
    emit networkError(str);
}

void Network::m_resetConnections()
{
    if (m_tcpsocket != nullptr) {
        if (m_tcpsocket->state() == QTcpSocket::ConnectedState ||
            m_tcpsocket->state() == QTcpSocket::ConnectingState)
        {
            m_tcpsocket->close();
        }
        m_tcpsocket->disconnect();
        m_tcpsocket->deleteLater();
        m_tcpsocket = nullptr;
    }
    

    if (m_tcpserver->isListening())
    {
        m_tcpserver->close();
    }

    m_isConnected = false;
}
