#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
#pragma once

#include "defines.h"
#include "ui_settingsdialog.h"

class SettingsDialog : public QDialog, public Ui::SettingsDialog
{
  Q_OBJECT

public:
    SettingsDialog(QSettings &s, QWidget *parent = nullptr);
    ~SettingsDialog() override;

signals:
    void settingsUpdated();
    void updateMapImages();
    void webSocketReconnect();
    void toggleNightMapImage();
    void updateNightMapImages();

private:
    QSettings& settings;
    QColor mapShadeColor;

    void loadSettings();
    void acceptChanges();
    void rejectChanges();
    void browseFileDialog();
    void setMapShadeColor();

};

#endif
