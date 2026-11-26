/////////////////////////////////////////////////////////////////////////////
// Name:        uilogic.cpp
// Purpose:     The main window (header)
// Author:      Jan Buchholz
// Created:     2025-10-13
// Changed:     2026-11-26
/////////////////////////////////////////////////////////////////////////////

#include "uilogic.h"
#include "listfontdialog.h"
#include "listitemdialog.h"
#include "iconlist.h"
#include <QJsonArray>
#include <QJsonDocument>
#include "thememanager.hpp"

UILogic::UILogic(QMainWindow *parent) : QObject(parent), m_mainWindow(parent) {
    createListWidget();
    createEditor();
    resetAll();
    connect(m_listWidget, &JBListWidget::currentItemChanged, this, &UILogic::onCurrentItemChanged);
    connect(m_listWidget, &JBListWidget::dropEventAccepted, this, &UILogic::onJBListDropEvent);
}

UILogic::~UILogic() {}

void UILogic::startUp(const QByteArray ba, QString workDir) {
    resetAll();
    if (ba.isEmpty()) return;
    jsonToData(ba);
    IconList iconList;
    for (auto &j : m_data) {
        QListWidgetItem *item = new QListWidgetItem(m_listWidget, c_userType);
        item->setData(Qt::UserRole, j.id);
        item->setIcon(ThemeManager::createThemedIcon(iconList.getIconPathAtIndex(j.icon)));
        item->setText(j.subject);
        m_listWidget->addItem(item);
        if (m_id < j.id) m_id = j.id;
    }
    if (m_listWidget->count() > 0) {
        QListWidgetItem *item = m_listWidget->item(0);
        m_listWidget->setCurrentItem(item);
    }
    m_Editor->setFocus();
}

void UILogic::resetAll() {
    m_listWidget->clear();
    m_Editor->blockSignals(true);
    m_Editor->clear();
    m_Editor->blockSignals(false);
    m_data.clear();
    m_id = 0;
}

void UILogic::createListWidget(){
    m_listWidget = new JBListWidget(m_mainWindow);
    m_listWidget->setMinimumWidth(150);
    m_listWidget->setViewMode(QListView::ListMode);
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listWidget->setSortingEnabled(false);
    m_listWidget->setDragEnabled(true);
    m_listWidget->setAcceptDrops(true);
    m_listWidget->setDropIndicatorShown(true);
    m_listWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_listWidget->setDragDropOverwriteMode(false);
    m_listWidget->setDefaultDropAction(Qt::MoveAction);
    m_listWidget->setFrameStyle(QStyleOptionFrame::None);
    m_listWidget->setAutoFillBackground(true);
}

void UILogic::createEditor() {
    m_Editor = new QTextEdit(m_mainWindow);
    m_Editor->setAcceptRichText(false);
    m_Editor->setWordWrapMode(QTextOption::WordWrap);
    m_Editor->setLineWrapMode(QTextEdit::WidgetWidth);
    m_Editor->setOverwriteMode(false);
    m_Editor->setUndoRedoEnabled(true);
    m_Editor->setFrameStyle(QStyleOptionFrame::None);
    m_Editor->setAutoFillBackground(true);
}

bool UILogic::addListItem() {
    bool b = false;
    ListItemDialog dlg;
    journalData data = {};
    if (dlg.Execute(ListItemDialog::create, &data.subject, &data.icon) == QDialog::Accepted) {
        m_id++;
        data.id = m_id;
        data.payload.clear();
        IconList iconList;
        QListWidgetItem *item = new QListWidgetItem(m_listWidget, c_userType);
        item->setData(Qt::UserRole, data.id);
        item->setIcon(ThemeManager::createThemedIcon(iconList.getIconPathAtIndex(data.icon)));
        item->setText(data.subject);
        m_listWidget->addItem(item);
        m_listWidget->setCurrentItem(item);
        m_data.append(data);
        m_Editor->clear();
        m_Editor->setFocus();
        b = true;
    }
    return b;
}

bool UILogic::editListItem() {
    bool changed = false;
    QList<QListWidgetItem*> itemList = m_listWidget->selectedItems();
    if (itemList.empty()) return changed;
    QListWidgetItem *item = itemList[0];
    IconList iconList;
    ListItemDialog dlg;
    int icon = -1;
    for (auto &p : m_data) {
        if (p.id == item->data(Qt::UserRole).toInt()) {
            icon = p.icon;
            break;
        }
    }
    QString text = item->text();
    if (dlg.Execute(ListItemDialog::edit, &text, &icon) == QDialog::Accepted) {
        item->setText(text);
        item->setIcon(ThemeManager::createThemedIcon(iconList.getIconPathAtIndex(icon)));
        for (auto &p : m_data) {
            if (p.id == item->data(Qt::UserRole).toInt()) {
                if (icon != p.icon || text != p.subject) changed = true;
                p.icon = icon;
                p.subject = text;
                break;
            }
        }
    }
    return changed;
}

