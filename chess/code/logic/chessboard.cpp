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
#include "chessboard.h"
#include <QtAlgorithms>
#include <QDebug>


//==============================================================
//                    namespace notation
//==============================================================


Square::Square()
{
    position = toPosition(A1);
}

Square::Square(const Position &pos)
{
    position = pos;
}

Square::Square(eSquareNames sqName)
{
    position = toPosition(sqName);
}

Position Square::toPosition(eSquareNames name) const
{
    return Position(( name % 8 ) + 'A', qFloor((qreal)( name ) / 8) + 1);
}

eSquareNames Square::toName() const
{
    return eSquareNames(( position.file - 'A' ) + 8 * ( position.rank - 1 ));
}

Square Square::moveLeft(int squresToMove /*= 1*/) const
{
    return Square(Position(position.file - squresToMove, position.rank));
}

Square Square::moveRight(int squresToMove /*= 1*/) const
{
    return Square(Position(position.file + squresToMove, position.rank));
}

Square Square::moveUp(int squresToMove /*= 1*/) const
{
    return Square(Position(position.file, position.rank + squresToMove));
}

Square Square::moveDown(int squresToMove /*= 1*/) const
{
    return Square(Position(position.file, position.rank - squresToMove));
}

Square Square::moveLeftUp(int squresToMove /*= 1*/) const
{
    return moveLeft(squresToMove).moveUp(squresToMove);
}

Square Square::moveLeftDown(int squresToMove /*= 1*/) const
{
    return moveLeft(squresToMove).moveDown(squresToMove);
}

Square Square::moveRightUp(int squresToMove /*= 1*/) const
{
    return moveRight(squresToMove).moveUp(squresToMove);
}

Square Square::moveRightDown(int squresToMove /*= 1*/) const
{
    return moveRight(squresToMove).moveDown(squresToMove);
}

bool Square::isValid() const
{
    if ( position.rank >= 1 && position.rank <= 8 &&
        position.file >= 'A'  && position.file <= 'H' )
        return true;
    else
        return false;
}

bool Square::operator==(const Square& sq) const
{
    return this->position == sq.position;
}

bool Square::operator!=(const Square& sq) const
{
    return !( this->operator==(sq) );
}


//==============================================================
//                           Piece
//==============================================================

Piece::Piece(const Square &square, ePieceType type, eColor color)
{
    this->m_color   = color;
    this->m_square  = square;
    this->m_type    = type;
    this->m_isTaken = false;
    this->m_numberOfMoves = 0;
}

bool Piece::isOposite(const Piece& piece) const
{
    if (piece.m_color != this->m_color) return true;
    else return false;
}

bool Piece::operator ==(const Piece& piece) const
{
    if (this->m_isTaken == piece.m_isTaken &&
        this->m_square == piece.m_square &&
        this->m_color == piece.m_color &&
        this->m_type == piece.m_type)
        return true;
    else return false;
}

inline int Piece::getNumberOfMoves() const
{
    return this->m_numberOfMoves;
}

bool Piece::isTaken() const
{
    return this->m_isTaken;
}

eColor Piece::getColor() const
{
    return this->m_color;
}

Square Piece::getSquare() const
{
    return this->m_square;
}

ePieceType Piece::getPieceType() const
{
    return m_type;
}

QString Piece::getPieceTypeString() const
{
    switch (m_type) {
        case PAWN:
            return QString("pawn");
        case KNIGHT:
            return QString("knight");
        case BISHOP:
            return QString("bishop");
        case ROOK:
            return QString("rook");
        case QUEEN:
            return QString("queen");
        case KING:
            return QString("king");
        default: return QString();
    }
}

void Piece::move(Square squre, bool iterateMovesForward /*= true*/)
{
    if (this->m_square != squre) {
        this->m_square = squre;
        if (iterateMovesForward) // do move
            this->m_numberOfMoves++;
        else // undo move
            this->m_numberOfMoves--;

        emit pieceMoved(squre);
    }
}

void Piece::promoteTo(ePieceType type)
{
    this->m_type = type;
    emit piecePromoted();
}

void Piece::setTaken(bool isTaken)
{
    this->m_isTaken = isTaken;
    emit pieceTaken(isTaken);
}


//==============================================================
//                        PieceData
//==============================================================

PieceData::PieceData()
{
    square  = Square();
    type    = ePieceType();
    color   = eColor();
    isTaken = false;
}

PieceData::PieceData(const Piece &piece)
{
    isTaken = piece.isTaken();
    square  = piece.getSquare();
    color   = piece.getColor();
    type    = piece.getPieceType();
    numberOfMoves = piece.getNumberOfMoves();
}

PieceData::PieceData(const Square &square, ePieceType type, eColor color, int numberOfMoves /*= 0*/)
{
    this->isTaken = false;
    this->square  = square;
    this->color   = color;
    this->type    = type;
    this->numberOfMoves = numberOfMoves;
}

bool PieceData::isOpposite(const PieceData& piece) const
{
    return this->color != piece.color;
}

bool PieceData::isOpposite(eColor color) const
{
    return this->color != color;
}

bool PieceData::operator==(const PieceData& piece) const
{
    // Move class doesn't contain numberOfMoves information, so we shouldn't
    // check this, otherwise it may lead to the bug where we can't find a piece
    // because we move the first one we get by getPieceAt method in m_undoMovePack
    // Bug: (on the same square captures two pieces with the same color and type)
    return (/* this->numberOfMoves == piece.numberOfMoves && */
            this->isTaken       == piece.isTaken &&
            this->square        == piece.square &&
            this->type          == piece.type &&
            this->color         == piece.color);
}

void PieceData::moveToSquare(Square square)
{
    if (this->square != square) {
        this->square = square;
        numberOfMoves++;
    }
}


//==============================================================
//                      Chessboard
//==============================================================

