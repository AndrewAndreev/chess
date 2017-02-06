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
#include "boardwidget.h"
#include "utilities/chessutilities.h"

#include <QPainter>

//==============================================================
//                      PieceWrapper
//==============================================================

PieceWrapper::PieceWrapper(QWidget *parent, QObject *boardwidget)
    : QWidget(parent)
{
    m_isSelected = false;
    m_isSeletable = true;

    this->m_piece = nullptr; // initializing in connectPiece(Piece *piece)
    this->m_boardwidget = boardwidget;
    this->m_pieceLabel = new QLabel(this);

    QPixmap selected(":/resources/move_selected.png");
    m_selectedLabel = new QLabel(this);
    m_selectedLabel->setPixmap(selected);
    m_selectedLabel->hide();

    QPixmap check(":/resources/check.png");
    m_checkLabel = new QLabel(this);
    m_checkLabel->setPixmap(check);
    m_checkLabel->hide();

    // Raise piece upon check and selection
    this->m_pieceLabel->raise();
    this->show(); // show the piece if it was hidden
}

QLabel* PieceWrapper::getPieceUI()
{
    return this->m_pieceLabel;
}

Piece* PieceWrapper::getPiece()
{
    return this->m_piece;
}

void PieceWrapper::connectPiece(Piece *piece)
{
    // finally connect pieces for their moves
    piece->disconnect(); // disconnect from other PieceWrappers if there is one
    connect(piece, SIGNAL(pieceMoved(Square)), this, SLOT(pieceMoved(Square)));
    connect(piece, SIGNAL(piecePromoted()), this, SLOT(piecePromoted()));
    connect(piece, SIGNAL(pieceTaken(bool)), this, SLOT(pieceTaken(bool)));

    this->m_piece = piece;

    setSelectable(true);
    updateView();
}

void PieceWrapper::pieceMoved(Square square)
{
    qobject_cast<BoardWidget*>(m_boardwidget)->movePiece(this, square);
}

void PieceWrapper::piecePromoted()
{
    updateView();
}

void PieceWrapper::pieceTaken(bool isTaken)
{
    if (isTaken) this->hide(); // Hide wrapper itself
    else         this->show(); // if its piece is taken
}

void PieceWrapper::setSelectable(bool isSelectable)
{
    this->m_isSeletable = isSelectable;
    this->setAttribute(Qt::WA_TransparentForMouseEvents, !isSelectable);
}

void PieceWrapper::setChecked(bool isUnderCheck)
{
    if (isUnderCheck) m_checkLabel->show();
    else              m_checkLabel->hide();
}

void PieceWrapper::select()
{
    // Only one piece selection can be made at a time
    qobject_cast<BoardWidget*>(m_boardwidget)->unselectAll();
    m_isSelected = true;
    m_selectedLabel->show();
    emit selected(this);
}

void PieceWrapper::unselect()
{
    m_isSelected = false;
    m_selectedLabel->hide();
    emit unselected(this);
}

void PieceWrapper::updateView()
{
    Theme* theme = qobject_cast<BoardWidget*>(m_boardwidget)->getCurrentTheme();
    if (theme == nullptr || m_piece == nullptr) return;

    // Hide piece
    this->hide();

    // Reload pixmap
    QPixmap piecePixmap = theme->getPiece(m_piece->getPieceType(), m_piece->getColor());
    m_pieceLabel->setPixmap(piecePixmap);
    m_pieceLabel->show();

    // Move to square
    qobject_cast<BoardWidget*>(m_boardwidget)->movePiece(this, m_piece->getSquare(), 1);
    
    // Show if hasn't been taken
    if (m_piece->isTaken() == false) // show piece only if it wasn't taken
        this->show();

    // Raise piece upon everything on board wrapper
    this->raise();
}

void PieceWrapper::reset()
{
    if (this->m_piece) {
        this->m_piece->disconnect(this);
    }
    this->m_piece = nullptr;

    setSelectable(false);
    unselect();

    // Hide piece
    this->hide();
}

