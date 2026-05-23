/////////////////////////////////////////////////////////////////////////////
// Name:        uilogic.h
// Purpose:     QListWidget & QTextEdit logic, JSON conversion (header)
// Author:      Jan Buchholz
// Created:     2025-10-13
// Changed:     2026-05-22
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <QObject>
#include <QTextEdit>
#include <QTextBrowser>
#include <QJsonObject>
#include <QByteArray>
#include <QMainWindow>
#include "jblistwidget.h"

class UILogic : public QObject {
    Q_OBJECT

public:
    explicit UILogic(QMainWindow *parent);
    ~UILogic();
    typedef struct {
        int id;
        int icon;
        QString subject;
        QString payload;
    } journalData;
    QListWidget* getListWidget() const {return m_listWidget;}
    QTextEdit* getEditor() const {return m_Editor;}
    void resetAll();
    void startUp(const QByteArray ba, QString workDir);
    bool addListItem();
    bool editListItem();
    bool deleteListItem();
    UILogic::journalData getSelectedListItem();
    bool getTextChangeIgnore() const {return m_textChangeIgnore;}
    void openListSettingsDialog();
    bool isAnyItemSelected();
    int getItemCount();
    QByteArray dataToJson();

private:
    QMainWindow* m_mainWindow;
    JBListWidget* m_listWidget;
    QTextEdit* m_Editor;
    QVector<journalData> m_data;
    int m_id;
    bool m_textChangeIgnore;
    void createListWidget();
    void createEditor();
    void setPayload(int id, QString payload);
    QString getPayLoad(int id);
    void jsonToData(QByteArray ba);
    QJsonObject journalDataToJson(journalData data);
    journalData journalDataFromJson(QJsonObject json);

private slots:
    void onCurrentItemChanged(QListWidgetItem*, QListWidgetItem*);
    void onJBListDropEvent();

signals:
    void listChanged();

};