Chessboard::Chessboard(GameConfig _config)
{
    // Public members initialization

    config = _config;
    if (config.userColor == EMPTY) // Generating at random
        config.userColor = (eColor)(qrand() % 2); // WHITE = 0, BLACK = 1

    enPassantablePiece  = nullptr;

    // Initializing all pieces
    // pawns
    for (auto i = 0; i < 8; i++) {
        pieces.push_back(new Piece((eSquareNames)(A2 + i), PAWN, WHITE));
        pieces.push_back(new Piece((eSquareNames)(A7 + i), PAWN, BLACK));
    }

    // WHITE
    pieces.push_back(new Piece((eSquareNames)(A1), ROOK,    WHITE));
    pieces.push_back(new Piece((eSquareNames)(B1), KNIGHT,  WHITE));
    pieces.push_back(new Piece((eSquareNames)(C1), BISHOP,  WHITE));
    pieces.push_back(new Piece((eSquareNames)(D1), QUEEN,   WHITE));
    pieces.push_back(new Piece((eSquareNames)(E1), KING,    WHITE));
    pieces.push_back(new Piece((eSquareNames)(F1), BISHOP,  WHITE));
    pieces.push_back(new Piece((eSquareNames)(G1), KNIGHT,  WHITE));
    pieces.push_back(new Piece((eSquareNames)(H1), ROOK,    WHITE));

    //BLACK
    pieces.push_back(new Piece((eSquareNames)(A8), ROOK,    BLACK));
    pieces.push_back(new Piece((eSquareNames)(B8), KNIGHT,  BLACK));
    pieces.push_back(new Piece((eSquareNames)(C8), BISHOP,  BLACK));
    pieces.push_back(new Piece((eSquareNames)(D8), QUEEN,   BLACK));
    pieces.push_back(new Piece((eSquareNames)(E8), KING,    BLACK));
    pieces.push_back(new Piece((eSquareNames)(F8), BISHOP,  BLACK));
    pieces.push_back(new Piece((eSquareNames)(G8), KNIGHT,  BLACK));
    pieces.push_back(new Piece((eSquareNames)(H8), ROOK,    BLACK));
    

    // Private members initialization

    m_nMovesWithoutCapture = 0;
    m_moveStackIterator    = -1;
    m_teamToMove           = WHITE;

    // Initializing the last board data
    for each (auto piece in pieces)
        m_lastPiecesData.append(PieceData(*piece));
}

Chessboard::~Chessboard()
{
    qDeleteAll(pieces);
    pieces.clear();
}

Piece* Chessboard::getPieceAt(Square square, eColor searchColor/*= EMPTY*/,
                              bool includeTaken/*= false*/, ePieceType type/*= PAWN*/)
{
    for (auto i = 0; i < pieces.size(); i++) {
        bool sameSquare = pieces[i]->getSquare()    == square;
        bool uncaptured = pieces[i]->isTaken()      == false;
        bool sameType   = pieces[i]->getPieceType() == type;
        bool sameColor  = pieces[i]->getColor()     == searchColor;
        if (sameSquare &&
            ((includeTaken && sameType) || uncaptured) &&
            (searchColor == EMPTY || sameColor))
            return pieces[i]; // returns piece if there is one
    }
    return nullptr; // returns nullptr if there is no such piece
}

eColor Chessboard::getTeamToMove()
{
    return m_teamToMove;
}

bool Chessboard::isMovablePosition()
{
    // Method calls by UI to determine whether to make a move or not
    if ((m_moveStack.size() - 1) != m_moveStackIterator) // isn't last move
        return false;
    
    // Here UI board state (pieces) is the last one which next methods use in their calculations
    if (isKingCheckmated(m_teamToMove) == CHECKMATE_STATE) // king is either checked or checkmated
        return false;
    if (isStalemate())
        return false;
    
    return true;
}

bool Chessboard::isStalemate()
{
    for each (auto piece in m_lastPiecesData) {
        if (piece.color == m_teamToMove) {
            if (getPossibleMoves(piece).size() != 0)
                return false; // there is at least one move
        }
    }
    return true; // there is no moves
}

eCheckMateSate Chessboard::isKingCheckmated(eColor kingColor) const
{
    auto piecesData = m_lastPiecesData;
    
    // If result is in NOCHECK state, then the king is safe
    if (m_isKingUnderCheck(kingColor, piecesData) == false) return NOCHECK;

    auto getPieceDataAt = [&](Square square) {
        return m_getPieceDataIdx(piecesData, square);
    };

    // Here result is CHECK, we should look if king side pieces have any move to protect the king
    bool isThereUncheckMove = false;
    for (auto i = 0; i < piecesData.size(); i++) {
        if (piecesData[i].color == kingColor && piecesData[i].isTaken == false)
        {   // Looking for moves of king mates and king itself
            // getting moves without check for CHECKMATE otherwise it would loop 
            auto moves = getPossibleMoves(piecesData[i], false);
            for each (auto move in moves)
            {
                // Make a move
                if (move.idxPieceToCapture != -1) {
                    piecesData[move.idxPieceToCapture].isTaken = true;
                }
                piecesData[i].moveToSquare(move.square);

                // check if the king is NOT more under CHECK
                if (m_isKingUnderCheck(kingColor, piecesData) == false) {
                    isThereUncheckMove = true;
                    break; // breaks `for` and doesn't reset data
                }
                piecesData = m_lastPiecesData; // reset data after every move has been checked
            }
            piecesData = m_lastPiecesData; // reset data after every piece has been checked
        }
        if (isThereUncheckMove) break; // move has been found
    }

    if (isThereUncheckMove) return CHECK_STATE;
    else return CHECKMATE_STATE; // if there is no such a move king is CHECKMATED
}

QVector<Move> Chessboard::getPossibleMoves(const PieceData &piece, bool checkmateValidation) const
{
    int pieceIdx = m_lastPiecesData.indexOf(piece);
    if (pieceIdx == -1) return QVector<Move>(0);
    return m_getPossibleMoves(m_lastPiecesData, pieceIdx, checkmateValidation);
}

bool Chessboard::isKingChecked() {
    QVector<PieceData> piecesData;
    for (auto i = 0; i < pieces.size(); i++) {
        piecesData.push_back(PieceData(*pieces[i]));
    }

    // If result is in NOCHECK state, then the king is safe
    return m_isKingUnderCheck(m_teamToMove, piecesData);
}

void Chessboard::scrollToMove(int index)
{
    // Validations:
    if (index < 0 || index > (m_moveStack.size() - 1)) return;
    // i is valid and there is at least one move
    if (m_moveStack.size() == 0)
        throw std::runtime_error("ERROR: Chessboard::scrollToMove(int index) - There is no any move yet");
    // m_moveStackIterator is valid iterator of stack

    if (m_moveStackIterator == index) return; // do nothing
    auto step = (m_moveStackIterator > index) ? -1: 1;
    
    while (m_moveStackIterator != index)
    {
        if (step == -1) // undo first
            m_undoMovePack( &m_moveStack[ m_moveStackIterator-- ] );
        if (step == 1) // do the very next move
            m_doMovePack(   &m_moveStack[ ++m_moveStackIterator ] );
    }

    // after first move of white iterator is at the index of 0
    // thus when index is even it is BLACK to move
    if (m_moveStackIterator % 2 == 0)
        m_teamToMove = BLACK;
    else
        m_teamToMove = WHITE;

    emit moveStateScrolled();
}