void PieceWrapper::mousePressEvent(QMouseEvent*)
{
    m_isSelected = !m_isSelected;
    if (m_isSelected)
        select();
    else
        unselect();
}

//==============================================================
//                      MoveTilesWrapper
//==============================================================

MoveTilesWrapper::MoveTilesWrapper(QWidget *parent, Move move, bool isEmptySquare, QObject *boardwidget) 
    : QWidget(parent)
{
    this->setAttribute(Qt::WA_Hover);

    this->m_boardwidget = boardwidget;
    this->m_isActive = false;
    this->m_isEmptySquare = isEmptySquare;
    this->m_move = move;

    this->setGeometry( qobject_cast<BoardWidget*>(m_boardwidget)->getGeometry(move.square) );

    m_pixEmpty.load(":/resources/move_empty.png");
    m_pixEmptyHover.load(":/resources/move_empty_hover.png");
    m_pixPiece.load(":/resources/move_piece.png");

    m_moveTile = new QLabel(this);

    if (isEmptySquare) m_moveTile->setPixmap(m_pixEmpty);
    else               m_moveTile->setPixmap(m_pixPiece);

    m_moveTile->hide();
}

void MoveTilesWrapper::activate(const Move &move, bool isEmptySquare)
{
    this->m_isActive = true;
    this->m_move = move;
    this->m_isEmptySquare = isEmptySquare;

    QPixmap moveTilePixmap;
    if (isEmptySquare)  moveTilePixmap.load(":/resources/move_empty.png");
    else                moveTilePixmap.load(":/resources/move_piece.png");
    m_moveTile->setPixmap(moveTilePixmap);
    m_moveTile->show();
}

void MoveTilesWrapper::deactivate()
{
    this->m_isActive = false;
    m_moveTile->hide();
}

void MoveTilesWrapper::mousePressEvent(QMouseEvent*)
{
    if (m_isActive) emit selected(m_move);
}


bool MoveTilesWrapper::event(QEvent * e)
{
    switch (e->type())
    {
        case QEvent::HoverEnter:
            hoverEnter(dynamic_cast<QHoverEvent*>(e));
            return true;
            break;
        case QEvent::HoverLeave:
            hoverLeave(dynamic_cast<QHoverEvent*>(e));
            return true;
            break;
        default:
            break;
    }
    return QWidget::event(e);
}

void MoveTilesWrapper::hoverEnter(QHoverEvent*)
{
    if (m_isEmptySquare) m_moveTile->setPixmap(m_pixEmptyHover);
}

void MoveTilesWrapper::hoverLeave(QHoverEvent*)
{
    if (m_isEmptySquare) m_moveTile->setPixmap(m_pixEmpty);
    else                 m_moveTile->setPixmap(m_pixPiece);
}

//==============================================================
//                      LastMoveWrapper
//==============================================================

LastMoveWrapper::LastMoveWrapper(QWidget *parent, QObject *boardwidget)
    : QWidget(parent)
{
    m_boardwidget = boardwidget;

    m_pixLastMove.load(":/resources/last_move.png");

    m_tileFrom  = new QLabel(parent);
    m_tileTo    = new QLabel(parent);

    m_tileFrom->setPixmap(m_pixLastMove);
    m_tileTo->setPixmap(m_pixLastMove);

    deactivate();
}

void LastMoveWrapper::activate(Square from, Square to)
{
    if (from.isValid() == false && to.isValid() == false) {
        deactivate();
        return;
    }

    if (from.isValid()) {
        m_tileFrom->setGeometry(qobject_cast<BoardWidget*>(m_boardwidget)->getGeometry(from));
        m_tileFrom->show();
    }
    if (to.isValid()) {
        m_tileTo->setGeometry(qobject_cast<BoardWidget*>(m_boardwidget)->getGeometry(to));
        m_tileTo->show();
    }

    m_isActive = true;
}

