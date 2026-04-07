/////////////////////////////////////////////////////////////////////////////
// Name:        passworddialog.cpp
// Purpose:     Popup to enter password
// Author:      Jan Buchholz
// Created:     2025-10-13
// Changed:     2026-04-05
/////////////////////////////////////////////////////////////////////////////

#include "passworddialog.h"
#include "ui_passworddialog.h"
#include <QPushButton>

PasswordDialog::PasswordDialog(QWidget *parent, CipherEngine *cipherEngine) : QDialog(parent),
    ui(new Ui::PasswordDialog) {
    ui->setupUi(this);
    setFixedSize(this->geometry().width(),this->geometry().height());
    engine = cipherEngine;
    ui->labelUpper->setText(LABEL_UPPER);
    ui->labelLower->setText(LABEL_LOWER);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &PasswordDialog::onOkClicked);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &PasswordDialog::onCancelClicked);
    connect(ui->lineEditUpper, &QLineEdit::textEdited, this, &PasswordDialog::onTextChanged);
    connect(ui->lineEditLower, &QLineEdit::textEdited, this, &PasswordDialog::onTextChanged);
}

PasswordDialog::~PasswordDialog() {
    delete ui;
}

QDialog::DialogCode PasswordDialog::Execute(behavior mode) {
    m_mode = mode;
    ui->lineEditLower->clear();
    ui->lineEditUpper->clear();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    if (mode == GET) {
        ui->labelLower->setVisible(false);
        ui->lineEditLower->setVisible(false);
        setWindowTitle(TITLE_DECRYPT);
    } else if (mode == SET) {
        ui->labelLower->setVisible(true);
        ui->lineEditLower->setVisible(true);
        setWindowTitle(TITLE_ENCRYPT);
    }
    ui->lineEditUpper->setFocus();
    exec();
    return static_cast<QDialog::DialogCode>(result());
}

bool PasswordDialog::checkPassword() {
    if (m_mode == SET) {
        if ((!ui->lineEditUpper->text().isEmpty()) &&
            ui->lineEditLower->text() == ui->lineEditUpper->text()) return true;
    }
    if (m_mode == GET) if (!ui->lineEditUpper->text().isEmpty()) return true;
    return false;
}

void PasswordDialog::onOkClicked() {
    engine->initializeVector(qStringToStdVector(ui->lineEditUpper->text()), m_mode);
    setResult(QDialog::Accepted);
}

void PasswordDialog::onCancelClicked() {
    ui->lineEditLower->clear();
    ui->lineEditUpper->clear();
    close();
    setResult(QDialog::Rejected);
}

void PasswordDialog::onTextChanged(QString s) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(this->checkPassword());
}