int Chessboard::getCurrentMoveIndex() const
{
    return m_getCurrentIteratorIndex();
}

int Chessboard::getNumberOfMoves() const
{
    return m_moveStack.size();
}

QVector<PieceData> Chessboard::getBoardStateAfter(int moveIndex)
{
    QVector<PieceData> piecesData;

    // getPieceDataAt uses:
    //      QVector<PieceData> piecesData
    auto getPieceDataAt = [&](Square square) {
        if (square.isValid() == false) return (PieceData*)nullptr;
        for (auto i = 0; i < piecesData.size(); i++)
            if (piecesData[i].square == square && piecesData[i].isTaken == false)
                return &piecesData[i];
        return (PieceData*)nullptr;
    };

    // Initializing board state 
    // pawns
    for (auto i = 0; i < 8; i++) {
        piecesData.push_back(PieceData((eSquareNames)(A2 + i), PAWN, WHITE));
        piecesData.push_back(PieceData((eSquareNames)(A7 + i), PAWN, BLACK));
    }

    // WHITE
    piecesData.push_back(PieceData((eSquareNames)(A1), ROOK,    WHITE));
    piecesData.push_back(PieceData((eSquareNames)(B1), KNIGHT,  WHITE));
    piecesData.push_back(PieceData((eSquareNames)(C1), BISHOP,  WHITE));
    piecesData.push_back(PieceData((eSquareNames)(D1), QUEEN,   WHITE));
    piecesData.push_back(PieceData((eSquareNames)(E1), KING,    WHITE));
    piecesData.push_back(PieceData((eSquareNames)(F1), BISHOP,  WHITE));
    piecesData.push_back(PieceData((eSquareNames)(G1), KNIGHT,  WHITE));
    piecesData.push_back(PieceData((eSquareNames)(H1), ROOK,    WHITE));

    //BLACK
    piecesData.push_back(PieceData((eSquareNames)(A8), ROOK,    BLACK));
    piecesData.push_back(PieceData((eSquareNames)(B8), KNIGHT,  BLACK));
    piecesData.push_back(PieceData((eSquareNames)(C8), BISHOP,  BLACK));
    piecesData.push_back(PieceData((eSquareNames)(D8), QUEEN,   BLACK));
    piecesData.push_back(PieceData((eSquareNames)(E8), KING,    BLACK));
    piecesData.push_back(PieceData((eSquareNames)(F8), BISHOP,  BLACK));
    piecesData.push_back(PieceData((eSquareNames)(G8), KNIGHT,  BLACK));
    piecesData.push_back(PieceData((eSquareNames)(H8), ROOK,    BLACK));

    //  Iterate moves and
    //  change board state
    for (auto i = 0; i < m_moveStack.size() && i <= moveIndex; i++) {
        for each (auto result in m_moveStack[i].results) {
            auto piece = getPieceDataAt(result.startSquare);
            piece->square = result.endSquare;
            piece->isTaken = result.isTaken;
            piece->type = result.typeEnd;
        }
    }

    return piecesData;
}

QPair<Square, Square> Chessboard::getLastMove() const
{
    if (m_moveStackIterator == -1) { // if there is no moves yet
        return QPair<Square, Square>(Square(Position(0, 0)), Square(Position(0, 0))); // return the empty squares
    }
    for each (auto moveResult in m_moveStack[m_moveStackIterator].results)
        if (moveResult.startSquare != moveResult.endSquare)
            return QPair<Square, Square>(moveResult.startSquare, moveResult.endSquare);
    // if there wasn't any return in for
    // one of move results doesn't contain any actual move
    // but maybe we should return the same square? or throw the error #debug
    // throw std::runtime_error("Error: Chessboard::getLastMove() - one of move results doesn't contain any actual move.");
    return QPair<Square, Square>(Square(Position(0, 0)), Square(Position(0, 0)));
}

QString Chessboard::m_getLastPositionInFEN() const
{
    const QVector<PieceData> &pieceData = m_lastPiecesData;
    QString positionInFEN;
    QChar type;
    int emptySquareSequence = 0;

    // Forsyth–Edwards Notation
    // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation

    // 1. Piece placement (from white's perspective)

    for (auto rank = 8; rank >= 1; rank--)
    {
        for (auto file = 'A'; file <= 'H'; file++) {
            auto pieceIdx = m_getPieceDataIdx(pieceData, Square(Position(file, rank)) );
            
            if (pieceIdx != -1) // Empty square
            { 
                emptySquareSequence++;
            }
            else // Square with a piece
            {
                if (emptySquareSequence) {
                    positionInFEN += QString::number(emptySquareSequence);
                    emptySquareSequence = 0;
                }

                const PieceData &piece = pieceData[pieceIdx];

                switch (piece.type)
                {
                    case PAWN:
                        type = 'P';
                        break;
                    case KNIGHT:
                        type = 'N';
                        break;
                    case BISHOP:
                        type = 'B';
                        break;
                    case ROOK:
                        type = 'R';
                        break;
                    case QUEEN:
                        type = 'Q';
                        break;
                    case KING:
                        type = 'K';
                        break;
                }

                if (piece.color == WHITE)
                    type = type.toUpper();
                else // BLACK
                    type = type.toLower();
            }
        }

        if (emptySquareSequence) {
            positionInFEN += QString::number(emptySquareSequence);
            emptySquareSequence = 0;
        }
        positionInFEN += "/";
    }
    positionInFEN += " ";

    // NOTE: The rest of notation does use data of the last move
    // It is important to know, that this part won't work right
    // for arbitrary pieceData.
    // So it generates notation along with m_lastPiecesData only
    // To make it arbitrary we should: 
    //   either return only position
    //   or provide information about enPassant piece,
    //     team to move and number of moves

    // 2. Active color
    if (m_teamToMove == WHITE)
        positionInFEN += "w";
    else // Black
        positionInFEN += "b";
    positionInFEN += " ";

    // 3. Castling availability
    // Castling checks. First king side, then queen side
    auto whiteKingIdx = m_getPieceDataIdx(pieceData, E1);
    auto blackKingIdx = m_getPieceDataIdx(pieceData, E8);
    // if both of kings have been moved
    if (whiteKingIdx == -1 && blackKingIdx == -1) {
        positionInFEN += "-"; // none can castle
    } else {
        bool isCastlingPossible = false;

        // Castle for white
        if (whiteKingIdx != -1) {

            const PieceData &whiteKing = pieceData[whiteKingIdx];
            if (whiteKing.numberOfMoves == 0) {
                // king side
                auto rookH1Idx = m_getPieceDataIdx(pieceData, H1);
                if (rookH1Idx != -1) {
                    const PieceData &rookH1 = pieceData[rookH1Idx];
                    if (rookH1.numberOfMoves == 0) {
                        isCastlingPossible = true;
                        positionInFEN += "K";
                    }
                }

                // queen side
                auto rookA1Idx = m_getPieceDataIdx(pieceData, A1);
                if (rookA1Idx != -1) {
                    const PieceData &rookA1 = pieceData[rookA1Idx];
                    if (rookA1.numberOfMoves == 0) {
                        isCastlingPossible = true;
                        positionInFEN += "Q";
                    }
                }
            }

        }

        // Castle for black
        if (blackKingIdx != -1) {

            const PieceData &blackKing = pieceData[blackKingIdx];
            if (blackKing.numberOfMoves == 0) {
                // king side
                auto rookH8Idx = m_getPieceDataIdx(pieceData, H8);
                if (rookH8Idx != -1) {
                    const PieceData &rookH8 = pieceData[rookH8Idx];
                    if (rookH8.numberOfMoves == 0) {
                        isCastlingPossible = true;
                        positionInFEN += "k";
                    }
                }

                // queen side
                auto rookA8Idx = m_getPieceDataIdx(pieceData, A8);
                if (rookA8Idx != -1) {
                    const PieceData &rookA8 = pieceData[rookA8Idx];
                    if (rookA8.numberOfMoves == 0) {
                        isCastlingPossible = true;
                        positionInFEN += "q";
                    }
                }
            }

        }

        // If neither side can castle
        if (isCastlingPossible == false) {
            positionInFEN += "-";
        }

    }
    positionInFEN += " ";

    // 4. En passant target square in algebraic notation
    if (enPassantablePiece != nullptr)
    {
        auto notationPos = enPassantablePiece->square.position;
        positionInFEN += QChar(notationPos.file).toLower() + QString::number(notationPos.rank);
    }
    else
    {
        positionInFEN += "-";
    }
    positionInFEN += " ";

    // 5. Halfmove clock:
    //    This is the number of halfmoves since the last capture or pawn advance.
    //    This is used to determine if a draw can be claimed under the fifty-move rule.
    positionInFEN += QString::number(m_nMovesWithoutCapture);
    positionInFEN += " ";

    // 6. The number of the full move. It starts at 1, and is incremented after Black's move.
    positionInFEN += QString::number((m_moveStack.size() - 1) / 2 + 1); // Integer devision by 2
    positionInFEN += " ";

    return positionInFEN;
}

