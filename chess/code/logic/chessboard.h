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
#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <QObject>
#include <QVector>
#include <QDataStream>
#include <qmath.h>

#include "chessevent.h"

//==============================================================
//                          Data types
//==============================================================

namespace notation
{

enum eColor {
    WHITE,
    BLACK,
    EMPTY
};

enum eSquareNames {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

struct Position {
    Position() : file('A'), rank(1) {}
    Position(char _file, int _rank) : file(_file), rank(_rank) {}

    char file; // A='A', B='B', C='C', ..., H='H'
    int  rank; // 1-8

    bool operator==(const Position& b) const {
        return b.file == file && b.rank == rank; 
    }


    // DataStream overload
    friend QDataStream & operator << (QDataStream &arch, const Position & object)
    {
        arch << static_cast<quint8>(object.file);
        arch << static_cast<quint32>(object.rank);
        return arch;
    }

    friend QDataStream & operator >> (QDataStream &arch, Position & object)
    {
        arch >> reinterpret_cast<quint8&>(object.file);
        arch >> reinterpret_cast<quint32&>(object.rank);
        return arch;
    }
};

struct Square {
    Position position;

    Square();
    Square(const Position &pos);
    Square(eSquareNames sqName);

    Position toPosition(eSquareNames name) const;
    eSquareNames toName() const;
    
    Square moveLeft     (int squresToMove = 1) const;
    Square moveRight    (int squresToMove = 1) const;
    Square moveUp       (int squresToMove = 1) const;
    Square moveDown     (int squresToMove = 1) const;
    Square moveLeftUp   (int squresToMove = 1) const;
    Square moveLeftDown (int squresToMove = 1) const;
    Square moveRightUp  (int squresToMove = 1) const;
    Square moveRightDown(int squresToMove = 1) const;

    bool isValid() const;
    bool operator==(const Square& sq) const;
    bool operator!=(const Square& sq) const;


    // DataStream overload
    friend QDataStream & operator << (QDataStream &arch, const Square & object)
    {
        arch << object.position;
        return arch;
    }

    friend QDataStream & operator >> (QDataStream &arch, Square & object)
    {
        arch >> object.position;
        return arch;
    }
};

struct RecordPGN {
    int     move_number;
    QString white_move;
    QString black_move;
};

enum eCheckMateSate {
    NOCHECK,
    CHECK_STATE,
    CHECKMATE_STATE
};

}
using namespace notation;

class PieceData;
struct Move {
    Move() : idxPieceToCapture(-1), isTwoSquareAdvance(false), isCastling(false) {}
    Move(Square sq, int pieceIdx, bool isTwoSquare = false, bool isCastlingMove = false) :
        square(sq), idxPieceToCapture(pieceIdx), isTwoSquareAdvance(isTwoSquare), isCastling(isCastlingMove) {}

    Square square;  // Square to move
    int    idxPieceToCapture;

    // if PAWN moves when this flag is true, then this PAWN can be captured by En Passant
    bool    isTwoSquareAdvance; // PAWN flag
    // Moves ROOK and KING and castle either KING or QUEEN side
    bool    isCastling; // KING flag

    bool operator==(const Move& move) const
    {
        return ( this->square == move.square &&
                this->idxPieceToCapture == move.idxPieceToCapture &&
                this->isTwoSquareAdvance == move.isTwoSquareAdvance &&
                this->isCastling == move.isCastling );
    }


    // Stream overload
    friend QDataStream & operator << (QDataStream &arch, const Move & object)
    {
        arch << object.square;
        arch << object.idxPieceToCapture;
        arch << object.isTwoSquareAdvance;
        arch << object.isCastling;
        return arch;
    }

    friend QDataStream & operator >> (QDataStream &arch, Move & object)
    {
        arch >> object.square;
        arch >> object.idxPieceToCapture;
        arch >> object.isTwoSquareAdvance;
        arch >> object.isCastling;
        return arch;
    }
};
Q_DECLARE_METATYPE(Move)

struct GameConfig {
    enum eTimeControl {
        UNLIMITED,
        TIMER
    } timeControl;
    // color of user for making moves in UI
    eColor userColor;

    int timeLeft; // total seconds given on start, don't use when UNLIMITED timeControl
    int increment; // seconds to increment in timer for a move

    GameConfig(eColor color = WHITE, eTimeControl control = UNLIMITED, int seconds = 60, int inc = 2)
    {
        userColor   = color;
        timeControl = control;
        timeLeft    = seconds;
        increment   = inc;
    }


