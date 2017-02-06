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
#include "createdialog.h"

#include <QPushButton>

CreateDialog::CreateDialog(QWidget * parent) : QDialog(parent) {
	ui.setupUi(this);

    ui.timeControlBox->setCurrentIndex(0); // Set Unlimited time control by default
    ui.minutesPerSide->setEnabled(false); // Disable sliders
    ui.secondsForMove->setEnabled(false);
    slidersMoved(); // Call slot for initializing value labels

    connect(ui.timeControlBox, SIGNAL(currentIndexChanged(int)), this, SLOT(timeControlChanged()));
    connect(ui.minutesPerSide, SIGNAL(valueChanged(int)), this, SLOT(slidersMoved()));
    connect(ui.secondsForMove, SIGNAL(valueChanged(int)), this, SLOT(slidersMoved()));

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(onAccepted()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(onRejected()));

    // set default side to random
    config.userColor = EMPTY;
    ui.randomKingButton->setChecked(true);
    connect(ui.blackKingButton,  SIGNAL(toggled(bool)), this, SLOT(onSideChosen()));
    connect(ui.randomKingButton, SIGNAL(toggled(bool)), this, SLOT(onSideChosen()));
    connect(ui.whiteKingButton,  SIGNAL(toggled(bool)), this, SLOT(onSideChosen()));

}

CreateDialog::~CreateDialog() {
	
}

void CreateDialog::slidersMoved()
{
    int minutes = ui.minutesPerSide->value();
    int seconds = ui.secondsForMove->value();
    if (minutes == 0 && seconds == 0) // Invalid game configuration
    {   // Disable OK button
        ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false); 
    }
    else // valid game configuration
    {   // Enable OK button
        ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }

    if (ui.timeControlBox->currentIndex() == 0) // Unlimited
        ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true); // Enable OK button

    ui.minutesPerSideValue->setText(QString::number(minutes));
    ui.secondsForMoveValue->setText(QString::number(seconds));

    if (ui.timeControlBox->currentIndex() == 0) // Unlimited
        config.timeControl = GameConfig::UNLIMITED;
    else if (ui.timeControlBox->currentIndex() == 1) // Real time
        config.timeControl = GameConfig::TIMER;
    config.timeLeft = minutes;
    config.increment = seconds;
}

void CreateDialog::timeControlChanged()
{
    // Enable OK button if it was disabled
    ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);

    if (ui.timeControlBox->currentIndex() == 0) // Unlimited
    {   // Disable sliders
        ui.minutesPerSide->setEnabled(false);
        ui.secondsForMove->setEnabled(false);
    }
    else if (ui.timeControlBox->currentIndex() == 1) // Real time
    {   // Enable sliders
        ui.minutesPerSide->setEnabled(true);
        ui.secondsForMove->setEnabled(true);
    }
}

GameConfig CreateDialog::getConfig()
{
    return config;
}

void CreateDialog::onAccepted()
{
    accept();
}

void CreateDialog::onRejected()
{
    reject();
}

void CreateDialog::onSideChosen()
{
    bool isAllUnchecked = !(ui.blackKingButton->isChecked()  ||
                            ui.randomKingButton->isChecked() ||
                            ui.whiteKingButton->isChecked()  );

    if (sender() == ui.blackKingButton) {
        if (isAllUnchecked) ui.blackKingButton->setChecked(true);
        ui.randomKingButton->setChecked(false);
        ui.whiteKingButton->setChecked(false);
        config.userColor = BLACK;
    }
    if (sender() == ui.randomKingButton) {
        if (isAllUnchecked) ui.randomKingButton->setChecked(true);
        ui.blackKingButton->setChecked(false);
        ui.whiteKingButton->setChecked(false);
        config.userColor = EMPTY;
    }
    if (sender() == ui.whiteKingButton) {
        if (isAllUnchecked) ui.whiteKingButton->setChecked(true);
        ui.blackKingButton->setChecked(false);
        ui.randomKingButton->setChecked(false);
        config.userColor = WHITE;
    }
}