QVector<Move> Chessboard::m_getPossibleMoves(const QVector<PieceData> &piecesData, int pieceIndex, bool checkmateValidation /*= true*/) const
{
    QVector<Move> moves;
    Square pieceSquare, moveSquare;
    int pieceDataIdx = -1;

    PieceData piece(piecesData[pieceIndex]);
    
    if (piece.isTaken) { // There is no moves for taken piece
        return QVector<Move>(0);
    }

    auto getPieceDataIdx = [&](Square square) {
        return m_getPieceDataIdx(piecesData, square);
    };

    switch (piece.type)
    {
        case PAWN:
        {
            pieceSquare = piece.square;
            auto add_move = [&]() {
                pieceDataIdx = getPieceDataIdx(moveSquare);
                if (pieceDataIdx != -1 && piecesData[pieceDataIdx].color != piece.color)
                    moves.push_back(Move(moveSquare, pieceDataIdx));
            };

            auto moveForward = [&]() {
                if (piece.color == WHITE) pieceSquare = pieceSquare.moveUp();
                if (piece.color == BLACK) pieceSquare = pieceSquare.moveDown();
            };

            moveForward();
            // it should be impossible for pawn to got invalid forward move o_O
            if (pieceSquare.isValid() == false) break;

            // pawn can make a forward move
            if (getPieceDataIdx(pieceSquare) == -1)
                moves.push_back(Move(pieceSquare, -1));

            if ((moveSquare = pieceSquare.moveLeft()).isValid())  add_move();
            if ((moveSquare = pieceSquare.moveRight()).isValid()) add_move();

            // check for two-square advance move
            if (getPieceDataIdx(pieceSquare) == -1 &&
                (piece.color == WHITE && piece.square.position.rank == 2 ||
                 piece.color == BLACK && piece.square.position.rank == 7))
            {
                moveForward();
                // pawn can make a forward move
                if (getPieceDataIdx(pieceSquare) == -1)
                    moves.push_back(Move(pieceSquare, -1, true));
            }
            // check for En Passant move
            if (enPassantablePiece) {
                // if current piece and enPassantablePiece is on the same horizontal position
                if (enPassantablePiece->square.position.rank == piece.square.position.rank) {
                    // NOTE: pieceSquare already moved forward from piece->square
                    // if enPassantablePiece either from the left or from the right
                    if (piece.square.moveLeft() == enPassantablePiece->square)
                        moves.push_back(Move(pieceSquare.moveLeft(), piecesData.indexOf(*enPassantablePiece)));
                    if (piece.square.moveRight() == enPassantablePiece->square)
                        moves.push_back(Move(pieceSquare.moveRight(), piecesData.indexOf(*enPassantablePiece)));

                }
            }

            break;
        }
        case KNIGHT:
        {
            pieceSquare = piece.square;
            auto add_move = [&]() {
                pieceDataIdx = getPieceDataIdx(moveSquare);
                if (pieceDataIdx == -1 || piece.isOpposite(piecesData[pieceDataIdx]))
                    moves.push_back(Move(moveSquare, pieceDataIdx));
            };

            if ((moveSquare = pieceSquare.moveLeft(2).moveUp()).isValid())    add_move();
            if ((moveSquare = pieceSquare.moveLeft(2).moveDown()).isValid())  add_move();
            if ((moveSquare = pieceSquare.moveRight(2).moveUp()).isValid())   add_move();
            if ((moveSquare = pieceSquare.moveRight(2).moveDown()).isValid()) add_move();
            if ((moveSquare = pieceSquare.moveUp(2).moveLeft()).isValid())    add_move();
            if ((moveSquare = pieceSquare.moveUp(2).moveRight()).isValid())   add_move();
            if ((moveSquare = pieceSquare.moveDown(2).moveLeft()).isValid())  add_move();
            if ((moveSquare = pieceSquare.moveDown(2).moveRight()).isValid()) add_move();

            break;
        }
        case BISHOP:
        {
            pieceSquare = piece.square;
            bool isLast;
            auto add_move = [&]() {
                isLast = false;
                pieceDataIdx = getPieceDataIdx(moveSquare);
                if (pieceDataIdx == -1 || (isLast = piece.isOpposite(piecesData[pieceDataIdx]))) {
                    moves.push_back(Move(moveSquare, pieceDataIdx));
                    return (isLast == false); // if (isLast == false) add_move returns true to continue
                } else return false; // in move square isn't opposite piece
            };

            moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveLeftUp()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveRightUp()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveRightDown()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveLeftDown()).isValid() && add_move());

            break;
        }
        case ROOK:
        {
            pieceSquare = piece.square;
            bool isLast;
            auto add_move = [&]() {
                isLast = false;
                pieceDataIdx = getPieceDataIdx(moveSquare);
                if (pieceDataIdx == -1 || (isLast = piece.isOpposite(piecesData[pieceDataIdx]))) {
                    moves.push_back(Move(moveSquare, pieceDataIdx));
                    return (isLast == false); // if (isLast == false) add_move returns true to continue
                } else return false; // in move square isn't opposite piece
            };

            moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveLeft()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveUp()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveRight()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveDown()).isValid() && add_move());

            break;
        }
        case QUEEN:
        {
            pieceSquare = piece.square;
            bool isLast;
            auto add_move = [&]() {
                isLast = false;
                pieceDataIdx = getPieceDataIdx(moveSquare);
                if (pieceDataIdx == -1 || (isLast = piece.isOpposite(piecesData[pieceDataIdx]))) {
                    moves.push_back(Move(moveSquare, pieceDataIdx));
                    return (isLast == false); // if (isLast == false) add_move returns true to continue
                } else return false; // in move square isn't opposite piece
            };

            moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveLeft()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveLeftUp()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveUp()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveRightUp()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveRight()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveRightDown()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveDown()).isValid() && add_move()); moveSquare = pieceSquare;
            while ((moveSquare = moveSquare.moveLeftDown()).isValid() && add_move());

            break;
        }
        case KING:
        {
            pieceSquare = piece.square;
            auto add_move = [&]() {
                pieceDataIdx = getPieceDataIdx(moveSquare);
                if (pieceDataIdx == -1 || piece.isOpposite(piecesData[pieceDataIdx]))
                    moves.push_back(Move(moveSquare, pieceDataIdx));
            };

            if ((moveSquare = pieceSquare.moveLeft()).isValid())      add_move();
            if ((moveSquare = pieceSquare.moveLeftUp()).isValid())    add_move();
            if ((moveSquare = pieceSquare.moveUp()).isValid())        add_move();
            if ((moveSquare = pieceSquare.moveRightUp()).isValid())   add_move();
            if ((moveSquare = pieceSquare.moveRight()).isValid())     add_move();
            if ((moveSquare = pieceSquare.moveRightDown()).isValid()) add_move();
            if ((moveSquare = pieceSquare.moveDown()).isValid())      add_move();
            if ((moveSquare = pieceSquare.moveLeftDown()).isValid())  add_move();

            // Check for possible castling move
            if (piece.numberOfMoves == 0) {
                int    rookIdx = -1;
                eColor oppositeColor;
                bool   isCastlable, isUnderAttack;
                Square rookSquare, kingSquare = Square(piece.square);
                rookSquare = kingSquare; // rook square moves towards its end position
                moveSquare = kingSquare; // move square is at the end position of the king

                // checks squares from rook to king and adds move of castling if it is possible
                auto add_castling = [&](ePieceType side) {
                    rookIdx = getPieceDataIdx(rookSquare);
                    if (rookIdx != -1) { // if there is rook 
                        if (piecesData[rookIdx].numberOfMoves != 0) return; // rook hasn't been moved

                        isCastlable = true;
                        isUnderAttack = false;
                        // isValid() - check for unsought consequences
                        while (rookSquare != kingSquare && rookSquare.isValid())
                        {
                            isUnderAttack = m_isSquareUnderAttack(rookSquare, oppositeColor, piecesData);
                            pieceDataIdx = getPieceDataIdx(rookSquare);
                            // isCastlable if rookSquare isn't under attack and all the way to king is only
                            // either rook itself or an empty square
                            isCastlable = !((isUnderAttack) || (pieceDataIdx != -1 && rookIdx != pieceDataIdx));
                            if (isCastlable == false) break;
                            // move rookSquare either by queen-side or king-side
                            if (side == QUEEN)
                                rookSquare = rookSquare.moveRight();
                            else if (side == KING)
                                rookSquare = rookSquare.moveLeft();
                            else {
                                isCastlable = false;
                                break;
                            }
                        }
                        if (rookSquare.isValid() == false) return; // rookSquare should always be valid
                        if (isCastlable == true &&
                            m_isKingUnderCheck(piece.color, piecesData) == false)
                        {
                            moves.push_back(Move(moveSquare, -1, false, true));
                        }
                    }
                };

                if (piece.color == WHITE) oppositeColor = BLACK;
                if (piece.color == BLACK) oppositeColor = WHITE;

                rookSquare.position.file = 'A'; // Queen side castling
                moveSquare.position.file = 'C';
                add_castling(QUEEN);

                rookSquare.position.file = 'H'; // King side castling
                moveSquare.position.file = 'G';
                add_castling(KING);
            }

            break;
        }
        default: break;
    }

    // Validate moves with respect to CHECK state
    if (checkmateValidation) {
        auto testMoveData = piecesData;

        eCheckMateSate checkmate_state = isKingCheckmated(piece.color);
        if (checkmate_state == CHECKMATE_STATE) return QVector<Move>(0);

        QVector<Move> validMoves;
        PieceData*    pieceToCheck = nullptr;

        for each (auto& move in moves)
        {
            pieceToCheck = &testMoveData[m_getPieceDataIdx(testMoveData, piece.square)];
            // Make a move
            if (move.idxPieceToCapture != -1) {
                auto pieceIdx = m_getPieceDataIdx(testMoveData, piecesData[move.idxPieceToCapture].square);
                testMoveData[pieceIdx].isTaken = true;
            }

            pieceToCheck->moveToSquare(move.square);

            // check if the king is NOT more under CHECK
            if (m_isKingUnderCheck(piece.color, testMoveData) == NOCHECK)
                validMoves.push_back(move);

            testMoveData = piecesData; // reset data after every move has been checked
        }
        moves = validMoves;
    }

    return moves;
}

