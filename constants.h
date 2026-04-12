/////////////////////////////////////////////////////////////////////////////
// Name:        constants.h
// Purpose:     Constants and text snippets
// Author:      Jan Buchholz
// Created:     2025-10-13
// Changed:     2026-04-06
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QString>
#include <QStringList>

#define APPNAME "SecretJournalQt"
#define APP_FOLDER "SecretJournalQt"
#define SET_COMPANY "org.jan.buchholz"
#define SET_WGEOMETRY "mainwindow/geometry"
#define SET_WSTATE "mainwindow/windowState"
#define SET_LASTFOLDER "file/lastfolder"
#define SET_SSTATE "splitter/state"
#define SET_SYNC "options/sync"
#define SET_EDITORFONT "options/editorfont"
#define SET_LISTFONT "options/listfont"
#define SET_ICONSIZE "options/iconsize"
#define SET_BULLETSIDX "options/bulletsindex"
#define PLACEHOLDER " [*]"

int constexpr MAX_ICONSIZE = 24;
int constexpr MIN_ICONSIZE = 12;
int constexpr DEF_ICONSIZE = 16;

enum behavior {GET = 0, SET};

enum ErrorCode {
    MSG_OK = 0,
    MSG_READ_ERROR,
    MSG_WRITE_ERROR,
    MSG_TOO_SHORT_ERROR,
    MSG_INTERNAL_ERROR,
    MSG_SIGNATURE_ERROR,
    MSG_DECRYPTION_ERROR,
    MSG_FILE_CORRUPT_ERROR
};

inline const QStringList ERROR_MESSAGES = {
    "OK",
    "I/O error while reading the file.",
    "I/O error while writing the file.",
    "File is too short or incomplete.",
    "Internal error.",
    "File signature does not match.",
    "Decryption failed. Please check entered password.",
    "The file cannot be opened because it appears to be corrupted. The encrypted data could not be decrypted."
};