void LastMoveWrapper::deactivate()
{
    m_tileFrom->hide();
    m_tileTo->hide();

    m_isActive = false;
}

//==============================================================
//                         Theme
//==============================================================

Theme::Theme() 
    : isInitalized(false)
{
    // TODO: make default pieces
    m_defaultTheme = nullptr;
}

Theme::Theme(QString name, Theme * defaultTheme)
{
    m_defaultTheme = defaultTheme;
    m_themeName = name;
    loadTheme(name);
}


QString Theme::getThemeName() const
{
    return m_themeName;
}

QPixmap Theme::getPiece(ePieceType pieceType, eColor color) const
{
    auto getPixmap = [&](const ThemePixmap &tpix) {
        if (color == WHITE) return tpix.white;
        else return tpix.black;
    };

    switch (pieceType) {
        case PAWN:
            return getPixmap(m_pawn);
        case KNIGHT:
            return getPixmap(m_knight);
        case BISHOP:
            return getPixmap(m_bishop);
        case ROOK:
            return getPixmap(m_rook);
        case QUEEN:
            return getPixmap(m_queen);
        case KING:
            return getPixmap(m_king);
        default:
            // return PAWN pixmap on strange request
            return getPixmap(m_pawn);
    }
}

QPixmap Theme::getTile(eColor color) const
{
    if (color == WHITE) return m_tile.white;
    else return m_tile.black;
}

void Theme::loadThemeRes(QString name)
{
    m_tile.white.load(":/resources/themes/" + name + "/board/white_tile.png");
    m_tile.black.load(":/resources/themes/" + name + "/board/black_tile");

    m_king.black.load(":/resources/themes/" + name + "/black/king.png");
    m_king.white.load(":/resources/themes/" + name + "/white/king.png");

    m_queen.black.load(":/resources/themes/" + name + "/black/queen.png");
    m_queen.white.load(":/resources/themes/" + name + "/white/queen.png");

    m_rook.black.load(":/resources/themes/" + name + "/black/rook.png");
    m_rook.white.load(":/resources/themes/" + name + "/white/rook.png");

    m_bishop.black.load(":/resources/themes/" + name + "/black/bishop.png");
    m_bishop.white.load(":/resources/themes/" + name + "/white/bishop.png");

    m_knight.black.load(":/resources/themes/" + name + "/black/knight.png");
    m_knight.white.load(":/resources/themes/" + name + "/white/knight.png");

    m_pawn.black.load(":/resources/themes/" + name + "/black/pawn.png");
    m_pawn.white.load(":/resources/themes/" + name + "/white/pawn.png");

    m_themeName = name;
    isInitalized = true;
    emit themeChanged();
}

void Theme::setDefaultTheme(Theme* defaultTheme)
{
    m_defaultTheme = defaultTheme;
}