MovePack Chessboard::m_getMovePackFromMove(const Move &move, const PieceData &piece, const QVector<PieceData> &pieceData)
{
    MovePack mpack;

    // Saving move of piece
    mpack.results.push_back(MoveResult(piece.type,
                                       piece.type,
                                       piece.square,
                                       move.square,
                                       piece.color));

    // Saving taken piece
    if (move.idxPieceToCapture != -1) {
        mpack.results.push_back(MoveResult(pieceData[move.idxPieceToCapture].type,
                                           pieceData[move.idxPieceToCapture].type,
                                           pieceData[move.idxPieceToCapture].square,
                                           pieceData[move.idxPieceToCapture].square,
                                           pieceData[move.idxPieceToCapture].color,
                                           true));
    }

    // Saving rook move in castling
    if (move.isCastling) {
        int rookIdx;
        Square rookSquare = piece.square; // get king square

        // Setup rook square
        if (move.square.position.file == 'C') { // Queen side castling
            rookSquare.position.file = 'A';
            rookIdx = m_getPieceDataIdx(pieceData, rookSquare);
            rookSquare.position.file = 'D';
        }
        if (move.square.position.file == 'G') { // King side castling
            rookSquare.position.file = 'H';
            rookIdx = m_getPieceDataIdx(pieceData, rookSquare);
            rookSquare.position.file = 'F';
        }

        mpack.results.push_back(MoveResult(pieceData[rookIdx].type,
                                           pieceData[rookIdx].type,
                                           pieceData[rookIdx].square,
                                           rookSquare,
                                           pieceData[rookIdx].color));
    }

    // check for PAWN promotion 
    if (piece.type == PAWN &&
        (move.square.position.rank == 8 || move.square.position.rank == 1))
    {
        ePieceType promotion = QUEEN; // #TODO: make a gui for choosing a piece for pawn promotion
                                      // Saving piece promotion
        mpack.results.push_back(MoveResult(piece.type,
                                           promotion,
                                           move.square, // Move is saved
                                           move.square,
                                           piece.color));
    }

    return mpack;
}