    // Stream overload
    friend QDataStream & operator << (QDataStream &arch, const GameConfig & object)
    {
        arch << static_cast<quint32>(object.timeControl);
        arch << static_cast<quint32>(object.userColor);
        arch << object.timeLeft;
        arch << object.increment;
        return arch;
    }
    friend QDataStream & operator >> (QDataStream &arch, GameConfig & object)
    {
        arch >> reinterpret_cast<quint32&>(object.timeControl);
        arch >> reinterpret_cast<quint32&>(object.userColor);
        arch >> object.timeLeft;
        arch >> object.increment;
        return arch;
    }

};
Q_DECLARE_METATYPE(GameConfig)

//==============================================================
//                      Piece
//==============================================================

enum ePieceType {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

//    Piece contains all information about piece,
//    Used in public namespace of Chessboard for connecting
//    UI pieces and `Chessboard class` representation
class Piece : public QObject {
    Q_OBJECT

public:
    Piece(const Square &square, ePieceType type, eColor color);

    bool isOposite(const Piece& piece) const;
    bool operator ==(const Piece& piece) const;


    int          getNumberOfMoves() const;

    bool         isTaken() const;
    eColor       getColor() const;
    Square       getSquare() const;
    ePieceType   getPieceType() const;

    QString     getPieceTypeString() const;

public slots:
    void move(Square, bool iterateMovesForward = true); // slot saves position and emits signal pieceMoved 
    void promoteTo(ePieceType); // slot uses for PAWN to promote on eighth rank, emits signal promoted
    void setTaken(bool isTaken = true);

signals:
    void pieceMoved(Square);
    void piecePromoted();
    void pieceTaken(bool);

private:
    int         m_numberOfMoves; // counter of moves

    bool        m_isTaken;
    Square      m_square;
    eColor      m_color;
    ePieceType  m_type;
};

//    PieceData contains the same information as Piece class about piece,
//    but doesn't have signals and slots and isn't encapsulated.
//    Used in private namespace of Chessboard
class PieceData {
public:
    PieceData();
    PieceData(const Piece &piece);
    PieceData(const Square &square, ePieceType type, eColor color, int numberOfMoves = 0);

    bool isOpposite(const PieceData& piece) const;
    bool isOpposite(eColor color) const;
    bool operator ==(const PieceData& piece) const;

    void moveToSquare(Square square);

    int         numberOfMoves;
    bool        isTaken;
    Square      square;
    ePieceType  type;
    eColor      color;

    // Stream overload
    friend QDataStream & operator << (QDataStream &arch, const PieceData & object)
    {
        arch << object.numberOfMoves;
        arch << object.isTaken;
        arch << object.square;
        arch << static_cast<quint32>(object.type);
        arch << static_cast<quint32>(object.color);
        return arch;
    }

    friend QDataStream & operator >> (QDataStream &arch, PieceData & object)
    {
        arch >> object.numberOfMoves;
        arch >> object.isTaken;
        arch >> object.square;
        arch >> reinterpret_cast<quint32&>(object.type);
        arch >> reinterpret_cast<quint32&>(object.color);
        return arch;
    }
};
Q_DECLARE_METATYPE(PieceData)


//==============================================================
//        MoveResult contains info for reversing moves
//==============================================================

struct MoveResult {
    MoveResult(ePieceType type = ePieceType(),
               ePieceType promotion = ePieceType(),
               Square start = Square(),
               Square end = Square(),
               eColor color = WHITE,
               bool taken = false) :
        typeStart(type), typeEnd(promotion),
        startSquare(start), endSquare(end),
        pieceColor(color), isTaken(taken) { }
    ePieceType typeStart;
    ePieceType typeEnd;
    Square     startSquare;
    Square     endSquare;
    eColor     pieceColor;
    bool       isTaken;
};

struct MovePack {
    QVector<MoveResult> results;
};


//==============================================================
//                      Chessboard
//==============================================================

class Chessboard : public QObject {
    Q_OBJECT
public:
    Chessboard(GameConfig);
    ~Chessboard();

    GameConfig          config;
    QVector<Piece*>     pieces;    // contains current board state for UI
    QVector<RecordPGN>  movesPGN; // Move in PGN format
    // enPassantablePiece - store piece which can be taken by enpassant move
    PieceData*  enPassantablePiece;

    // getPieceAt:
    //      Reruns piece pointer at square from passing QVector<PieceData> piecesData
    //      By default looking for an uncaptured piece
    //      but if we are looking for captured piece, then we should check for all of the given data
    //      including piece type if there are several pieces have been captured on the same square
    Piece*      getPieceAt(Square, eColor = EMPTY, bool includeTaken = false, ePieceType = PAWN);

    //             Methods use board state after the very last move
    //                          m_lastPiecesData