void Theme::loadTheme(QString name)
{
    auto validateTile = [&](bool loaded) {
        if (loaded == false && m_defaultTheme) {
            m_tile.white = m_defaultTheme->getTile(WHITE);
            m_tile.black = m_defaultTheme->getTile(BLACK);
        }
    };
    auto validatePiece = [&](bool loaded, ThemePixmap* tpix, ePieceType pieceType) {
        if (loaded == false && m_defaultTheme) {
            tpix->white = m_defaultTheme->getPiece(pieceType, WHITE);
            tpix->black = m_defaultTheme->getPiece(pieceType, BLACK);
        }
    };

    validateTile(m_tile.white.load(":/resources/themes/" + name + "/board/white_tile.png"));
    validateTile(m_tile.black.load(":/resources/themes/" + name + "/board/black_tile.png"));


    validatePiece(m_pawn.white.load(":/resources/themes/" + name + "/white/pawn.png"), &m_king, PAWN);
    validatePiece(m_pawn.black.load(":/resources/themes/" + name + "/black/pawn.png"), &m_king, PAWN);

    validatePiece(m_knight.white.load(":/resources/themes/" + name + "/white/knight.png"), &m_king, KNIGHT);
    validatePiece(m_knight.black.load(":/resources/themes/" + name + "/black/knight.png"), &m_king, KNIGHT);

    validatePiece(m_bishop.white.load(":/resources/themes/" + name + "/white/bishop.png"), &m_king, BISHOP);
    validatePiece(m_bishop.black.load(":/resources/themes/" + name + "/black/bishop.png"), &m_king, BISHOP);

    validatePiece(m_rook.white.load(":/resources/themes/" + name + "/white/rook.png"), &m_king, ROOK);
    validatePiece(m_rook.black.load(":/resources/themes/" + name + "/black/rook.png"), &m_king, ROOK);

    validatePiece(m_queen.white.load(":/resources/themes/" + name + "/white/queen.png"), &m_king, QUEEN);
    validatePiece(m_queen.black.load(":/resources/themes/" + name + "/black/queen.png"), &m_king, QUEEN);

    validatePiece(m_king.white.load(":/resources/themes/" + name + "/white/king.png"), &m_king, KING);
    validatePiece(m_king.black.load(":/resources/themes/" + name + "/black/king.png"), &m_king, KING);

    m_themeName = name;
    isInitalized = true;
    emit themeChanged();
}

//==============================================================
//                         AwaitingWidget
//==============================================================


WaitingWidget::WaitingWidget(QWidget* parent) : QWidget(parent)
{
    m_backgroundLabel = new QLabel(this);
    m_textLabel = new QLabel(this);

    m_textLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setStyleSheet("font-weight: bold; color : white;");

    m_backgroundLabel->hide();
    m_textLabel->hide();

    this->hide();
}


void WaitingWidget::enableWidget(const QString &text)
{
    m_lastMessage = text;
    m_backgroundLabel->show();

    m_textLabel->show();
    m_textLabel->setText(ChessUtilities::chessTr(m_lastMessage));
    this->show();
}

void WaitingWidget::disableWidget()
{
    m_backgroundLabel->hide();
    m_textLabel->hide();
    this->hide();
}

void WaitingWidget::resizeEvent(QResizeEvent *e)
{
    m_pixmap = QPixmap(e->size());
    m_pixmap.fill(Qt::transparent);

    QPainter p(&m_pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::black, 1));

    QPainterPath path;
    path.addRoundedRect(QRectF(0.5, 0.5, (qreal)this->width() - 0.5, (qreal)this->height() - 0.5), 16, 16);

    p.fillPath(path, QColor(0, 0, 0, 192));
    p.drawPath(path);


    m_textLabel->resize(this->size());
    m_backgroundLabel->resize(this->size());
    m_backgroundLabel->setPixmap(m_pixmap);
}

void WaitingWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        m_textLabel->setText(ChessUtilities::chessTr(m_lastMessage));
    }

    QWidget::changeEvent(event);
}

//==============================================================
//==============================================================

//==============================================================
//                      BoardWidget
//==============================================================