int Chessboard::m_findPieceDataIndex(Piece* piece, const QVector<PieceData> &piecesData)
{
    if (piece == nullptr) return -1;
    PieceData pieceToSearch(*piece);

    for (auto i = 0; i < piecesData.size(); i++)
        if (pieceToSearch == piecesData[i])
            return i;

    return -1;
}

QString Chessboard::m_getRecordPGN(const Move &move, int movedPieceIdx, const QVector<PieceData> &piecesData) const
{
    QString moveInPGN;
    PieceData piece(piecesData[movedPieceIdx]);
    QVector<Square> identicalPiecesSquares;

    // Note: rank contains int number from 1-8 and it should be converted to string before concatenation
    // for instance by adding code of zero (+ '0') and converting to string
    // file in notation should be lowercase

    // Search if there is any ambiguities with this move

    // Pawn move generation
    if (piece.type == PAWN)
    {
        if (move.idxPieceToCapture != -1) // if PAWN takes
        {
            moveInPGN += QChar(piece.square.position.file).toLower(); // PAWN file
            moveInPGN += "x"; // Takes
        }
        // Square to move
        moveInPGN += QChar(move.square.position.file).toLower() + QString(move.square.position.rank + '0');
        // PAWN promotion 
        if (move.square.position.rank == 1 || move.square.position.rank == 8) {
            moveInPGN += "=Q"; // #TODO: choose promotion and save it to the Move class
        }
        return moveInPGN; // Generation done
    }

    if (move.isCastling) {
        if (move.square.position.file == 'G') // King side castling
            moveInPGN = "O-O";
        if (move.square.position.file == 'C') // Queen side castling
            moveInPGN = "O-O-O";
        return moveInPGN; // Generation done
    }

    // Other pieces disambiguation is generalized

    // Add piece type
    // Note: PAWN has been processed
    switch (piece.type)
    {
        case KNIGHT:
            moveInPGN += "N";
            break;
        case BISHOP:
            moveInPGN += "B";
            break;
        case ROOK:
            moveInPGN += "R";
            break;
        case QUEEN:
            moveInPGN += "Q";
            break;
        case KING:
            moveInPGN += "K";
            break;
    }

    // Search from all the pieces which can make the same move
    // to remove disambiguation
    for (auto i = 0; i < piecesData.size(); i++) {
        const PieceData &ithPiece = piecesData[i];
        if (ithPiece.color == piece.color &&    // Search in identical mate pieces
            ithPiece.type == piece.type &&
            ithPiece.square != piece.square) // Except the piece itself
        {
            auto ithPieceMoves = m_getPossibleMoves(piecesData, piecesData.indexOf(ithPiece));
            auto itMove = std::find_if(
                ithPieceMoves.begin(), ithPieceMoves.end(),
                [&](Move m) { return m.square == move.square; }
            );
            if (itMove != ithPieceMoves.end()) // Founded piece with the same possible move
            {
                identicalPiecesSquares.push_back(move.square);
            }
        }
    }

    // Disambiguation between pieces
    // Searching same square positions
    bool differFile = true, differRank = true;
    for each (auto square in identicalPiecesSquares)
    {
        if (square.position.file == piece.square.position.file)
            differFile = false;
        if (square.position.rank == piece.square.position.rank)
            differRank = false;
    }

    // Disambiguation
    if (identicalPiecesSquares.size()) {// if there is at least one
        if (differFile) { // files are differ
            moveInPGN += QChar(piece.square.position.file).toLower();
        } else { // files are the same
            if (differRank) { // but ranks are differ
                moveInPGN += piece.square.position.rank + '0';
            } else { // files and ranks are the same
                moveInPGN += QChar(piece.square.position.file).toLower();
                moveInPGN += piece.square.position.rank + '0';
            }
        }
    }

    if (move.idxPieceToCapture != -1) // if pieces takes
    {
        moveInPGN += "x"; // Takes
    }

    // Square to move
    moveInPGN += QChar(move.square.position.file).toLower() + QString(move.square.position.rank + '0');

    return moveInPGN;
}

void Chessboard::m_updateLastPiecesData()
{
    auto lastMove = m_moveStack.last();
    for each (auto moveResult in lastMove.results)
    {
        auto pieceDataIdx = m_getPieceDataIdx(m_lastPiecesData, moveResult.startSquare, moveResult.pieceColor);
        if (pieceDataIdx == -1)
            throw std::runtime_error("ERROR: Chessboard::m_updateLastPiecesData() - incompatibility with moveStack occurred");
        auto &pieceData = m_lastPiecesData[pieceDataIdx];
        pieceData.type = moveResult.typeEnd;
        pieceData.isTaken = moveResult.isTaken;
        pieceData.moveToSquare(moveResult.endSquare);
    }
}

