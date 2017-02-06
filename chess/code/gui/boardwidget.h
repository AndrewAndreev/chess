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
#ifndef BOARD_WIDGET_H
#define BOARD_WIDGET_H

#include "logic\chessboard.h"
#include "gui\boardinterface.h"

#include <QFileSystemWatcher> 
#include <QPropertyAnimation>
#include <QHoverEvent>
#include <QWidget>
#include <QLabel>
#include <QVector>

class Consts {
private:
    Consts() {}
public:
    // Linear dimension (in pixels) of size for tiles width and height accordingly
    static inline int tileSize() { return 64; }
};

//==============================================================
//                      Wrappers
//==============================================================

// == PieceWrapper ==

class PieceWrapper : public QWidget {
    Q_OBJECT
public:
    PieceWrapper(QWidget *parent, QObject *boardwidget); 
    QLabel *getPieceUI();
    Piece  *getPiece();

    void connectPiece(Piece *);

public slots:
    void pieceMoved(Square); // Slot for Piece classes from Chessboard, moves QLabel Piece
    void piecePromoted();
    void pieceTaken(bool isTaken = true);
    void setSelectable(bool isSelectable = true);
    void setChecked(bool isUnderCheck = true);
    void select();
    void unselect();
    void updateView();
    void reset();

signals:
    void selected(PieceWrapper*);
    void unselected(PieceWrapper*);
protected:
    void mousePressEvent(QMouseEvent*);
private:
    Piece   *m_piece;
    QLabel  *m_pieceLabel;
    QLabel  *m_selectedLabel;
    QLabel  *m_checkLabel; // only for KING
    QObject *m_boardwidget;
    bool     m_isSelected;
    bool     m_isSeletable;
};

// == MoveTilesWrapper ==

class MoveTilesWrapper : public QWidget {
    Q_OBJECT
public:
    MoveTilesWrapper(QWidget *parent, Move move, bool isEmptySquare, QObject *boardwidget);
    void activate(const Move& move, bool isEmptySquare = true);
    void deactivate();
signals:
    void selected(Move);
protected:
    void mousePressEvent(QMouseEvent*);
    bool event(QEvent*);
    void hoverEnter(QHoverEvent*);
    void hoverLeave(QHoverEvent*);
private:
    QPixmap m_pixEmpty;
    QPixmap m_pixEmptyHover;
    QPixmap m_pixPiece;

    QObject *m_boardwidget;
    QLabel  *m_moveTile;
    Move     m_move;
    bool     m_isEmptySquare;
    bool     m_isActive;
};

// == LastMoveWrapper ==

class LastMoveWrapper : public QWidget {
    Q_OBJECT
public:
    LastMoveWrapper(QWidget *parent, QObject *boardwidget);
    void activate(Square from, Square to);
    void deactivate();

private:
    QPixmap m_pixLastMove;

    QLabel  *m_tileFrom;
    QLabel  *m_tileTo;
    QObject *m_boardwidget;
    bool     m_isActive;
};

// == Theme == 

struct ThemePixmap {
    QPixmap black;
    QPixmap white;
};

class Theme : public QObject{
    Q_OBJECT
public:
    Theme();
    // defaultTheme used for reinitializing unfounded pixmaps
    Theme(QString name, Theme* defaultTheme = nullptr);

    QString getThemeName() const;
    QPixmap getPiece(ePieceType, eColor) const;
    QPixmap getTile(eColor) const;

    // loadThemeRes:
    //      loading theme from resources
    //      :/resources/themes/%name%
    void    loadThemeRes(QString name);
    void    setDefaultTheme(Theme* defaultTheme);

public slots:
    void    loadTheme(QString name);

signals:
    void themeChanged();

private:
    bool isInitalized;

    ThemePixmap m_tile;
    ThemePixmap m_king;
    ThemePixmap m_queen;
    ThemePixmap m_rook;
    ThemePixmap m_bishop;
    ThemePixmap m_knight;
    ThemePixmap m_pawn;

    QString m_themeName;

    Theme*  m_defaultTheme;
};

class WaitingWidget : public QWidget {
    Q_OBJECT

public:
    explicit WaitingWidget(QWidget* parent);

    void enableWidget(const QString &text);
    void disableWidget();

protected:
    void resizeEvent(QResizeEvent *);
    void changeEvent(QEvent *);

private:
    QLabel *m_textLabel;
    QLabel *m_backgroundLabel;
    QPixmap m_pixmap;
    QString m_lastMessage;
};

//==============================================================
//                      BoardWidget
//==============================================================

class BoardWidget : public QWidget {
    Q_OBJECT

public:
    BoardWidget(QWidget* parent);

    void    setBoard(Chessboard *);
    Chessboard *getBoard();
    // Accepts coordinates in range [0;7]
    // returns geometry QRect for QLabel by Position on board 
    QRect   getGeometry(Square);
    // Overload for explicit coordinates
    QRect   getGeometry(int x, int y);

    void    movePiece(QWidget*, Square, int delay = 250);

    Theme* getCurrentTheme();

public slots:
    void setupBoardPieces(); // set all pieces up
    void resetBoardPieces(); // reset all pieces
    void pieceSelected(PieceWrapper*);
    void pieceUnselected();
    void unselectAll();
    void moveSelected(const Move&);
    void deactivateAllMoves();
    void setLastMove(Square, Square);
    void setLastMove(const QPair<Square, Square>&);
    void setActiveTeam(eColor);
    void setKingChecked(eColor kingColor, bool isChecked = true);
    void setCurrentTheme(Theme*);
    void updateView();
    void setBoardRotation(eColor);

    void scrollMoves(int index);

    void moveDone(const QString&);
    void enableWaiting(const QString&);
    void disableWaiting();

private:
    void m_updateMoveTiles(); // updates geometry of MoveTilesWrappers
    void m_updatePieceWrappers(); // updates geometry of PieceWrappers
    void m_updateNotationLabels(); // updates geometry of Notation labels

    void m_enableBoard();  // makes UI piece active depend on team to move and user color
    void m_disableBoard(); // makes all UI pieces disabled

    QVector<Theme>   m_themes;
    Theme            m_defaultTheme;
    Theme           *m_currentTheme;
    WaitingWidget   *m_awatingWidget;

    // View from either WHITE or BLACK
    eColor          m_boardRotation;

    Chessboard      *m_board;
    BoardInterface  *m_boardInterface;
    QWidget         *m_boardWrapper;
    
    LastMoveWrapper            *m_lastMoveWrapper;
    PieceWrapper               *m_selectedPiece;
    QVector<PieceWrapper*>      m_uiPieces;
    QVector<MoveTilesWrapper*>  m_tilesToMove;  // Initialize on constructor and show on piece selected

    // Initialized once
    QVector<QLabel*> m_boardNotationLabels;
    QVector<QLabel*> m_tilesVisited; 
    QVector<QLabel*> m_tiles;
};

#endif//BOARD_WIDGET_H