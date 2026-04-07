/////////////////////////////////////////////////////////////////////////////
// Name:        io.cpp
// Purpose:     File load/save methods
// Author:      Jan Buchholz
// Created:     2025-11-19
// Changed:     2026-04-06
/////////////////////////////////////////////////////////////////////////////

#include "io.h"
#include <QDir>
#include "constants.h"

int Io::save(const QString& fileName, const QByteArray& payload) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) return MSG_WRITE_ERROR;
    file.write(payload);
    file.close();
    return MSG_OK;
}

int Io::load(const QString& fileName, QByteArray *payload) {
    payload->clear();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) return MSG_READ_ERROR;
    *payload = file.readAll();
    file.close();
    return MSG_OK;
}