int Chessboard::m_getPieceDataIdx(const QVector<PieceData> &piecesData,
                                        Square square, eColor searchColor/*= EMPTY*/,
                                        bool includeTaken/*= false*/) const
{
    for (auto i = 0; i < piecesData.size(); i++) {
        if (piecesData[i].square == square &&
            (includeTaken || piecesData[i].isTaken == false) &&
            (searchColor == EMPTY || piecesData[i].color == searchColor))
            return i; // returns piece index if there is one
    }
    return -1; // returns -1 if there is no such piece
}

int Chessboard::m_getCurrentIteratorIndex() const
{
    return m_moveStackIterator;
}

void Chessboard::m_doMovePack(QVector<MovePack>::iterator it)
{
    for (auto i = 0; i < it->results.size(); i++) {
        auto moveResult = it->results.at(i);
        Piece* piece = getPieceAt(moveResult.startSquare, moveResult.pieceColor);
        if (piece == nullptr)
            throw std::runtime_error("ERROR: m_doMovePack() hasn't been called in a proper order");
        piece->promoteTo(moveResult.typeEnd);
        piece->setTaken(moveResult.isTaken);
        piece->move(moveResult.endSquare);
    }
}

void Chessboard::m_undoMovePack(QVector<MovePack>::iterator it)
{
    for (auto i = 0; i < it->results.size(); i++) {
        auto moveResult = it->results.at(i);
        // search for piece including taken pieces and the same type to disambiguate
        Piece* piece = getPieceAt(moveResult.endSquare, moveResult.pieceColor, true, moveResult.typeEnd); 
        if (piece == nullptr)
            throw std::runtime_error("ERROR: m_undoMovePack() hasn't been called in a proper order");
        piece->setTaken(false); // if it was involved in the move it hasn't be taken
        piece->promoteTo(moveResult.typeStart); // Reverse promotion if one has done
        piece->move(moveResult.startSquare, false);
    }
}

bool Chessboard::m_isSquareUnderAttack(Square square, eColor attackFrom, const QVector<PieceData> &piecesData) const 
{
    int    pieceIdx = -1;
    Square initalSquare = square;
    Square checkSquare;
    bool   isUnderAttack;

    // getPieceDataAt uses:
    //      QVector<PieceData> piecesData
    auto getPieceDataAt = [&](Square square) {
        return m_getPieceDataIdx(piecesData, square);
    };

    // checkSquareFor uses:
    //      Square checkSquare as square for searching pieces which attacks the square
    //      QVector<PieceData> piecesData
    //      getPieceDataAt()
    //      
    //      returns true for breaking loop
    auto checkSquareFor = [&](ePieceType type) {
        pieceIdx = getPieceDataAt(checkSquare);
        if (pieceIdx != -1) {
            if (piecesData[pieceIdx].type == type && piecesData[pieceIdx].color == attackFrom) {
                isUnderAttack = true;
                return true;    // enemy piece; break a loop
            } else return true; // mate piece or piece of other type; break a loop
        } else {
            return false; // no pieces, continue loop
        }
    };

    // Checks pieces which can attack the `search square` by their moves
    // move square -> piece -> attack => square under check
    isUnderAttack = false; // reset result state before calculating moves

    // PAWN
    if (attackFrom == WHITE) checkSquare = initalSquare.moveDown();
    if (attackFrom == BLACK) checkSquare = initalSquare.moveUp();

    if ((pieceIdx = getPieceDataAt(checkSquare.moveLeft())) != -1 &&
        piecesData[pieceIdx].type == PAWN && piecesData[pieceIdx].color == attackFrom)
        isUnderAttack = true;
    if ((pieceIdx = getPieceDataAt(checkSquare.moveRight())) != -1 &&
        piecesData[pieceIdx].type == PAWN && piecesData[pieceIdx].color == attackFrom)
        isUnderAttack = true;

    // KNIGHT
    checkSquare = initalSquare.moveLeft(2).moveUp();    checkSquareFor(KNIGHT);
    checkSquare = initalSquare.moveLeft(2).moveDown();  checkSquareFor(KNIGHT);
    checkSquare = initalSquare.moveRight(2).moveUp();   checkSquareFor(KNIGHT);
    checkSquare = initalSquare.moveRight(2).moveDown(); checkSquareFor(KNIGHT);
    checkSquare = initalSquare.moveUp(2).moveLeft();    checkSquareFor(KNIGHT);
    checkSquare = initalSquare.moveUp(2).moveRight();   checkSquareFor(KNIGHT);
    checkSquare = initalSquare.moveDown(2).moveLeft();  checkSquareFor(KNIGHT);
    checkSquare = initalSquare.moveDown(2).moveRight(); checkSquareFor(KNIGHT);
    checkSquare = initalSquare;
    // BISHOP
    while ((checkSquare = checkSquare.moveLeftUp()).isValid()    && checkSquareFor(BISHOP) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveRightUp()).isValid()   && checkSquareFor(BISHOP) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveRightDown()).isValid() && checkSquareFor(BISHOP) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveLeftDown()).isValid()  && checkSquareFor(BISHOP) == false); checkSquare = initalSquare;

    // ROOK
    while ((checkSquare = checkSquare.moveLeft()).isValid()  && checkSquareFor(ROOK) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveUp()).isValid()    && checkSquareFor(ROOK) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveRight()).isValid() && checkSquareFor(ROOK) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveDown()).isValid()  && checkSquareFor(ROOK) == false); checkSquare = initalSquare;

    // QUEEN
    while ((checkSquare = checkSquare.moveLeft()).isValid()      && checkSquareFor(QUEEN) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveLeftUp()).isValid()    && checkSquareFor(QUEEN) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveUp()).isValid()        && checkSquareFor(QUEEN) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveRightUp()).isValid()   && checkSquareFor(QUEEN) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveRight()).isValid()     && checkSquareFor(QUEEN) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveRightDown()).isValid() && checkSquareFor(QUEEN) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveDown()).isValid()      && checkSquareFor(QUEEN) == false); checkSquare = initalSquare;
    while ((checkSquare = checkSquare.moveLeftDown()).isValid()  && checkSquareFor(QUEEN) == false); checkSquare = initalSquare;

    // KING
    checkSquare = initalSquare.moveLeft();      checkSquareFor(KING);
    checkSquare = initalSquare.moveLeftUp();    checkSquareFor(KING);
    checkSquare = initalSquare.moveUp();        checkSquareFor(KING);
    checkSquare = initalSquare.moveRightUp();   checkSquareFor(KING);
    checkSquare = initalSquare.moveRight();     checkSquareFor(KING);
    checkSquare = initalSquare.moveRightDown(); checkSquareFor(KING);
    checkSquare = initalSquare.moveDown();      checkSquareFor(KING);
    checkSquare = initalSquare.moveLeftDown();  checkSquareFor(KING);

    return isUnderAttack;
}