bool UILogic::deleteListItem() {
    QList<QListWidgetItem*> itemList = m_listWidget->selectedItems();
    if (itemList.empty()) return false;
    QListWidgetItem *item_d = itemList[0];
    int id = item_d->data(Qt::UserRole).toInt();
    m_listWidget->takeItem(m_listWidget->row(item_d));
    for (int i = 0; i < m_data.count(); i++) {
        if (m_data[i].id == id) {
            m_data.remove(i);
            break;
        }
    }
    if (m_listWidget->count() > 0) {
        QListWidgetItem *item_c = m_listWidget->item(0);
        m_listWidget->setCurrentItem(item_c);
    } else {
        m_Editor->clear();
    }
    return true;
}

UILogic::journalData UILogic::getSelectedListItem() {
    QList<QListWidgetItem*> itemList = m_listWidget->selectedItems();
    if (itemList.empty()) return {};
    QListWidgetItem *item_d = itemList[0];
    int id = item_d->data(Qt::UserRole).toInt();
    for (auto &p : m_data) {
        if (p.id == id) return p;
    }
    return {};
}

void UILogic::setPayload(int id, QString payload) {
    for (auto &p : m_data) {
        if (p.id == id) {
            p.payload = payload;
            return;
        }
    }
}

QString UILogic::getPayLoad(int id) {
    for (auto &p : m_data) {
        if (p.id == id) return p.payload;
    }
    return "";
}

QJsonObject UILogic::journalDataToJson(journalData data) {
    QJsonObject json = {};
    json["id"] = data.id;
    json["icon"] = data.icon;
    json["subject"] = data.subject;
    json["payload"] = data.payload;
    return json;
}

UILogic::journalData UILogic::journalDataFromJson(QJsonObject json) {
    journalData data = {};
    if (QJsonValue v = json["id"]; v.isDouble()) data.id = v.toInt();
    if (QJsonValue v = json["icon"]; v.isDouble()) data.icon = v.toInt();
    if (QJsonValue v = json["subject"]; v.isString()) data.subject = v.toString();
    if (QJsonValue v = json["payload"]; v.isString()) data.payload = v.toString();
    return data;
}

QByteArray UILogic::dataToJson() {
    QJsonArray array = {};
    QList<QListWidgetItem*> itemList = m_listWidget->selectedItems();
    if (!itemList.empty()) {
        setPayload(itemList[0]->data(Qt::UserRole).toInt(), m_Editor->toPlainText());
    }
    for (int i = 0; i < m_listWidget->count(); i++) {
        QListWidgetItem* item = m_listWidget->item(i);
        int id = item->data(Qt::UserRole).toInt();
        for (auto &p : m_data) {
            if (p.id == id) {
                array.append(journalDataToJson(p));
                break;
            }
        }
    }
    QJsonDocument document(array);
    QByteArray ba = document.toJson();
    return ba;
}

void UILogic::jsonToData(QByteArray ba) {
    QJsonDocument document = QJsonDocument::fromJson(ba);
    if (document.isArray()) {
        QJsonArray array = document.array();
        for (const auto &a : std::as_const(array)) {
            QJsonObject json = a.toObject();
            journalData data = journalDataFromJson(json);
            m_data.append(data);
        }
    }
}

bool UILogic::isAnyItemSelected() {
    return !m_listWidget->selectedItems().empty();
}

int UILogic::getItemCount() {
    return m_listWidget->count();
}

void UILogic::openListSettingsDialog() {
    ListFontDialog dlg;
    QFont font = m_listWidget->font();
    int iconSize = m_listWidget->iconSize().width();
    if (dlg.Execute(&font, &iconSize) == QDialog::Accepted) {
        m_listWidget->setFont(font);
        m_listWidget->setIconSize(QSize(iconSize, iconSize));
    }
}

void UILogic::onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    if (previous) {
        setPayload(previous->data(Qt::UserRole).toInt(), m_Editor->toPlainText());
    }
    if (current) {
        QString md = getPayLoad(current->data(Qt::UserRole).toInt());
        m_Editor->blockSignals(true);
        m_Editor->setPlainText(md);
        m_Editor->blockSignals(false);
        m_Editor->setFocus();
    }
}

void UILogic::onJBListDropEvent() {
    emit listChanged();
}
