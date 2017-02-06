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
#include "boardinterface.h"

BoardInterface::BoardInterface(QWidget * parent) : QWidget(parent) {
	ui.setupUi(this);

    movesCount = 0;

    // Create columns for white and black moves
    ui.tableMovesPGN->insertColumn(0);
    ui.tableMovesPGN->insertColumn(1);
    // Make single selection behavior
    ui.tableMovesPGN->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui.tableMovesPGN->setSelectionMode(QAbstractItemView::SingleSelection);
    
    ui.tableMovesPGN->horizontalHeader()->hide();
    ui.tableMovesPGN->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    auto selectionModel = ui.tableMovesPGN->selectionModel();
    connect(selectionModel, SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(moveSelected()));
}

BoardInterface::~BoardInterface() {
	
}


void BoardInterface::addMovePGN(const QString &moveInPGN, int row, int col)
{
    if (ui.tableMovesPGN->rowCount() <= row) {
        ui.tableMovesPGN->insertRow(row);

        auto itemWhite = new QTableWidgetItem();
        itemWhite->setFlags(itemWhite->flags() ^ Qt::ItemIsEditable);
        itemWhite->setFlags(itemWhite->flags() ^ Qt::ItemIsSelectable);
        ui.tableMovesPGN->setItem(row, 0, itemWhite);

        auto itemBlack = new QTableWidgetItem();
        itemBlack->setFlags(itemBlack->flags() ^ Qt::ItemIsEditable);
        itemBlack->setFlags(itemBlack->flags() ^ Qt::ItemIsSelectable);
        ui.tableMovesPGN->setItem(row, 1, itemBlack);
    }

    auto item = ui.tableMovesPGN->item(row, col);
    item->setText(moveInPGN);
    item->setFlags(item->flags() | Qt::ItemIsSelectable);

    movesCount++;
}

void BoardInterface::clearHistoryPGN()
{
    movesCount = 0;
    ui.tableMovesPGN->setRowCount(movesCount);
}

void BoardInterface::moveSelected()
{
    auto currentItem = ui.tableMovesPGN->selectionModel()->currentIndex();
    emit moveSelected(currentItem.row()*2 + currentItem.column());
}

void BoardInterface::selectLastMove()
{
    ui.tableMovesPGN->setCurrentCell((movesCount - 1) / 2, (movesCount - 1) % 2);
}

void BoardInterface::on_proposeTakeback_clicked() {
    emit proposeTakeback_clicked();
}

void BoardInterface::on_offerDraw_clicked() {
    emit offerDraw_clicked();
}

void BoardInterface::on_resign_clicked() {
    emit resign_clicked();
}
