/////////////////////////////////////////////////////////////////////////////
// Name:        mainwindow.h
// Purpose:     The main window (header)
// Author:      Jan Buchholz
// Created:     2025-10-13
// Changed:     2026-04-12
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QMainWindow>
#include <QToolBar>
#include <QSplitter>
#include <QCloseEvent>
#include <QFontComboBox>
#include <QCheckBox>
#include <QLabel>
#include "dialogs.h"
#include "uilogic.h"
#include "cipherengine.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *e);

private:
    Ui::MainWindow *ui;
    QToolBar *m_mainToolBar, *m_mainListToolBar;
    QStatusBar *m_statusBar;
    QSplitter *m_mainSplitter, *m_treeSplitter;
    QFontComboBox *m_fontComboBox;
    QComboBox *m_fontSizeBox, *m_bulletBox;
    UILogic *mc_uiLogic;
    QString m_lastFolder;
    QString m_fileName;
    Dialogs *mc_dialogs;
    CipherEngine *m_cipherEngine;
    const QStringList fontSizeList = {"8", "9", "10", "11", "12", "13", "14", "16", "18"};
    const QStringList bulletList = {"\u2022", "\u25CF", "\u2605", "\u26AA", "\u26AB", "\u25FB", "\u25FC"};
    void createToolBars();
    void createSplitters();
    void createStatusBar();
    void populateMainMenu();
    void setConnections();
    void savePreferences();
    void loadPreferences();
    void setLockStatus();
    void fileNew();
    bool fileOpen();
    bool fileSave(bool saveAs = false);
    void settingsAfterLoad();
    void settingsAfterNew();
    void settingsAfterAddNote();
    void settingsAfterEditNote();
    void settingsAfterDeleteNote();

    int const MsgDisplayTime = 5000;

private slots:
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onSaveFileAs();
    void onListSettings();
    void onOptionsPassword();
    void onAppInfo();
    void onAppQuit();
    void onEditCopy();
    void onEditCut();
    void onEditPaste();
    void onEditSelectAll();
    void onEditUndo();
    void onEditRedo();
    void onEditInsert();
    void onToggleLock();
    void onAddNote();
    void onDeleteNote();
    void onEditNote();

    void onCurrentFontChanged(const QFont);
    void onCurrentTextChanged(const QString);
    void onTextChanged();
    void onListChanged();
};