    // getTeamToMove(): returns color of the team to make a move
    eColor      getTeamToMove();
    // isMovablePosition():
    //      True if teamToMove may make a move
    //      False otherwise: (checkmate, stalemate, position isn't last `scrolled`)
    bool        isMovablePosition();
    // isStalemate():
    //      True if teamToMove doesn't have any move (stalemate)
    //      False otherwise
    bool        isStalemate();
    // isKingCheckmated:
    //      returns:
    //        1. NOCHECK if king is safe
    //        2. CHECK if king is under attack
    //        3. CHECKMATE if king has been checkmated
    eCheckMateSate  isKingCheckmated(eColor king_color) const;
    // getPossibleMoves:
    //      Searches for all valid move with respect to:
    //        1. For CHECKed king returns only protecting moves
    //        2. For CHECKMATed king returns no moves at all
    //        3. Two-square advance of pawns if pawn hasn't been moved yet
    //        4. En Passant move if opponent pawn has just made a two-square advance move
    //        5. If piece has been captured don't return any moves for it
    //      bool checkmateValidation = false - turns off 1. and 2. checks
    QVector<Move>   getPossibleMoves(const PieceData &piece, bool checkmateValidation = true) const;


    //                  Methods use arbitrary board state 
    //               board state stored in QVector<PieceData>

    // isKingChacked(): returns true if king has been checked on the very last move
    bool        isKingChecked();

    // Sets board state as after i-th move
    void scrollToMove(int index);
    int  getCurrentMoveIndex() const;
    int  getNumberOfMoves() const;

    // Gets board state as after i-th move
    QVector<PieceData> getBoardStateAfter(int moveIndex);

    QPair<Square, Square> getLastMove() const;

public slots:
    // Slot for network player moves
    // If there is no move leading to this position
    // gameOver signal will be emitted with EMPTY_GAMEOVER state
    // #TODO: reimplement system of Move exchange through network (1)
    // void netPieceMoved(const QString &baordInFEN);
    void netPieceMoved(const QList<QVariant> &);

    // Slot for ui board moves
    // Will trigger moveDone() signal
    void uiPieceMoved(Piece* piece, const Move &move);

signals:
    // Signals for UI board
    void moveDone(const QString&); // Move in PGN format for BoardWidget
    void moveStateScrolled(); // Emits for UI update view

    // Signals for controller->network
    // #TODO: reimplement system of Move exchange through network (2)
    // void moveDoneFEN(const QString&); // Board position in FEN format after move
    void netMoveDone(const QList<QVariant> &); // signal for Network player
    // gameOver:
    //      The reason is either STALEMATE or CHECKMATE
    //      If something went wrong EMPTY_GAMEOVER state occurs
    void gameOver(eGameoverType reason);

private:
    QVector<PieceData>  m_lastPiecesData; // board state after the very last move
    QVector<MovePack>   m_moveStack;     // contains pack of move results
    int                 m_moveStackIterator;

    int m_nMovesWithoutCapture;

    // m_getLastPositionInFEN:
    //      Calculates position of m_lastPiecesData in FEN format
    QString m_getLastPositionInFEN() const;

    // m_getPossibleMoves(const QVector<PieceData> &piecesData, int pieceIndex, bool checkmateValidation = true);
    //      Takes arbitrary board state instead of getPossibleMoves method
    QVector<Move>   m_getPossibleMoves(const QVector<PieceData> &piecesData, int pieceIndex, bool checkmateValidation = true) const;

    // m_getMovePackFromMove:
    //      Returns MovePack calculated from Move class
    MovePack m_getMovePackFromMove(const Move&, const PieceData&, const QVector<PieceData> &);

    // m_findPieceDataIndex:
    //      Searches for PieceData index by Piece data
    int     m_findPieceDataIndex(Piece* piece, const QVector<PieceData> &piecesData);

    // m_getRecordPGN:
    //      Calculates move in PGN format from data in Move class and given piecesData
    QString m_getRecordPGN(const Move &move, int movedPieceIdx, const QVector<PieceData> &piecesData) const;

    // m_updateLastPiecesData:
    //      Updates m_lastPiecesData according to the last move
    void    m_updateLastPiecesData();

    // m_getPieceDataAt:
    //      Reruns piece data index at square from passing QVector<PieceData> piecesData
    //      By default looking for an uncaptured piece data
    int     m_getPieceDataIdx(const QVector<PieceData> &piecesData, Square, eColor = EMPTY, bool includeTaken = false) const;

    //  m_getCurrentIteratorIndex:
    //      Search through move stack and returns index of iterator in stack
    //          if iterator is out of stack then returns -1
    int     m_getCurrentIteratorIndex() const;

    //  m_doMovePack:
    //      Forces changes of MovePack to be done
    void    m_doMovePack(QVector<MovePack>::iterator);

    //  m_undoMovePack:
    //      Forces changes of MovePack to be undone
    void    m_undoMovePack(QVector<MovePack>::iterator);

    //  m_isSquareUnderAttack:
    //      returns either true or false according to square is either under attack or not.
    bool    m_isSquareUnderAttack(Square square, eColor attackFrom, const QVector<PieceData> &piecesData) const;
    //  m_isKingUnderCheck:
    //      returns either true or false according to king is either under CHECK or not.
    bool    m_isKingUnderCheck(eColor kingColor, const QVector<PieceData> &piecesData) const;

    eColor  m_teamToMove; // team to move in current board position
};

#endif//CHESS_BOARD_H