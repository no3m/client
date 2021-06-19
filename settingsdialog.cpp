#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QSettings &s, QWidget *parent)  : QDialog(parent),settings(s)
{
  setupUi(this);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::acceptChanges);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::rejectChanges);
  connect(pbBrowse, &QPushButton::released, this, &SettingsDialog::openDirectoryDialog);
  connect(pbShadeColor, &QPushButton::released,
          this, &SettingsDialog::setMapShadeColor);

  loadSettings();
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::setMapShadeColor()
{
  QColor c;
  // options: QColorDialog::DontUseNativeDialog|QColorDialog::ShowAlphaChannel|QColorDialog::NoButtons
  c = QColorDialog::getColor(mapShadeColor,
                             this,
                             "Select Color",
                             QColorDialog::ShowAlphaChannel|QColorDialog::DontUseNativeDialog
                            );
  if (c.isValid()) {
      mapShadeColor = c;
  }

  QPixmap pixmap_fill(16, 16);
  pixmap_fill.fill(mapShadeColor.rgba());
  QIcon icon_fill(pixmap_fill);
  pbShadeColor->setIcon(icon_fill);
}

void SettingsDialog::loadSettings()
{
  serverIp->setText(settings.value(s_serverIp, s_serverIp_def).toString());
  serverPort->setText(settings.value(s_serverPort, s_serverPort_def).toString());
  serverAutoConnect->setChecked(settings.value(s_serverAutoConnect, s_serverAutoConnect_def).toBool());
  mapDirectory->setText(settings.value(s_mapDirectory, s_mapDirectory_def).toString());
  mapShadeColor = settings.value(s_mapShadeColor, s_mapShadeColor_def).value<QColor>();
  cbShadeUnavailable->setChecked(settings.value(s_mapShadeUnavailable, s_mapShadeUnavailable_def).toBool());
  cbTheme->setChecked(settings.value(s_darkPalette, s_darkPalette_def).toBool());

  QPixmap pixmap_fill(16, 16);
  pixmap_fill.fill(mapShadeColor.rgba());
  QIcon icon_fill(pixmap_fill);
  pbShadeColor->setIcon(icon_fill);
  //qDebug() << "loadsettings";
}

void SettingsDialog::acceptChanges()
{
  settings.setValue(s_serverIp, serverIp->text());
  settings.setValue(s_serverPort, serverPort->text());
  settings.setValue(s_serverAutoConnect, serverAutoConnect->isChecked());
  settings.setValue(s_mapDirectory, mapDirectory->text());
  settings.setValue(s_mapShadeColor, mapShadeColor);
  settings.setValue(s_mapShadeUnavailable, cbShadeUnavailable->isChecked());
  settings.setValue(s_darkPalette, cbTheme->isChecked());

  settings.sync();
  emit settingsUpdated();
  //qDebug() << "accepted";
  accept();
}

void SettingsDialog::rejectChanges()
{
  loadSettings();
  //qDebug() << "rejected";
  reject();
}

void SettingsDialog::openDirectoryDialog()
{
  QFileDialog *directoryDialog = new QFileDialog(this);
  directoryDialog->setFileMode(QFileDialog::Directory);
  directoryDialog->setOption(QFileDialog::ShowDirsOnly, true);

  QString directory = directoryDialog->getExistingDirectory(this, ("Map Files Directory"), mapDirectory->text());
  if (!directory.isNull() && !directory.isEmpty()){
    mapDirectory->setText(directory);
  }
  delete directoryDialog;
}