bool Chessboard::m_isKingUnderCheck(eColor kingColor, const QVector<PieceData> &piecesData) const
{
    Square kingSquare;
    eColor oppoentColor;
    bool   isKingUnderCheck;
    int    kingIdx = -1;

    // Declaration lambdas 
    // Takes pointer to king PieceData according to piecesData
    auto initKingFromData = [&]() {
        for (auto i = 0; i < piecesData.size(); i++) {
            if (piecesData[i].type == KING && piecesData[i].color == kingColor)
                kingIdx = i;
        }
        if (kingIdx == -1)
            throw std::runtime_error("ERROR: Chessboard::isKingCheckmated() - there is no king on the board.");
        
        kingSquare = piecesData[kingIdx].square;
        if (piecesData[kingIdx].color == WHITE) oppoentColor = BLACK;
        if (piecesData[kingIdx].color == BLACK) oppoentColor = WHITE;
    };

    // Checks piece which can CHECK the king by their moves
    // move square -> piece -> check => king under check

    initKingFromData();
    isKingUnderCheck = m_isSquareUnderAttack(kingSquare, oppoentColor, piecesData);
    return isKingUnderCheck;
}


//==============================================================
//                        Public slots
//==============================================================

void Chessboard::netPieceMoved(const QList<QVariant> &moveList)
{
    bool isMoveFound = false;
    int  pieceMovedIdx = -1;
    Move      move       = moveList.at(0).value<Move>();
    PieceData movedPiece = moveList.at(1).value<PieceData>();

    auto pieceIdx = m_lastPiecesData.indexOf(movedPiece);
    if (pieceIdx != -1) {
        auto moves = m_getPossibleMoves(m_lastPiecesData, pieceIdx);
        if (moves.indexOf(move) != -1) {
            pieceMovedIdx = pieceIdx;
            movedPiece = m_lastPiecesData[pieceIdx];
            isMoveFound = true;
        }
    }
    if (isMoveFound == false) {
        emit gameOver(EMPTY_GAMEOVER);
        return;
    }

    QString moveInPGN = m_getRecordPGN(move, pieceMovedIdx, m_lastPiecesData);

    // Generate MovePack for reversing moves
    MovePack mpack = m_getMovePackFromMove(move, movedPiece, m_lastPiecesData);

    if (move.idxPieceToCapture != -1) {
        m_nMovesWithoutCapture = 0;
    } else {
        m_nMovesWithoutCapture++;
    }

    enPassantablePiece = nullptr; // enpassant capture must be made at the very next turn or the right to do so is lost
    if (move.isTwoSquareAdvance) { // if PAWN makes two-square advance move it may be taken as en passant
        auto enpassIdx = m_getPieceDataIdx(m_lastPiecesData, movedPiece.square);
        enPassantablePiece = &m_lastPiecesData[enpassIdx];
    }

    // Saving all move results into move stack
    m_moveStack.push_back(mpack);
    m_updateLastPiecesData(); // update last board data
    scrollToMove(m_moveStack.size() - 1); // scroll to the very last move
    
    // #TODO: take action of checkmate state
    eCheckMateSate checkmateState = isKingCheckmated(m_teamToMove);
    if (checkmateState == NOCHECK)         qDebug() << "NOCHECK";
    if (checkmateState == CHECK_STATE) {
        qDebug() << "CHECK";
        moveInPGN += "+";
    }
    if (checkmateState == CHECKMATE_STATE) {
        qDebug() << "CHECKMATE";
        moveInPGN += "#";
        emit gameOver(CHECKMATE);
    }
    if (isStalemate()) {
        qDebug() << "STALEMATE";
        emit gameOver(STALEMATE);
    }

    // #TODO: complete move actions between classes
    emit moveDone(moveInPGN);
}

void Chessboard::uiPieceMoved(Piece* piece, const Move &move)
{
    if (piece == nullptr) // there is no such piece
        throw std::runtime_error("ERROR: Chessboard::uiPieceMoved() - trying to move nonexistent piece.");
    if (m_moveStackIterator != (m_moveStack.size() - 1))
        throw std::runtime_error("ERROR: Chessboard::uiPieceMoved() - accpted UI move signal not in the last position");

    PieceData movedPiece;

    // Generate PGN
    int pieceMovedIdx = m_findPieceDataIndex(piece, m_lastPiecesData);
    if (pieceMovedIdx == -1)
        throw std::runtime_error("ERROR: Chessboard::uiPieceMoved() - called when board state isn't in last position");
    QString moveInPGN = m_getRecordPGN(move, pieceMovedIdx, m_lastPiecesData);
    movedPiece = m_lastPiecesData[pieceMovedIdx];

    // Generate MovePack for reversing moves
    MovePack mpack = m_getMovePackFromMove(move, PieceData(*piece), m_lastPiecesData);

    if (move.idxPieceToCapture != -1) {
        m_nMovesWithoutCapture = 0;
    } else {
        m_nMovesWithoutCapture++;
    }

    enPassantablePiece = nullptr; // enpassant capture must be made at the very next turn or the right to do so is lost
    if (move.isTwoSquareAdvance) { // if PAWN makes two-square advance move it may be taken as en passant
        auto enpassIdx = m_getPieceDataIdx(m_lastPiecesData, piece->getSquare());
        enPassantablePiece = &m_lastPiecesData[enpassIdx];
    }
    
    // Saving all move results into move stack
    m_moveStack.push_back(mpack);
    m_updateLastPiecesData(); // update last board data
    scrollToMove(m_moveStack.size() - 1); // scroll to the very last move

    // #TODO: take action of checkmate state
    eCheckMateSate checkmateState = isKingCheckmated(m_teamToMove);
    if (checkmateState == NOCHECK)         qDebug() << "NOCHECK";
    if (checkmateState == CHECK_STATE) {
        qDebug() << "CHECK";
        moveInPGN += "+";
    }
    if (checkmateState == CHECKMATE_STATE) {
        qDebug() << "CHECKMATE";
        moveInPGN += "#";
        emit gameOver(CHECKMATE);
    }
    if (isStalemate()) {
        qDebug() << "STALEMATE";
        emit gameOver(STALEMATE);
    }

    QList<QVariant> moveList;
    QVariant variantMove, variantPieceData;

    variantMove.setValue(move);
    variantPieceData.setValue(movedPiece);

    moveList.push_back(variantMove);
    moveList.push_back(variantPieceData);

    emit moveDone(moveInPGN);
    emit netMoveDone(moveList);
}
