/////////////////////////////////////////////////////////////////////////////
// Name:        passworddialog.h
// Purpose:     Header for passworddialog.cpp
// Author:      Jan Buchholz
// Created:     2025-10-13
// Changed:     2026-05-22
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QDialog>
#include "cipherengine.h"

namespace Ui {
class PasswordDialog;
}

class PasswordDialog : public QDialog {
    Q_OBJECT

public:
    explicit PasswordDialog(QWidget *parent, CipherEngine& cipherEngine);
    ~PasswordDialog();
    QDialog::DialogCode Execute(behavior mode);

private:
    Ui::PasswordDialog *ui;
    behavior m_mode;
    CipherEngine& m_engine;
    bool checkPassword();
    QString TITLE_ENCRYPT = tr("Password for Encryption");
    QString TITLE_DECRYPT = tr("Password for Decryption");
    QString LABEL_UPPER = tr("Password");
    QString LABEL_LOWER = tr("Confirm Password");

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onTextChanged(QString);

};

