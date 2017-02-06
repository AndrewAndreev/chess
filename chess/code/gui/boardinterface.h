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
#ifndef BOARDINTERFACE_HPP
#define BOARDINTERFACE_HPP
#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include "ui_boardinterface.h"

class BoardInterface : public QWidget {
	Q_OBJECT

public:
	BoardInterface(QWidget * parent = Q_NULLPTR);
	~BoardInterface();

    void addMovePGN(const QString &moveInPGN, int row, int col);
    void clearHistoryPGN();

public slots:
    void moveSelected(); // Slot for table selection model
    void selectLastMove(); // Slot for UI board to reset selection

private slots:
    void on_proposeTakeback_clicked();
    void on_offerDraw_clicked();
    void on_resign_clicked();

signals:
    void proposeTakeback_clicked();
    void offerDraw_clicked();
    void resign_clicked();
    void moveSelected(int index);

private:
	Ui::BoardInterface ui;
    int movesCount;
};

#endif // BOARDINTERFACE_HPP