BoardWidget::BoardWidget(QWidget* parent) 
    : QWidget(parent) 
{
    this->m_board = nullptr; // initializing in setBoard(Chessboard *board)
    m_currentTheme = nullptr;

    // #TODO: make relative design to avoid magic numbers
    m_awatingWidget = new WaitingWidget(this);
    m_awatingWidget->setGeometry(17 + 128, 32 + 128, 256, 256);

    m_boardInterface = new BoardInterface(this);
    m_boardInterface->setGeometry(545, 112, m_boardInterface->width(), m_boardInterface->height());
    
    m_boardWrapper = new QWidget(this);
    m_boardWrapper->setGeometry(17, 32, 512, 512 + 20); // Shift and size of board + menu height

    m_defaultTheme.loadThemeRes("default");
    m_boardRotation = WHITE;

    // Create notation labels
    QFont font("Helvetica", 8);
    QPalette pallete;
    pallete.setColor(QPalette::Window,     "#eeeeee");
    pallete.setColor(QPalette::WindowText, "#8a8a8a");

    auto initLastNotationLabel = [&]() {
        auto label = m_boardNotationLabels.last();
        label->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
        label->setFont(font);
        label->setPalette(pallete);
    };

    for (auto i = 0; i < 8; i++) { // Letters
        m_boardNotationLabels.push_back(new QLabel(this));
        m_boardNotationLabels.last()->setGeometry(17 + i*64, 544, 64, 16);
        //m_boardNotationLabels.last()->setText(QChar('A' + i));
        initLastNotationLabel();
    }
    for (auto i = 0; i < 8; i++) { // Digits
        m_boardNotationLabels.push_back(new QLabel(this)); 
        m_boardNotationLabels.last()->setGeometry(528, 32 + i*64, 16, 64);
        //m_boardNotationLabels.last()->setText(QChar('1' + i));
        initLastNotationLabel();
    }
    m_updateNotationLabels();

    // Create board tiles
    for (auto digit = 0; digit < 8; digit++)
        for (auto letter = 0; letter < 8; letter++) {
            QLabel *tileLabel = new QLabel(m_boardWrapper);
            m_tiles.push_back(tileLabel);
        }

    m_lastMoveWrapper = new LastMoveWrapper(m_boardWrapper, this);

    // Create move tiles
    for (auto i = 0; i < 64; i++) {
        m_tilesToMove.push_back(new MoveTilesWrapper(m_boardWrapper, Move((eSquareNames)i, -1), true, this));
        connect(m_tilesToMove.last(), SIGNAL(selected(Move)), this, SLOT(moveSelected(Move)));
    }

    // Create UI pieces
    for (auto i = 0; i < 32; i++) {
        m_uiPieces.push_back(new PieceWrapper(m_boardWrapper, this));
        connect(m_uiPieces.last(), SIGNAL(selected(PieceWrapper*)), this, SLOT(pieceSelected(PieceWrapper*)));
        connect(m_uiPieces.last(), SIGNAL(unselected(PieceWrapper*)), this, SLOT(pieceUnselected()));
    }
    m_selectedPiece = nullptr;
    resetBoardPieces();

    // Connect widgets
    connect(m_boardInterface, SIGNAL(moveSelected(int)), this, SLOT(scrollMoves(int)));

    // Set theme to default
    setCurrentTheme(&m_defaultTheme);

    // Set awatingWidget upon everything on the board
    m_awatingWidget->raise();
}


void BoardWidget::setBoard(Chessboard *board)
{
    if (m_board)
        m_board->disconnect();

    m_board = board;

    if (m_board == nullptr) {
        disableWaiting();
        resetBoardPieces();
        // clear the last move
        setLastMove(QPair<Square, Square>(Square(Position(0, 0)), Square(Position(0, 0))));
        return;
    } 
    // m_board is valid
    m_boardRotation = m_board->config.userColor;

    m_boardInterface->clearHistoryPGN();
    setupBoardPieces();
    updateView(); 
    connect(m_board, SIGNAL(moveStateScrolled()), this, SLOT(updateView()));
    connect(m_board, SIGNAL(moveDone(const QString&)), this, SLOT(moveDone(const QString&)));
}

Chessboard *BoardWidget::getBoard()
{
    return this->m_board;
}

QRect BoardWidget::getGeometry(Square pos)
{
    // shift both ranges into 0-7
    int x = pos.position.file - 'A';
    int y = pos.position.rank - 1;
    return getGeometry(x, y);
}

QRect BoardWidget::getGeometry(int x, int y)
{
    QRect whiteSide = QRect(x*Consts::tileSize(), (7 - y)*Consts::tileSize(), Consts::tileSize(), Consts::tileSize());
    QRect blackSide = QRect((7 - x)*Consts::tileSize(), y*Consts::tileSize(), Consts::tileSize(), Consts::tileSize());

    if (m_board == nullptr) return whiteSide;

    if (m_boardRotation == WHITE)
        return whiteSide;
    else // BLACK
        return blackSide;
}

