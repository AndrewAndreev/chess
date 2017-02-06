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
#include "mainwindow.h"
#include "gui\createdialog.h"
#include "ui_mainwindow.h"

#include <QLineEdit>
#include <QInputDialog>
#include <QHostAddress>

//==============================================================
//                        MainWindow
//==============================================================

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    board_widget = new BoardWidget(this);
    controller = new Controller(board_widget);
    this->setCentralWidget(board_widget);
    
    // Fix window size for board and its interface only.
    int menuHeight = menuBar()->height();
    this->setMinimumSize(786, 580 + menuHeight);
    this->setMaximumSize(786, 580 + menuHeight);

    // Translation workflow
    m_langActionGroup = new QActionGroup(this);
    m_translator   = new QTranslator(this);
    m_translatorQt = new QTranslator(this);

    QList<QAction*> actions = ui->menuLanguage->actions();
    for each (auto action in actions)
        m_langActionGroup->addAction(action);

    connect(m_langActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(languageChosen(QAction*)));

    // Qt saving type
    qRegisterMetaTypeStreamOperators<ChessEvent>("ChessEvent");
    qRegisterMetaTypeStreamOperators<GameConfig>("GameConfig");
    qRegisterMetaTypeStreamOperators<PieceData>("PieceData");
    qRegisterMetaTypeStreamOperators<Move>("Move");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// public slots

void MainWindow::mw_close()
{
    close();
}


void MainWindow::languageChosen(QAction *action)
{
    if (action->objectName() == "languageEnglish") {
        QApplication::instance()->removeTranslator(m_translator);
        QApplication::instance()->removeTranslator(m_translatorQt);

        m_translator->load("chess_en", QApplication::instance()->applicationDirPath() + "/local");
        m_translatorQt->load("qt_en", QApplication::instance()->applicationDirPath() + "/local");

        QApplication::instance()->installTranslator(m_translator);
        QApplication::instance()->installTranslator(m_translatorQt);
    }
    else if (action->objectName() == "languageRussian") {
        QApplication::instance()->removeTranslator(m_translator);
        QApplication::instance()->removeTranslator(m_translatorQt);

        m_translator->load("chess_ru", QApplication::instance()->applicationDirPath() + "/local");
        m_translatorQt->load("qt_ru", QApplication::instance()->applicationDirPath() + "/local");

        QApplication::instance()->installTranslator(m_translator);
        QApplication::instance()->installTranslator(m_translatorQt);
    }
}

// protected clots

void MainWindow::resizeEvent(QResizeEvent *)
{
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QMainWindow::changeEvent(event);
}

// private clots

void MainWindow::on_create_triggered()
{
    CreateDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        controller->runServer(dialog.getConfig());
    }
}

void MainWindow::on_connect_triggered()
{
    bool ok;
    QString ip_adress = QInputDialog::getText(this, tr("Connect to server"),
                                              tr("IP:"), QLineEdit::Normal,
                                              "127.0.0.1", &ok);
    if (ok && ip_adress.isEmpty() == false) {
        QHostAddress IP;
        if (IP.setAddress(ip_adress)) // here we have correct IP
            controller->connectIP(IP);
        else
            statusBar()->showMessage(tr("Invalid IP address"));
    } else {
        statusBar()->showMessage(tr("Error occurred during IP parsing"));
    }

}

void MainWindow::on_exit_triggered()
{
    close();
}
