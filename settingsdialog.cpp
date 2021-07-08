#include "settingsdialog.h"

SettingsDialog::SettingsDialog(QSettings &s, QWidget *parent)  : QDialog(parent),settings(s)
{
  setupUi(this);

  connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::acceptChanges);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::rejectChanges);
  connect(pbBrowse, &QPushButton::released, this, &SettingsDialog::browseFileDialog);
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
  mapFile->setText(settings.value(s_mapFile, s_mapFile_def).toString());
  mapShadeColor = settings.value(s_mapShadeColor, s_mapShadeColor_def).value<QColor>();
  cbShadeUnavailable->setChecked(settings.value(s_mapShadeUnavailable, s_mapShadeUnavailable_def).toBool());
  cbTheme->setChecked(settings.value(s_darkPalette, s_darkPalette_def).toBool());
  cbNightShade->setChecked(settings.value(s_nightshading, s_nightshading_def).toBool());
  gridLocator->setText(settings.value(s_gridlocator, s_gridlocator_def).toString());
  cbNightTransparency->setCurrentIndex(settings.value(s_nighttransparency,s_nighttransparency_def).toInt());

  QPixmap pixmap_fill(16, 16);
  pixmap_fill.fill(mapShadeColor.rgba());
  QIcon icon_fill(pixmap_fill);
  pbShadeColor->setIcon(icon_fill);
  //qDebug() << "loadsettings";
}

void SettingsDialog::acceptChanges()
{
  bool updateMaps = false;
  bool updateWebsocket = false;
  bool updateNightMaps = false;
  bool toggleNightMap = false;
  if (settings.value(s_mapFile, s_mapFile_def).toString() != mapFile->text()
      || settings.value(s_gridlocator, s_gridlocator_def).toString() != gridLocator->text()) {
    updateMaps = true;
  }
  if (settings.value(s_serverIp, s_serverIp_def).toString() != serverIp->text()
      || settings.value(s_serverPort, s_serverPort_def).toString() != serverPort->text()) {
    updateWebsocket = true;
  }
  if (settings.value(s_nightshading, s_nightshading_def).toBool() != cbNightShade->isChecked()) {
    toggleNightMap = true;
  }
  if (settings.value(s_nighttransparency,s_nighttransparency_def).toInt() !=
          cbNightTransparency->currentIndex()) {
    updateNightMaps = true;
  }

  settings.setValue(s_serverIp, serverIp->text());
  settings.setValue(s_serverPort, serverPort->text());
  settings.setValue(s_serverAutoConnect, serverAutoConnect->isChecked());
  settings.setValue(s_mapFile, mapFile->text());
  settings.setValue(s_mapShadeColor, mapShadeColor);
  settings.setValue(s_mapShadeUnavailable, cbShadeUnavailable->isChecked());
  settings.setValue(s_darkPalette, cbTheme->isChecked());
  settings.setValue(s_gridlocator, gridLocator->text());
  settings.setValue(s_nightshading, cbNightShade->isChecked());
  settings.setValue(s_nighttransparency, cbNightTransparency->currentIndex());

  settings.sync();
  emit settingsUpdated();
  if (updateMaps) {
    emit updateMapImages();
  } else if (updateNightMaps) {
    emit updateNightMapImages();
  } else if (toggleNightMap) {
    emit toggleNightMapImage();
  }
  if (updateWebsocket) {
    emit webSocketReconnect();
  }
  //qDebug() << "accepted";
  accept();
}

void SettingsDialog::rejectChanges()
{
  loadSettings();
  //qDebug() << "rejected";
  reject();
}

void SettingsDialog::browseFileDialog()
{
  QFileDialog *fileDialog = new QFileDialog(this);
  fileDialog->setFileMode(QFileDialog::ExistingFile);

  QString filename = fileDialog->getOpenFileName(this,
                                                 tr("Map File"),
                                                 mapFile->text(),
                                                 tr("Images (*.png *.jpg *.PNG *.JPG)"));

  if (!filename.isNull() && !filename.isEmpty()){
    mapFile->setText(filename);
  }
  delete fileDialog;
}
