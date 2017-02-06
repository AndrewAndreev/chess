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
#ifndef CHESSEVENT_H
#define CHESSEVENT_H

#include <QDataStream>

//  =============
//      Enums
//  =============

enum eEventType {
    // Special event types
    INITALIZE, // Event is used to send GameConfig data at connection
    // Solid event types
    MOVE,
    GAMEOVER,
    // Proposing event types
    OFFER,
    RESPOND
};

// whenever event hasn't any connections with the type, EMPTY is set

enum eGameoverType {
    EMPTY_GAMEOVER,
    RESIGN,
    CHECKMATE,
    STALEMATE    
};

enum eOfferType {
    EMPTY_OFFER,
    TAKEBACK,
    DRAW
};

enum eRespondType {
    EMPTY_RESPOND,
    ACCEPT,
    DECLINE
};

//  ==================
//      ClassEvent
//  ==================

class ChessEvent {
public:
    ChessEvent(eEventType = MOVE, eGameoverType = EMPTY_GAMEOVER, eOfferType = EMPTY_OFFER, eRespondType = EMPTY_RESPOND);

    eEventType      getEventType()    const;
    eGameoverType   getGameoverType() const;
    eOfferType      getOfferType()    const;
    eRespondType    getRespondType()  const;

    friend QDataStream & operator << (QDataStream &arch, const ChessEvent & object)
    {
        arch << static_cast<quint32>(object.m_eventType);
        arch << static_cast<quint32>(object.m_gameoverType);
        arch << static_cast<quint32>(object.m_offerType);
        arch << static_cast<quint32>(object.m_respondType);
        return arch;
    }

    friend QDataStream & operator >> (QDataStream &arch, ChessEvent & object)
    {
        arch >> reinterpret_cast<quint32&>(object.m_eventType);
        arch >> reinterpret_cast<quint32&>(object.m_gameoverType);
        arch >> reinterpret_cast<quint32&>(object.m_offerType);
        arch >> reinterpret_cast<quint32&>(object.m_respondType);

        return arch;
    }

private:
    eEventType      m_eventType;
    eGameoverType   m_gameoverType;
    eOfferType      m_offerType;
    eRespondType    m_respondType;
};
Q_DECLARE_METATYPE(ChessEvent)

#endif//CHESSEVENT_H