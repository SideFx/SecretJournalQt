/////////////////////////////////////////////////////////////////////////////
// Name:        passworddialog.h
// Purpose:     Header for passworddialog.cpp
// Author:      Jan Buchholz
// Created:     2025-10-13
// Changed:     2026-04-04
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
    explicit PasswordDialog(QWidget *parent = nullptr, CipherEngine *cipherEngine = nullptr);
    ~PasswordDialog();
    QDialog::DialogCode Execute(behavior mode);

private:
    Ui::PasswordDialog *ui;
    behavior m_mode;
    CipherEngine *engine;
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

