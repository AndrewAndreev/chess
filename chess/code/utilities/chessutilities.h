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
#ifndef CHESS_UTILITIES
#define CHESS_UTILITIES
#include <QObject>

// Usage:
//      Create somewhere QObject::tr("Text to translate") and create
//      translation in .ts file
//      Compile .ts file into .qm
//      Load translation into QCoreApplication
// =============================================================
//      QString sourceText = "Text to translate";
//      QString translatedText = chessTr(chessMark(sourceText));
// =============================================================
// Note: ChessUtilities::chessTr() method will not do anything with strings
//       which hasn't been marked, so it can be used along with
//       Qt native translation when the strings get mixed

class ChessUtilities : public QObject {
	Q_OBJECT

public:
	ChessUtilities(QObject * parent = Q_NULLPTR);
	~ChessUtilities();

    static QString chessMark(const QString&);
    static QString chessTr(const QString&);
    static QString getChessTrMark();
};

#endif // CHESS_UTILITIES