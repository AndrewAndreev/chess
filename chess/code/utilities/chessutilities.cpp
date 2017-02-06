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
#include "chessutilities.h"

ChessUtilities::ChessUtilities(QObject * parent) : QObject(parent) {
	
}

ChessUtilities::~ChessUtilities() {
	
}

QString ChessUtilities::chessMark(const QString &text)
{
    return getChessTrMark() + text;
}

QString ChessUtilities::chessTr(const QString &text)
{
    const QString &mark = getChessTrMark();
    if (text.left(mark.size()) == mark) {
        return QObject::tr(qPrintable(text.mid(mark.size())));
    } else {
        return text;
    }
}

QString ChessUtilities::getChessTrMark()
{
    return QString("CHESS_TR#");
}
