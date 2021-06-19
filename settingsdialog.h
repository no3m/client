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

private:
    QSettings& settings;
    //QFileDialog *directoryDialog;
    QColor mapShadeColor;

    void loadSettings();
    void acceptChanges();
    void rejectChanges();
    void openDirectoryDialog();
    void setMapShadeColor();

};
