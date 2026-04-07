/////////////////////////////////////////////////////////////////////////////
// Name:        io.h
// Purpose:     File load/save methods (header)
// Author:      Jan Buchholz
// Created:     2025-11-19
// Changed:     2026-04-05
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QString>
#include <QObject>
#include <QByteArray>

class Io {

public:
    int save(const QString& fileName, const QByteArray& payload);
    int load(const QString& fileName, QByteArray *payload);

};