void BoardWidget::movePiece(QWidget *piece, Square pos, int delay)
{
    QPropertyAnimation *animation = new QPropertyAnimation(piece, "geometry");
    animation->setDuration(delay);

    animation->setStartValue(piece->geometry());
    animation->setEndValue(getGeometry(pos));
    animation->setEasingCurve(QEasingCurve::OutCubic);

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

Theme * BoardWidget::getCurrentTheme()
{
    return m_currentTheme;
}

void BoardWidget::setupBoardPieces()
{
    if (m_board->pieces.size() != m_uiPieces.size())
        throw std::runtime_error("Error: setupBoardPieces(), pieces in Chessboard and BoardWidget are different");
    for (auto i = 0; i < m_board->pieces.size(); i++)
        m_uiPieces[i]->connectPiece(m_board->pieces[i]);
}

void BoardWidget::resetBoardPieces()
{
    for (auto i = 0; i < m_uiPieces.size(); i++)
        m_uiPieces[i]->reset();
}

void BoardWidget::pieceSelected(PieceWrapper* pieceWrapper)
{
    // Call chessboard for possible moves of a selected piece
    m_selectedPiece = pieceWrapper;

    auto moves = m_board->getPossibleMoves(PieceData(*m_selectedPiece->getPiece()));
    for each (auto move in moves)
    {
        m_tilesToMove.at( move.square.toName() )->activate(move, m_board->getPieceAt(move.square) == nullptr);
    }
}

void BoardWidget::pieceUnselected()
{
    m_selectedPiece = nullptr;
    deactivateAllMoves();
}

void BoardWidget::unselectAll()
{
    for (auto i = 0; i < m_uiPieces.size(); i++)
        m_uiPieces[i]->unselect();
}

void BoardWidget::moveSelected(const Move &move)
{
    if (m_selectedPiece == nullptr) return;

    eColor movedTeamColor = m_selectedPiece->getPiece()->getColor();
    setKingChecked(movedTeamColor, false);  // KING can't be under check after any move

    m_board->uiPieceMoved(m_selectedPiece->getPiece(), move);
    unselectAll();
    deactivateAllMoves();
}

void BoardWidget::deactivateAllMoves()
{
    for (auto i = 0; i < m_tilesToMove.size(); i++)
        m_tilesToMove[i]->deactivate();
}

void BoardWidget::setLastMove(Square from, Square to)
{
    m_lastMoveWrapper->activate(from, to);
}

void BoardWidget::setLastMove(const QPair<Square, Square>& lastMove)
{
    m_lastMoveWrapper->activate(lastMove.first, lastMove.second);
}

void BoardWidget::setActiveTeam(eColor team)
{
    if (m_board == nullptr) return;
    for (auto i = 0; i < m_uiPieces.size(); i++) {
        bool selectable = m_uiPieces[i]->getPiece()->getColor() == team;
        m_uiPieces[i]->setSelectable(selectable);
    }
}

void BoardWidget::setKingChecked(eColor king_color, bool isChecked)
{
    for (auto i = 0; i < m_uiPieces.size(); i++) {
        Piece* piece = m_uiPieces[i]->getPiece();
        if (piece->getPieceType() == KING && piece->getColor() == king_color) {
            m_uiPieces[i]->setChecked(isChecked);
            break;
        }
    }
}

void BoardWidget::setCurrentTheme(Theme *theme)
{
    m_currentTheme = theme;

    QPixmap *tilePixmap = nullptr;
    bool colorToggler = false; // Bright - true, Dark - false

    // Set board tiles
    for (auto digit = 0; digit < 8; digit++) {
        for (auto letter = 0; letter < 8; letter++) {
            if (colorToggler == true) // Bright
                tilePixmap = &m_currentTheme->getTile(WHITE);
            else                     // Dark
                tilePixmap = &m_currentTheme->getTile(BLACK);

            auto tileLabel = m_tiles[Square(Position(letter + 'A', digit + 1)).toName()];

            tileLabel->setPixmap(*tilePixmap);
            tileLabel->setGeometry(getGeometry(letter, digit));

            tileLabel->show();

            colorToggler = !colorToggler; // toggle color of tile after tile
        }
        colorToggler = !colorToggler; // toggle color of tile after line
    }

    // Reload theme for UI pieces
    for (auto i = 0; i < m_uiPieces.size(); i++)
        m_uiPieces[i]->updateView();

}

void BoardWidget::updateView()
{
    setLastMove(m_board->getLastMove());
    unselectAll();
    m_updateMoveTiles();
    m_updateNotationLabels();

    // Update pieces positions if board has been rotated
    for (auto i = 0; i < m_uiPieces.size(); i++)
        m_uiPieces[i]->updateView();

    eColor teamToMove = m_board->getTeamToMove();
    eColor opposite = (teamToMove == WHITE) ? BLACK : WHITE;
    m_enableBoard(); // activates pieces 

    if (m_board->isKingChecked()) {
        setKingChecked(teamToMove, true);
    }
    else {
        setKingChecked(teamToMove, false);
    }
    setKingChecked(opposite, false); // opposite team should always has unchecked king
}

void BoardWidget::setBoardRotation(eColor color)
{
    m_boardRotation = color;
    updateView();
}

void BoardWidget::scrollMoves(int index)
{
    if (m_board) {
        m_board->scrollToMove(index);
    }
}

void BoardWidget::moveDone(const QString& moveInPGN)
{
    int teamCol, moveRow;
    if (m_board->getTeamToMove() == WHITE) // it is move by black
        teamCol = 1;
    else // it is move by black
        teamCol = 0;
    
    // one move is a complete sequence of white and black moves
    moveRow = (m_board->getNumberOfMoves() - 1)/2;

    m_boardInterface->addMovePGN(moveInPGN, moveRow, teamCol);
    m_boardInterface->selectLastMove();
}

void BoardWidget::enableWaiting(const QString &text)
{
    m_awatingWidget->enableWidget(text);
    m_disableBoard();
}

void BoardWidget::disableWaiting()
{
    m_awatingWidget->disableWidget();
    m_enableBoard(); // Waiting has been disabled, board should be enabled
}

void BoardWidget::m_updateMoveTiles()
{
    for (auto i = 0; i < m_tilesToMove.size(); i++) {
        m_tilesToMove[i]->setGeometry( getGeometry( Square((eSquareNames)i) ) );
    }
}

void BoardWidget::m_updatePieceWrappers()
{
    for (auto i = 0; i < m_uiPieces.size(); i++) {
        m_uiPieces[i]->updateView();
    }
}

void BoardWidget::m_updateNotationLabels()
{
    if (m_boardRotation == WHITE) { // in direct order
        for (auto i = 0; i < 8; i++) {
            m_boardNotationLabels[i]->setText(QChar('A' + i));   // File
            m_boardNotationLabels[i + 8]->setText(QChar('8' - i)); // Rank
        }
    } else {  // in reverse order
        for (auto i = 0; i < 8; i++) {
            m_boardNotationLabels[i]->setText(QChar('H' - i));   // File
            m_boardNotationLabels[i + 8]->setText(QChar('1' + i)); // Rank
        }
    }
}

void BoardWidget::m_enableBoard()
{
    if (m_board == nullptr) return;
    eColor teamToMove = m_board->getTeamToMove();
    if (m_board->isMovablePosition() && teamToMove == m_board->config.userColor) {
        setActiveTeam(teamToMove);
    } else {
        setActiveTeam(EMPTY);
    }
}

void BoardWidget::m_disableBoard()
{
    setActiveTeam(EMPTY);
}
