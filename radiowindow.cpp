#include "radiowindow.h"

RadioWindow::RadioWindow(QStringList args, QWidget *parent) : QWidget(parent)
{
  radioNrStr = QStringLiteral("");
  if (args.size() > 1) {
    radioNrStr = args[1]; // radio number
  } else {
    qWarning() << "No radio number argument given";
    exit(0);
  }
  radioNr = radioNrStr.toInt();
  if (radioNr <= 0 || radioNr > 8) {
    qWarning() << "Radio number out of range";
    exit(0);
  }

  settings=nullptr;
  settingsdialog=nullptr;
  GraphicsView = nullptr;
  GraphicsScene = nullptr;
  //GraphicsEllipse = nullptr;
  GraphicsPixmap = nullptr;
  GraphicsLegend = nullptr;
  webSocket = nullptr;
  m_timer = nullptr;
  currentBearing = -1;

  setupUi(this);
  //qDebug() << qApp->style()->metaObject()->className();
  //qDebug() << "frame w " << frame->width() << " h " << frame->height();
  setWindowTitle("Radio "+radioNrStr);

  // dimensions based on UI file frame
  kGraphicsSceneHeight   = frame->height(); // 300
  kGraphicsSceneWidth    = frame->width(); // 350
  int smallSide = kGraphicsSceneHeight;
  if (kGraphicsSceneWidth < kGraphicsSceneHeight)
    smallSide = kGraphicsSceneWidth;

  kMapWidth              = smallSide * 4 / 5; // 23 / 30; //4 / 5; //5 / 6; // 250
  kMapHeight             = kMapWidth;

  kGraphicsEllipseShrink = 0; // kMapWidth / 16; // 10
  kGraphicsLineLength    = kMapWidth / 2; // * 59 / 125; // 116
  kGraphicsLabelMargin   = kMapWidth / 10; //* 16 / 250; // 15
  kGraphicsFontSize      = kMapWidth * 6  / 125; // 12

  //qDebug() << radioNr;
  settings = new QSettings("softrx-client", "settings"+radioNrStr);
  settingsdialog = new SettingsDialog(*settings, this);
  settingsdialog->hide();

  restoreGeometry(settings->value("geometry").toByteArray());
  mapZoom = settings->value(s_mapZoom, s_mapZoom_def).toInt();
  //loadMaps();

  defaultPalette = qApp->palette();
  setPalette();

  connectionLED(false);

  connectSignals();

  layout = new QStackedLayout;
  layout->setContentsMargins(0,0,0,0);
  frame->setLayout(layout);

  // kDispList
  gridLayout = new QGridLayout();
  buttonWidget = new QWidget;
  buttonWidget->setLayout(gridLayout);
  layout->addWidget(buttonWidget);

  // kDispCompass
  createGraphicsView();
  createGraphicsLegend();
  updateGraphicsPixmap();
  GraphicsLines.clear();
  GraphicsText.clear();
  GraphicsEllipses.clear();

  // kDispNone
  blankWidget = new QWidget;
  layout->addWidget(blankWidget);
  layout->setCurrentIndex(kDispNone);

  m_timer = new QTimer();
  openWebSocket();

  // test
  double lat = 0.0;
  double lng = 0.0;
  //if (convertMaidenheadToCoordinates(QStringLiteral("en91wr"), &lat, &lng)) {
  if (locator2longlat(&lng, &lat, QString("en91wr").toLatin1().data())) {
    qDebug() << "Lat: " << lat << " Long: " << lng;
  }

  double dist = 0.0;
  double azim = 0.0;
  if (qrb(lng, lat, -0.375252, 51.453151, &dist, &azim)) {
    qDebug() << "Distance: " << dist << " Bearing: " << azim;
  }

  std::time_t start = std::time(0);
  QImage rectMap("/home/eric/.xplanet/images/6_earth_blue-water_boundaries.png");
  //QImage rectMap("/home/eric/.xplanet/images/6_earth_blue-water_boundaries-16k.png");
  //QImage rectMap("/home/eric/.xplanet/images/4_no_ice_clouds_mts_boundaries_4k.jpg");
  QImage azimuthalMap(2048,2048,QImage::Format_ARGB32);

  double latRad = lat * M_PI / 180.0;
  double lngRad = lng * M_PI / 180.0;
  for (int x =0; x < azimuthalMap.width(); ++x) {
    for (int y=0; y < azimuthalMap.height(); ++y) {
      double distance;
      double bearing;
      if (pixelAz2DistAz(x,y,
                         azimuthalMap.width(),20000.0,
                         distance, bearing)) {

        double longitude;
        double latitude;
        if (distAz2LongLat(distance, bearing,
                           lngRad, latRad,
                           longitude, latitude)) {

          int rectX;
          int rectY;
          if (longlat2PixelRect(longitude, latitude,
                                rectMap.width(), rectMap.height(),
                                rectX, rectY)) {

            azimuthalMap.setPixelColor(x, y, rectMap.pixelColor(rectX, rectY));
          }
        }
      }
    }
  }
  //azimuthalMap.save("/home/eric/.config/softrx-client/azimuthal-eq.png");
  int steps = sqrt(azimuthalMap.width() - kMapWidth) / 2;
  for (int i=steps; i>0; --i) {
    QImage image(azimuthalMap.scaled(
                 kMapWidth + i * i * 2,
                 kMapHeight + i * i * 2,
                 Qt::KeepAspectRatio,
                 Qt::SmoothTransformation
               ));
    image = image.copy(QRect(image.width()/2 - kMapWidth/2,
                             image.height()/2 - kMapHeight/2,
                             kMapWidth,
                             kMapHeight) );
    //image.save(QString("/home/eric/.config/softrx-client/azimuthal-eq%1.png").arg(i));
    QImage clipped(kMapWidth,kMapHeight,QImage::Format_ARGB32);
    clipped.fill(Qt::transparent);
    QBrush brush(image);
    QPainter p(&clipped);
    p.setBrush(brush);
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawEllipse(0,0,kMapWidth,kMapHeight);
    p.end();

    QPixmap *map = new QPixmap(QPixmap::fromImage(clipped));
    AzMaps << map;
  }
  qDebug() << std::time(0) - start;

}

RadioWindow::~RadioWindow()
{

}

void RadioWindow::setPalette()
{
  //QColor darkGray(53, 53, 53);
  //QColor gray(128, 128, 128);
  //QColor black(25, 25, 25);
  //QColor blue(42, 130, 218);
  //QPalette *palette = new QPalette();

  if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {

    QPalette palette;
    palette = qApp->palette();

    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, QColor(Qt::white));
    //palette.setColor(QPalette::Base, QColor(25, 25, 25));
    //palette.setColor(QPalette::Base, QColor(35, 35, 35)); // a
    palette.setColor(QPalette::Base,QColor(42,42,42)); // c
    //palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::AlternateBase,QColor(66,66,66)); // c
    palette.setColor(QPalette::ToolTipBase, QColor(Qt::black));
    //palette.setColor(QPalette::ToolTipBase, QColor(25, 25, 25)); // a
    palette.setColor(QPalette::ToolTipText, QColor(Qt::white));
    palette.setColor(QPalette::Text, QColor(Qt::white));
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, QColor(Qt::white));
    palette.setColor(QPalette::BrightText, QColor(Qt::red));
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    //palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter()); // b
    palette.setColor(QPalette::HighlightedText, QColor(Qt::black));
    //palette.setColor(QPalette::HighlightedText, QColor(35, 35, 35)); // a
    //palette.setColor(QPalette::HighlightedText,Qt::white); // c
    palette.setColor(QPalette::Active, QPalette::Button, QColor(53, 53, 53)); // a
    //palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(Qt::darkGray)); // a
    palette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127)); // c
    //palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(Qt::darkGray)); // a
    palette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127)); // c
    //palette.setColor(QPalette::Disabled, QPalette::Text, QColor(Qt::darkGray)); // a
    palette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127)); // c
    palette.setColor(QPalette::Disabled, QPalette::Light, QColor(53, 53, 53)); // a
    palette.setColor(QPalette::Dark,QColor(35,35,35)); // c
    palette.setColor(QPalette::Shadow,QColor(20,20,20)); // c
    palette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80)); // c
    palette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127)); // c

  /*
    // similar to wsjtx dark style
    palette.setColor(QPalette::Window, QColor("#19232D"));
    palette.setColor(QPalette::WindowText, QColor("#F0F0F0"));
    palette.setColor(QPalette::Base,QColor("#19232D").darker());
    palette.setColor(QPalette::AlternateBase,QColor("#19232D").lighter());
    palette.setColor(QPalette::ToolTipBase, QColor("#148CD2"));
    palette.setColor(QPalette::ToolTipText, QColor("#19232D"));
    palette.setColor(QPalette::Text, QColor(Qt::white));
    palette.setColor(QPalette::Button, QColor("#505F69"));
    palette.setColor(QPalette::ButtonText, QColor("#F0F0F0"));
    palette.setColor(QPalette::BrightText, QColor(Qt::red));
    palette.setColor(QPalette::Link, QColor("#F0F0F0").darker());
    palette.setColor(QPalette::Highlight, QColor("#1464A0"));
    palette.setColor(QPalette::HighlightedText, QColor("#F0F0F0"));
    palette.setColor(QPalette::Active, QPalette::Button, QColor("#505F69"));
    palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#32414B"));
    palette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor("#787878"));
    palette.setColor(QPalette::Disabled,QPalette::WindowText,QColor("#787878"));
    palette.setColor(QPalette::Disabled,QPalette::Text,QColor("#787878"));
    palette.setColor(QPalette::Disabled, QPalette::Light, QColor("#19232D"));
    palette.setColor(QPalette::Dark,QColor("#19232D").darker());
    palette.setColor(QPalette::Shadow,QColor("#19232D").darker());
    palette.setColor(QPalette::Disabled,QPalette::Highlight,QColor("#19232D").lighter());
    palette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor("#787878"));
  */

    qApp->setPalette(palette);

  } else {
    qApp->setPalette(defaultPalette);
  }
}

void RadioWindow::openWebSocket()
{
  if (webSocket == nullptr) {
    if (!settings->value(s_serverIp, s_serverIp_def).toString().isEmpty() &&
        !settings->value(s_serverPort, s_serverPort_def).toString().isEmpty()) {
      webSocket = new QWebSocket();
      connect(webSocket, &QWebSocket::connected,
              this, &RadioWindow::webSocketConnected);
      connect(webSocket, &QWebSocket::disconnected,
              this, &RadioWindow::webSocketClosed);
      connect(m_timer,&QTimer::timeout,this,&RadioWindow::webSocketReconnect);
      QString url="ws://"+settings->value(s_serverIp, s_serverIp_def).toString()+
                  ":"+settings->value(s_serverPort, s_serverPort_def).toString();
      webSocket->open(QUrl(url));
    }
  }
}
void RadioWindow::webSocketReconnect()
{
  //qDebug()<<"try to reconnect!";
  webSocket->abort();
  if (!settings->value(s_serverIp, s_serverIp_def).toString().isEmpty() &&
      !settings->value(s_serverPort, s_serverPort_def).toString().isEmpty()) {
    QString url="ws://"+settings->value(s_serverIp, s_serverIp_def).toString()+
                  ":"+settings->value(s_serverPort, s_serverPort_def).toString();
    webSocket->open(QUrl(url));
  }
}
void RadioWindow::webSocketConnected()
{
  m_timer->stop();
  //qDebug() << "webSocketConnected";
  connect(webSocket,
          &QWebSocket::binaryMessageReceived,
          this,
          &RadioWindow::messageReceived);
  QJsonObject object;
  object.insert("radio", QJsonValue::fromVariant(radioNr));
  sendData(object);
  connectionLED(true);
}
void RadioWindow::webSocketClosed()
{
  //qDebug() << "webSocketClosed";
  connectionLED(false);
  cleanup();
  m_timer->start(3000);
}
void RadioWindow::messageReceived(const QByteArray &message)
{
  //qDebug() << "message: " << message;
  const QJsonDocument doc = QJsonDocument::fromJson(message);
  const QJsonObject object = doc.object();
  if (!object.contains("object") || !object.contains("method")) return;
  if (!object.value("object").isString() || !object.value("method").isString()) return;

  // handle some basic validations
  //if (object.contains("text") && !object.value("text").isString()) return;


  // radioName
  if (object.value("object").toString() == "radioName") {
    if (object.value("method").toString() == "setText") {
      if (object.contains("text") && object.value("text").isString()) {
        radioName->setText(object.value("text").toString());
      }
    } else if (object.value("method").toString() == "status") {
      if (object.contains("state") && object.value("state").isBool()) {
        if (object.value("state").toBool()) {
          if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
            radioName->setStyleSheet("QLabel { color : white; font-weight:600; }");
          } else {
            radioName->setStyleSheet("QLabel { color : #0032ff; font-weight:600; }");
          }
        } else {
          radioName->setStyleSheet("QLabel { color : red; font-weight:600; }");
        }
      }
    }
  // cbBand
  } else if (object.value("object").toString() == "cbBand") {
    if (object.value("method").toString() == "addItem") {
      if (object.contains("labels") && object.value("labels").isArray()) {
        cbBand->blockSignals(true);
        cbBand->clear();
        const QJsonArray labels = object.value("labels").toArray();
        for (const QJsonValue &label : qAsConst(labels)) {
          if (label.isString()) {
            cbBand->addItem(label.toString());
          }
        }
        cbBand->blockSignals(false);
      }
    } else if (object.value("method").toString() == "setCurrentText") {
      if (object.contains("text") && object.value("text").isString()) {
        cbBand->blockSignals(true);
        cbBand->setCurrentText(object.value("text").toString());
        cbBand->blockSignals(false);
      }
    } else if (object.value("method").toString() == "setEnabled") {
      if (object.contains("state") && object.value("state").isBool()) {
        cbBand->setEnabled(object.value("state").toBool());
      }
    }
  // cbGroup
  } else if (object.value("object").toString() == "cbGroup") {
    if (object.value("method").toString() == "setCurrentText") {
      if (object.contains("text") && object.value("text").isString()) {
        cbGroup->blockSignals(true);
        cbGroup->setCurrentText(object.value("text").toString());
        cbGroup->blockSignals(false);
      }
    } else if (object.value("method").toString() == "addItem") {
      if (object.contains("labels") && object.value("labels").isArray()) {
        cbGroup->blockSignals(true);
        cbGroup->clear();
        const QJsonArray labels = object.value("labels").toArray();
        for (const QJsonValue &label : qAsConst(labels)) {
          if (label.isString()) {
            cbGroup->addItem(label.toString());
          }
        }
        cbGroup->blockSignals(false);
      }
    } else if (object.value("method").toString() == "setEnabled") {
      if (object.contains("state") && object.value("state").isBool()) {
        cbGroup->setEnabled(object.value("state").toBool());
      }
    }
  // pbLock
  } else if (object.value("object").toString() == "pbLock") {
    if (object.value("method").toString() == "status") {
      if (object.contains("state") && object.value("state").isBool()) {
        if (object.value("state").toBool()) {
          if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
            pbLock->setStyleSheet("QPushButton {background-color:red;color:black;}");
          } else {
            pbLock->setStyleSheet("QPushButton {background-color:red;}");
          }
        } else {
          pbLock->setStyleSheet("");
        }
        /*
        //testing
        QPalette pal = pbLock->palette();
        pal.setColor(QPalette::Button, QColor(Qt::red));
        //pal.setColor(QPalette::Window, QColor(Qt::red));
        pbLock->setAutoFillBackground(true);
        pbLock->setPalette(pal);
        pbLock->update();
        */
      }
    }
  // pbScan
  } else if (object.value("object").toString() == "pbScan") {
    if (object.value("method").toString() == "status") {
      if (object.contains("state") && object.value("state").isBool()) {
        if (object.value("state").toBool()) {
          if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
            pbScan->setStyleSheet("QPushButton {background-color:red;color:black;}");
          } else {
            pbScan->setStyleSheet("QPushButton {background-color:red;}");
          }
        } else {
          pbScan->setStyleSheet("");
        }
      }
    } else if (object.value("method").toString() == "setEnabled") {
      if (object.contains("state") && object.value("state").isBool()) {
        pbScan->setEnabled(object.value("state").toBool());
      }
    }
  // cbScanDelay
  } else if (object.value("object").toString() == "cbScanDelay") {
    if (object.value("method").toString() == "setEnabled") {
      if (object.contains("state") && object.value("state").isBool()) {
        cbScanDelay->setEnabled(object.value("state").toBool());
      }
    } else if (object.value("method").toString() == "setCurrentIndex") {
      if (object.contains("index")) { // && object.value("index").isDouble()
        cbScanDelay->blockSignals(true);
        cbScanDelay->setCurrentIndex(object.value("index").toInt());
        cbScanDelay->blockSignals(false);
      }
    } else if (object.value("method").toString() == "addItem") {
      if (object.contains("labels") && object.value("labels").isArray()) {
        cbScanDelay->blockSignals(true);
        cbScanDelay->clear();
        const QJsonArray labels = object.value("labels").toArray();
        for (const QJsonValue &label : qAsConst(labels)) {
          if (label.isString()) {
            cbScanDelay->addItem(label.toString()+"ms");
          }
        }
        cbScanDelay->blockSignals(false);
      }
    }
  // pbScanEnabled
  } else if (object.value("object").toString() == "pbScanEnabled") {
    if (object.value("method").toString() == "status") {
      if (object.contains("state") && object.value("state").isBool()) {
        if (object.value("state").toBool()) {
          if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
            pbScanEnabled->setStyleSheet("QPushButton {background-color:#66ff66;color:black;}");
          } else {
            pbScanEnabled->setStyleSheet("QPushButton {background-color:#66ff66;}");
          }
        } else {
          pbScanEnabled->setStyleSheet("");
        }
      }
    }
  // pbTrack
  } else if (object.value("object").toString() == "pbTrack") {
    if (object.value("method").toString() == "status") {
      if (object.contains("state") && object.value("state").isBool()) {
        if (object.value("state").toBool()) {
          if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
            pbTrack->setStyleSheet("QPushButton {background-color:red;color:black;}");
          } else {
            pbTrack->setStyleSheet("QPushButton {background-color:red;}");
          }
        } else {
          pbTrack->setStyleSheet("");
        }
      }
    } else if (object.value("method").toString() == "setEnabled") {
      if (object.contains("state") && object.value("state").isBool()) {
        pbTrack->setEnabled(object.value("state").toBool());
      }
    }
  // frame
  } else if (object.value("object").toString() == "frame") {
    if (object.value("method").toString() == "setEnabled") {
      if (object.contains("state") && object.value("state").isBool()) {
        frame->setEnabled(object.value("state").toBool());
      }
    }
  // cbLinked
  } else if (object.value("object").toString() == "cbLinked") {
    if (object.value("method").toString() == "setEnabled") {
      if (object.contains("state") && object.value("state").isBool()) {
        cbLinked->setEnabled(object.value("state").toBool());
      }

    } else if (object.value("method").toString() == "addItem") {
      if (object.contains("items") && object.value("items").isArray()) {
        cbLinked->blockSignals(true);
        cbLinked->clear();
        const QJsonArray items = object.value("items").toArray();
        for (int i=0; i < items.size(); ++i) {
          if (items.at(i).isObject()) {
            const QJsonObject item = items.at(i).toObject();
            if (item.contains("label") && item.contains("enabled")
                && item.value("label").isString()
                && item.value("enabled").isBool()
            ) {
              cbLinked->addItem(item.value("label").toString());
              if (!item.value("enabled").toBool()) {
                setComboBoxItemEnabled(cbLinked, i, false);
              }
            }
          }
        }
        cbLinked->blockSignals(false);
      }
    } else if (object.value("method").toString() == "setCurrentIndex") {
      if (object.contains("index")) { // && object.value("index").isDouble()
        cbLinked->blockSignals(true);
        cbLinked->setCurrentIndex(object.value("index").toInt());
        cbLinked->blockSignals(false);
      }
    }
  // bearingLabel
  } else if (object.value("object").toString() == "bearingLabel") {
    if (object.value("method").toString() == "setText") {
      if (object.contains("text") && object.value("text").isString()) {
        if (object.value("text").toString() != ""
            && isValidAngle(object.value("text").toInt())) {
          currentBearing = object.value("text").toInt();
          bearingLabel->setText(object.value("text").toString()+"°");
        } else {
          currentBearing = 0;
          bearingLabel->setText("");
        }
      }
    }
  // GraphicsEllipse
  } else if (object.value("object").toString() == "GraphicsEllipse") {
    if (object.value("method").toString() == "update") {
      if (object.contains("ellipses") && object.value("ellipses").isArray()) {
        if (GraphicsEllipses.count()) {
          for (const auto &ellispseitem : qAsConst(GraphicsEllipses)) {
            GraphicsScene->removeItem(ellispseitem);
            delete ellispseitem;
          }
          GraphicsEllipses.clear();
        }
        const QJsonArray ellipses = object.value("ellipses").toArray();
        for (const QJsonValue &ellipse : qAsConst(ellipses)) {
          if (ellipse.isObject()) {
            const QJsonObject attributes = ellipse.toObject();
            if (attributes.contains("start_deg") && isValidAngle(attributes.value("start_deg").toInt())
                && attributes.contains("stop_deg") && isValidAngle(attributes.value("stop_deg").toInt())
                && attributes.contains("state")
              ) {
                updateGraphicsEllipse( attributes.value("start_deg").toInt(),
                                       attributes.value("stop_deg").toInt(),
                                       attributes.value("state").toInt() );
            }
          }
        }
      }
    }
  // GraphicsLines
  } else if (object.value("object").toString() == "GraphicsLines") {
    if (object.value("method").toString() == "update") {
      if (object.contains("angles") && object.value("angles").isArray()) {
        if (GraphicsLines.count()) {
          // remove current lines
          for (const auto &line : qAsConst(GraphicsLines)) {
            GraphicsScene->removeItem(line);
            delete line;
          }
          GraphicsLines.clear();
        }
        const QJsonArray angles = object.value("angles").toArray();
        for (const QJsonValue &angle : qAsConst(angles)) {
          if (isValidAngle(angle.toInt())) {
            addGraphicsLine(angle.toInt());
          }
        }
      }
    }
  // GraphicsLabels
  } else if (object.value("object").toString() == "GraphicsLabels") {
    if (object.value("method").toString() == "update") {
      if (object.contains("labels") && object.value("labels").isArray()) {
        // remove current labels
        if (GraphicsText.count()) {
          for (const auto &label : qAsConst(GraphicsText)) {
            GraphicsScene->removeItem(label);
            delete label;
          }
          GraphicsText.clear();
        }
        const QJsonArray labels = object.value("labels").toArray();
        for (const QJsonValue &label : qAsConst(labels)) {
          if (label.isObject()) {
            const QJsonObject attributes = label.toObject();
            if (attributes.contains("angle") && isValidAngle(attributes.value("angle").toInt())
                && attributes.contains("text") && attributes.value("text").isString()
                && attributes.contains("state") //&& attributes.value("state").isDouble()
              ) {

              addGraphicsLabel(attributes.value("angle").toInt(),
                               attributes.value("text").toString(),
                               attributes.value("state").toInt() );
            }
          }
        }
      }
    }
  // AntennaButtons
  } else if (object.value("object").toString() == "AntennaButtons") {
    if (object.value("method").toString() == "create") {
      if (object.contains("buttons") && object.value("buttons").isArray()) {
        if (antennaButtons.count()) {
          for (const auto &Button : qAsConst(antennaButtons)) {
            if (Button.button != nullptr) {
              delete Button.button;
            }
          }
          antennaButtons.clear();
        }
        const QJsonArray buttons = object.value("buttons").toArray();
        for (const QJsonValue &button : qAsConst(buttons)) {
          if (button.isObject()) {
            const QJsonObject attributes = button.toObject();
            if (attributes.contains("antenna") && attributes.contains("label")
                && attributes.value("label").isString() // && attributes.value("antenna").isDouble()
            ) {
              antennaButtons << AntennaButton(nullptr,
                                              attributes.value("label").toString(),
                                              attributes.value("antenna").toInt()
                                             );
            }
          }
        }
        if (antennaButtons.count()) {
          createButtons();
        }
      }
    } else if (object.value("method").toString() == "update") {
      if (object.contains("buttons") && object.value("buttons").isArray()) {
        const QJsonArray buttons = object.value("buttons").toArray();
        for (const QJsonValue &button : qAsConst(buttons)) {
          if (button.isObject()) {
            const QJsonObject attributes = button.toObject();
            if (attributes.contains("antenna") && attributes.contains("state")) { // && .isDouble()?
              updateAntennaButton(attributes.value("antenna").toInt(),
                                  attributes.value("state").toInt() );
            }
          }
        }
      }
    }
  // pttLabel
  } else if (object.value("object").toString() == "ptt") {
    if (object.value("method").toString() == "state") {
      if (object.contains("state") && object.value("state").isBool()) {
        if (object.value("state").toBool()) {
          pttLabel->setText("TX");
          pttLabel->setStyleSheet("QLabel { color: red;}");
        } else {
          pttLabel->setText("RX");
          if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
            pttLabel->setStyleSheet("QLabel { color: #99ff66;}");
          } else {
            pttLabel->setStyleSheet("QLabel {color: green;}");
          }
        }
      }
    }
  // StackedLayout
  } else if (object.value("object").toString() == "StackedLayout") {
    if (object.value("method").toString() == "setCurrentIndex") {
      if (object.contains("index")) { // && object.value("index").isDouble() ?
        layout->setCurrentIndex(object.value("index").toInt());
      }
    }
  // hpfLabel
  } else if (object.value("object").toString() == "hpfLabel") {
    if (object.value("method").toString() == "setText") {
      if (object.contains("text") && object.value("text").isString()) {
        hpfLabel->setText(object.value("text").toString());
        if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
          hpfLabel->setStyleSheet("QLabel {color: #ff00ff;}");
        } else {
          hpfLabel->setStyleSheet("QLabel {color: purple;}");
        }
      }
    }
  // bpfLabel
  } else if (object.value("object").toString() == "bpfLabel") {
    if (object.value("method").toString() == "setText") {
      if (object.contains("text") && object.value("text").isString()) {
        bpfLabel->setText(object.value("text").toString());
        if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
          bpfLabel->setStyleSheet("QLabel {color: #ff00ff;}");
        } else {
          bpfLabel->setStyleSheet("QLabel {color: purple;}");
        }
      }
    }
  // auxLabel
  } else if (object.value("object").toString() == "auxLabel") {
    if (object.value("method").toString() == "setText") {
      if (object.contains("text") && object.value("text").isString()) {
        auxLabel->setText(object.value("text").toString());
        if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
          auxLabel->setStyleSheet("QLabel {color: #00fffa;}");
        } else {
          auxLabel->setStyleSheet("QLabel {color: blue;}");
        }
      }
    }
  // gainLabel
  } else if (object.value("object").toString() == "gainLabel") {
    if (object.value("method").toString() == "setText") {
      if (object.contains("text") && object.value("text").isString()) {
        gainLabel->setText(object.value("text").toString());
      }
    }
  }

}

void RadioWindow::closeEvent(QCloseEvent *event)
{
  for (const auto &map : qAsConst(AzMaps)) {
    delete map;
  }
  for (const auto &Button : qAsConst(antennaButtons)) {
    if (Button.button != nullptr) {
      delete Button.button;
    }
  }
  for (const auto &label : qAsConst(GraphicsText)) {
    GraphicsScene->removeItem(label);
    delete label;
  }
  for (const auto &line : qAsConst(GraphicsLines)) {
    GraphicsScene->removeItem(line);
    delete line;
  }
  for (const auto &ellispseitem : qAsConst(GraphicsEllipses)) {
    GraphicsScene->removeItem(ellispseitem);
    delete ellispseitem;
  }
  delete GraphicsPixmap;
  blankWidget->deleteLater();
  buttonWidget->deleteLater();
  gridLayout->deleteLater();
  layout->deleteLater();
  GraphicsScene->deleteLater();
  GraphicsView->deleteLater();

  settings->setValue(s_mapZoom, mapZoom);
  settings->setValue("geometry", saveGeometry());
  settings->sync();
  settings->deleteLater();
  settingsdialog->deleteLater();

  webSocket->abort();
  webSocket->deleteLater();
  //delete webSocket;

  m_timer->stop();
  m_timer->deleteLater();

  //qDebug() << "closing";
  event->accept();
  exit ( 0 );
}

void RadioWindow::cleanup()
{
  // called when websocket is disconnected
  layout->setCurrentIndex(kDispNone);
}

void RadioWindow::openSettingsDialog()
{
  settingsdialog->show();
}

void RadioWindow::settingsUpdated()
{
  QPixmapCache::clear(); // needed to update combobox, etc on palette change
  setPalette();

  loadMaps();
  updateGraphicsPixmap();
  if (webSocket->state() == QAbstractSocket::ConnectedState) {
    connectionLED(true);
  } else {
    connectionLED(false);
  }

  // refesh display elements
  QJsonObject object;
  object.insert("radio", QJsonValue::fromVariant(radioNr));
  sendData(object);
  //qDebug() << "settingsUpdated";
}

void RadioWindow::connectSignals()
{
  connect(settingsdialog, &SettingsDialog::settingsUpdated,
          this, &RadioWindow::settingsUpdated);

  connect(cbBand, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &RadioWindow::cbBandChanged);
  connect(cbGroup, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &RadioWindow::cbGroupChanged);
  connect(pbLock, &QPushButton::released,
          this, &RadioWindow::toggleAntennaLock);
  connect(pbScan, &QPushButton::released,
          this, &RadioWindow::toggleAntennaScan);
  connect(cbScanDelay, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &RadioWindow::cbScanDelayChanged);
  connect(pbTrack, &QPushButton::released,
          this, &RadioWindow::toggleAntennaTracking);
  connect(cbLinked, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &RadioWindow::cbLinkedChanged);
  connect(pbSettings, &QPushButton::released,
          this, &RadioWindow::openSettingsDialog);
  connect(pbScanEnabled, &QPushButton::released,
          this, &RadioWindow::toggleScanEnabled);
  connect(pbSwapAntennas, &QPushButton::released,
          this, &RadioWindow::swapAntennas);

  // zooming buttons only on Pi/touchscreen UI
  QPushButton *zoomUpButton = this->findChild<QPushButton*>("zoomUp");
  if (zoomUpButton) {
    connect(zoomUpButton, &QPushButton::released,
            this, &RadioWindow::zoomIn);
  }
  QPushButton *zoomDownButton = this->findChild<QPushButton*>("zoomDown");
  if (zoomDownButton) {
    connect(zoomDownButton, &QPushButton::released,
            this, &RadioWindow::zoomOut);
  }


}

void RadioWindow::connectionLED(bool state)
{
  if (state) {
    if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
      connectionStatus->setStyleSheet("QLabel {background-color:#99ff66;border-radius:6px;}");
    } else {
      connectionStatus->setStyleSheet("QLabel { background-color : green; border-radius: 6px; }");
    }
  } else {
    connectionStatus->setStyleSheet("QLabel { background-color : red; border-radius: 6px; }");
  }
}

void RadioWindow::sendData(const QJsonObject &object)
{
  QJsonDocument doc(object);
  QByteArray data = doc.toJson(QJsonDocument::Compact);
  webSocket->sendBinaryMessage(data);
}
void RadioWindow::cbBandChanged()
{
  //qDebug() << "cbBandChanged";
  QJsonObject object;
  object.insert("action", QJsonValue::fromVariant("changeband"));
  object.insert("value", QJsonValue::fromVariant(cbBand->currentText()));
  sendData(object);
}
void RadioWindow::cbGroupChanged()
{
  //qDebug() << "cbGroupChanged";
  QJsonObject object;
  object.insert("action", QJsonValue::fromVariant("changegroup"));
  object.insert("value", QJsonValue::fromVariant(cbGroup->currentText()));
  sendData(object);
}
void RadioWindow::toggleAntennaLock()
{
  //qDebug() << "toggleAntennaLock";
  QJsonObject object;
  object.insert("action", QJsonValue::fromVariant("togglelock"));
  sendData(object);
}
void RadioWindow::toggleAntennaScan()
{
  //qDebug() << "toggleAntennaScan";
  QJsonObject object;
  object.insert("action", QJsonValue::fromVariant("togglescan"));
  sendData(object);
}
void RadioWindow::toggleScanEnabled()
{
  QJsonObject object;
  object.insert("action", QJsonValue::fromVariant("togglescanenabled"));
  sendData(object);
}
void RadioWindow::cbScanDelayChanged()
{
  //qDebug() << "cbScanDelayChanged";
  QJsonObject object;
  object.insert("action", QJsonValue::fromVariant("changescandelay"));
  object.insert("value", QJsonValue::fromVariant(cbScanDelay->currentIndex()));
  sendData(object);
}
void RadioWindow::toggleAntennaTracking()
{
  //qDebug() << "toggleAntennaTracking";
  QJsonObject object;
  object.insert("action", QJsonValue::fromVariant("toggletracking"));
  sendData(object);
}
void RadioWindow::cbLinkedChanged()
{
  //qDebug() << "cbLinkedChanged";
  QJsonObject object;
  object.insert("action", QJsonValue::fromVariant("changelinked"));
  object.insert("value", QJsonValue::fromVariant(cbLinked->currentIndex()));
  sendData(object);
}
void RadioWindow::swapAntennas()
{
  QJsonObject object;
  object.insert("action", QJsonValue::fromVariant("swapantennas"));
  sendData(object);
}

void RadioWindow::loadMaps()
{

  //for (const auto &map : qAsConst(AzMaps)) {
  for (const auto &map : qAsConst(AzMaps)) {
    delete map;
  }
  AzMaps.clear();

  if (!settings->value(s_mapDirectory, s_mapDirectory_def).toString().isEmpty()) {
    QCollator collator;
    collator.setNumericMode(true);
    QDir directory(settings->value(s_mapDirectory, s_mapDirectory_def).toString());
    QStringList filter;
    filter << QLatin1String("*.png"); // only png
    directory.setNameFilters(filter);
    directory.setSorting(QDir::NoSort);
    QStringList filelist = directory.entryList();
    if (filelist.count()) {

      std::sort(filelist.begin(), filelist.end(), collator);
      for (const auto& file : qAsConst(filelist)) {
        QString imagePath = directory.absolutePath() + "/" + file;
        //qDebug() << imagePath;
        //QPixmap *map = new QPixmap(imagePath);
        QPixmap *scaledMap = new QPixmap(QPixmap(imagePath).scaled(kMapWidth,
                                                    kMapHeight,
                                                    Qt::KeepAspectRatio,
                                                    Qt::SmoothTransformation));

        AzMaps << scaledMap;
        //delete map;
      }

    } else { // no PNGs, try converting NS6T PDFs
      filter.clear();
      filter << QLatin1String("*.pdf");
      directory.setNameFilters(filter);
      filelist = directory.entryList();
      if (filelist.count()) {
        std::sort(filelist.begin(), filelist.end(), collator);
        for (const auto& file : qAsConst(filelist)) {
          QString filepath = directory.absolutePath() + "/" + file;
          //qDebug() << filepath;
#ifdef Q_OS_WIN
          Poppler::setDebugErrorFunction(dummy_error_function, QVariant());
          Poppler::Document* document = Poppler::Document::load(filepath);
          if (document && !document->isLocked()) {
            document->setRenderHint(Poppler::Document::Antialiasing);
            document->setRenderHint(Poppler::Document::TextAntialiasing);
            document->setRenderHint(Poppler::Document::TextHinting);
            document->setRenderHint(Poppler::Document::TextSlightHinting);
            //document->setRenderHint(Poppler::Document::ThinLineShape);
            document->setRenderBackend(Poppler::Document::ArthurBackend);
            document->setPaperColor(Qt::transparent);
            //qDebug() << "document loaded";

            QImage image = document->page(0)->renderToImage(100, 100); // 100 dpi
#endif
#ifdef Q_OS_LINUX
          QPdfDocument document;
          document.load(filepath);
          QImage image = document.render(0, QSize(850,1100));
#endif
            if (!image.isNull()) {
              //qDebug() << "QImage created h:" << image.size().height() << " w:" << image.size().width();
              image = image.copy(QRect(34,159,782,782));
              image = image.scaled(kMapWidth,
                                   kMapHeight,
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation);

              image.save(filepath.replace(".pdf", ".png", Qt::CaseInsensitive)); //, "PNG", 0);

              //QImage circle(780, 780, QImage::Format_ARGB32);
              //circle.fill(Qt::transparent);
              //QPainter pc(&circle);
              //pc.setRenderHints( QPainter::Antialiasing );
              //pc.setBrush( Qt::white );
              //pc.setPen( Qt::NoPen);
              //pc.drawEllipse(0,0,780,780);
              //pc.end();
              //QPainter pi(&image);
              //pi.setCompositionMode( QPainter::CompositionMode_DestinationOver );
              //pi.setRenderHints( QPainter::Antialiasing );
              //pi.drawImage( QRect( 0, 0, 780, 780 ), circle );
              //pi.end();

              //QPixmap map = QPixmap::fromImage(circle);
              /*
              QPixmap map = QPixmap::fromImage(image);
              QPixmap *scaledMap = new QPixmap(map.scaled(kMapWidth,
                                                  kMapHeight,
                                                  Qt::KeepAspectRatio,
                                                  Qt::SmoothTransformation));

              AzMaps << scaledMap;
              */
            }
#ifdef Q_OS_WIN
          }
          delete document;
#endif
        }
        filter.clear();
        filter << QLatin1String("*.png"); // re-check for PNGs and load
        directory.setNameFilters(filter);
        filelist = directory.entryList();
        if (filelist.count()) {

          std::sort(filelist.begin(), filelist.end(), collator);
          for (const auto& file : qAsConst(filelist)) {
            QString imagePath = directory.absolutePath() + "/" + file;
            QPixmap *scaledMap = new QPixmap(QPixmap(imagePath)
                                              .scaled(kMapWidth,
                                                      kMapHeight,
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation));
            AzMaps << scaledMap;
          }
        }
      }
    }
    //qDebug() << "Images loaded: " << AzMaps.size();
    //if (AzMaps.size()) {
    //  qDebug() << "Width: " << AzMaps[0].width();
    //  qDebug() << "Height: " << AzMaps[0].height();
    //}
  }
}

void RadioWindow::setComboBoxItemEnabled(QComboBox * comboBox,
                                         int index,
                                         bool enabled)
{
    auto * model = qobject_cast<QStandardItemModel*>(comboBox->model());
    assert(model);
    if(!model) return;

    auto * item = model->item(index);
    assert(item);
    if(!item) return;
    item->setEnabled(enabled);
}


// graphics frame

void RadioWindow::createGraphicsView()
{

  GraphicsView = new CustomGraphicsView();

  connect(GraphicsView,
          &CustomGraphicsView::mousewheel,
          this,
          [=](int step){ zoomChangedMouse(step); });
  connect(GraphicsView,
          &CustomGraphicsView::mousePressLeft,
          this,
          [=](int x, int y){ bearingChangedMouse(x, y); });

  GraphicsView->setStyleSheet("background: transparent;");
  GraphicsView->setWindowFlags(Qt::FramelessWindowHint);
  GraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  GraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  GraphicsView->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
  //GraphicsView->setCacheMode(QGraphicsView::CacheBackground);
  //GraphicsView->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
  //GraphicsView->setGeometry(QRect(0, 0, 400, 400));
  GraphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  layout->addWidget(GraphicsView);

  GraphicsScene = new QGraphicsScene(QRectF(0,0,kGraphicsSceneWidth,kGraphicsSceneHeight), GraphicsView);
  GraphicsView->setScene(GraphicsScene);

}

void RadioWindow::createGraphicsLegend()
{
  QImage image(kMapWidth, kMapHeight, QImage::Format_ARGB32);
  image.fill(Qt::transparent);
  QPainter p(&image);
  p.setRenderHints( QPainter::Antialiasing );
  p.setBrush( Qt::black );
  p.setPen( Qt::black);
  p.drawEllipse(100,100,kMapWidth-200,kMapHeight-200);
  p.end();
  QPixmap *legend = new QPixmap();
  legend->fromImage(image);
  *legend = legend->copy(0,0,kMapWidth,kMapHeight);
  GraphicsLegend = GraphicsScene->addPixmap(*legend);
  GraphicsLegend->setZValue(500);
  GraphicsLegend->setPos((kGraphicsSceneWidth-kMapWidth)/2,
                           (kGraphicsSceneHeight-kMapHeight)/2 );

}

void RadioWindow::updateGraphicsPixmap()
{
  if (GraphicsPixmap != nullptr) {
    GraphicsScene->removeItem(GraphicsPixmap);
    delete GraphicsPixmap;
    GraphicsPixmap = nullptr;
  }
  if (AzMaps.size()) {
    if (mapZoom >= AzMaps.size()) {
      mapZoom = AzMaps.size()-1;
    }
    GraphicsPixmap = GraphicsScene->addPixmap(*AzMaps[mapZoom]);
    GraphicsPixmap->setZValue(0);
    GraphicsPixmap->setPos((kGraphicsSceneWidth-kMapWidth)/2,
                           (kGraphicsSceneHeight-kMapHeight)/2 );
  }
}


void RadioWindow::updateGraphicsEllipse(int start, int stop, int state)
{

  if (state == kUnavailable &&
      !settings->value(s_mapShadeUnavailable, s_mapShadeUnavailable_def).toBool() ) {
    return;
  }

  // note: Qt painter angles increase counterclockwise
  if (stop < start) {
    start -= 360;
  }
  int span = stop - start;

  stop = (-stop + 90)%360; // transform to Qt angles

  QRectF rect(0, 0,
              kMapWidth-kGraphicsEllipseShrink, kMapHeight-kGraphicsEllipseShrink);
  QColor brushColor;
  if (state == kSelected) {
    brushColor = settings->value(s_mapShadeColor, s_mapShadeColor_def).value<QColor>();
  } else {
    brushColor = QColor(0, 0, 0, 40);
  }
  QGraphicsEllipseItem *ellipseitem = GraphicsScene->addEllipse(rect, Qt::NoPen, QBrush(brushColor));
  GraphicsEllipses << ellipseitem;
  ellipseitem->setStartAngle(stop*16);
  ellipseitem->setSpanAngle(span*16);
  ellipseitem->setZValue(5);
  ellipseitem->setPos( ((kGraphicsSceneWidth-kMapWidth)/2)+(kGraphicsEllipseShrink/2),
                                            ((kGraphicsSceneHeight-kMapHeight)/2)+(kGraphicsEllipseShrink/2) );

}


void RadioWindow::addGraphicsLine(int angle)
{
  if (GraphicsScene != nullptr) {

    QLineF line(kGraphicsSceneWidth/2,
                kGraphicsSceneHeight/2,
                kGraphicsSceneWidth/2,
                (kGraphicsSceneHeight/2) - kGraphicsLineLength);

    angle = (-angle + 90) % 360; // transform to Qt angles
    line.setAngle(angle);
    QPen pen(QColor(50,50,50,80), 1, Qt::SolidLine);
    QGraphicsLineItem *lineitem = GraphicsScene->addLine(line, pen);
    GraphicsLines << lineitem;
    lineitem->setZValue(10);
  }
}


void RadioWindow::addGraphicsLabel(int angle, QString label, int state)
{
  if (GraphicsScene != nullptr) {
    angle = (angle - 90) % 360; // transform to Qt angles

    // translate angle to position, given compass radius of ~175
    double x = ((kMapWidth/2)+kGraphicsLabelMargin) * qCos(qDegreesToRadians(double(angle))) + (kGraphicsSceneWidth/2);
    double y = ((kMapHeight/2)+kGraphicsLabelMargin) * qSin(qDegreesToRadians(double(angle))) + (kGraphicsSceneHeight/2);

    label.replace(" ", "\r");
    QFont font(QGuiApplication::font().family(),
               kGraphicsFontSize,
               QFont::DemiBold,
               QFont::StyleNormal);
    QGraphicsTextItem *text = GraphicsScene->addText(label, font);
    GraphicsText << text;
    switch (state) {
      case kUnavailable:
        if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
          text->setDefaultTextColor(QColor(100, 100, 100, 255));
        } else {
          text->setDefaultTextColor(QColor(170, 170, 170, 255));
        }
        break;
      case kSelected:
        if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
          text->setDefaultTextColor(QColor(255,102,102,255));
        } else {
          text->setDefaultTextColor(QColor(200, 0, 0, 255));
        }
        break;
      case kAvailable:
      default:
        if (settings->value(s_darkPalette, s_darkPalette_def).toBool()) {
          text->setDefaultTextColor(QColor(Qt::white));
        } else {
          text->setDefaultTextColor(QColor(0, 50, 255, 255));
        }
        break;
    }
    text->setZValue(100);
    QRectF bR = text->boundingRect();
    text->setPos(x - bR.width()/2, y - bR.height()/2);
  }
}

void RadioWindow::createButtons()
{

  int i = 0; // column
  int j = 0; // row
  for (auto &Button : antennaButtons) {
    Button.button = new QPushButton(Button.label);
    Button.button->setFixedSize(QSize(100, 40));
    connect(Button.button,
            &QPushButton::released,
            this,
            [=](){ antennaButtonClicked(Button.antenna); });

    gridLayout->addWidget(Button.button, j, i);
    if (j==4) {
      //if (j==(int)ceil(sqrt(numButtons))) {
      j=0;
      i++;
    } else {
      j++;
    }
  }
}

void RadioWindow::updateAntennaButton(int antenna, int state)
{
  for (const auto &Button : qAsConst(antennaButtons)) {
    if (Button.antenna == antenna) {
      if (state == kSelected) {
        Button.button->setStyleSheet("background-color: #00ff00;color:black;");
        Button.button->setEnabled(true);
      } else if (state == kUnavailable) {
        Button.button->setStyleSheet("");
        Button.button->setEnabled(false);
      } else {
        Button.button->setStyleSheet("");
        Button.button->setEnabled(true);
      }
    }
  }
}

void RadioWindow::antennaButtonClicked(int antenna)
{
  QJsonObject object;
  object.insert("action", QJsonValue::fromVariant("buttonclicked"));
  object.insert("antenna", QJsonValue::fromVariant(antenna));
  sendData(object);
}



void RadioWindow::zoomIn()
{
  zoomChangedMouse(1);
}

void RadioWindow::zoomOut()
{
  zoomChangedMouse(-1);
}

void RadioWindow::zoomChangedMouse(int step)
{
  //mapZoom += step; // up zooms out
  mapZoom -= step; // down zooms out
  if (mapZoom < 0) mapZoom = 0;
  if (mapZoom > AzMaps.size()-1) mapZoom = AzMaps.size()-1;
  //qDebug() << "zoomChangedMouse " << mapZoom;
  updateGraphicsPixmap();
}

void RadioWindow::bearingChangedMouse(int x, int y)
{
  auto xx = double(x) - (kGraphicsSceneWidth/2);
  auto yy = double(y) - (kGraphicsSceneHeight/2);
  auto radians = std::atan2(xx, -yy);
  auto degrees = radians * 180.0 / M_PI;
  if (degrees < 0.0 ) degrees += 360.0;
  int bearing = int(degrees);
  if (currentBearing != bearing) {
    currentBearing = bearing;
    QJsonObject object;
    object.insert("action", QJsonValue::fromVariant("bearing"));
    object.insert("degrees", QJsonValue::fromVariant(bearing));
    sendData(object);
  }
}

// validators
bool RadioWindow::isValidAngle(int value) {

  if (value >= 0 && value < 360) {
    return true;
  } else {
    return false;
  }
}



// locator2longlat and qrb adapted from hamlib src/locator.c

/**
 * \brief Convert Maidenhead grid locator to Longitude/Latitude
 * \param longitude	Pointer for the calculated Longitude
 * \param latitude	Pointer for the calculated Latitude
 * \param locator	The Maidenhead grid locator--2 through 12 char + nul string
 *
 *  Convert Maidenhead grid locator to Longitude/Latitude (decimal degrees).
 *  The locator should be in 2 through 12 chars long format.
 *  \a locator2longlat is case insensitive, however it checks for
 *  locator validity.
 *
 *  Decimal long/lat is computed to center of grid square, i.e. given
 *  EM19 will return coordinates equivalent to the southwest corner
 *  of EM19mm.
 *
 * \retval -RIG_EINVAL if locator exceeds RR99xx99xx99 or exceeds length
 *  limit--currently 1 to 6 lon/lat pairs.
 * \retval RIG_OK if conversion went OK.
 *
 * \bug The fifth pair ranges from aa to xx, there is another convention
 *  that ranges from aa to yy.  At some point both conventions should be
 *  supported.
 *
 * \sa longlat2locator()
 */
bool RadioWindow::locator2longlat(double *longitude, double *latitude, const char *locator) {
	int x_or_y, paircount;
	int locvalue, pair;
	int divisions;
	double xy[2], ordinate;

	/* bail if NULL pointers passed */
	if (!longitude || !latitude)
		return false;

	paircount = strlen(locator) / 2;

	/* verify paircount is within limits */
	if (paircount > MAX_LOCATOR_PAIRS)
		paircount = MAX_LOCATOR_PAIRS;
	else if (paircount < MIN_LOCATOR_PAIRS)
		return false;

	/* For x(=longitude) and y(=latitude) */
	for (x_or_y = 0;  x_or_y < 2;  ++x_or_y) {
		ordinate = -90.0;
		divisions = 1;

		for (pair = 0;  pair < paircount;  ++pair) {
			locvalue = locator[pair*2 + x_or_y];

			/* Value of digit or letter */
			locvalue -= (loc_char_range[pair] == 10) ? '0' :
				(isupper(locvalue)) ? 'A' : 'a';

			/* Check range for non-letter/digit or out of range */
			if ((locvalue < 0) || (locvalue >= loc_char_range[pair]))
				return false;

			divisions *= loc_char_range[pair];
			ordinate += locvalue * 180.0 / divisions;
		}
		/* Center ordinate in the Maidenhead "square" or "subsquare" */
		ordinate += 90.0 / divisions;

		xy[x_or_y] = ordinate;
	}

	*longitude = xy[0] * 2.0;
	*latitude = xy[1];

	return true;
}

/**
 * \brief Calculate the distance and bearing between two points.
 * \param lon1		The local Longitude, decimal degrees
 * \param lat1		The local Latitude, decimal degrees
 * \param lon2		The remote Longitude, decimal degrees
 * \param lat2		The remote Latitude, decimal degrees
 * \param distance	Pointer for the distance, km
 * \param azimuth	Pointer for the bearing, decimal degrees
 *
 *  Calculate the QRB between \a lon1, \a lat1 and \a lon2, \a lat2.
 *
 *	This version will calculate the QRB to a precision sufficient
 *	for 12 character locators.  Antipodal points, which are easily
 *	calculated, are considered equidistant and the bearing is
 *	simply resolved to be true north (0.0°).
 *
 * \retval -RIG_EINVAL if NULL pointer passed or lat and lon values
 * exceed -90 to 90 or -180 to 180.
 * \retval RIG_OK if calculations are successful.
 *
 * \return The distance in kilometers and azimuth in decimal degrees
 *  for the short path are stored in \a distance and \a azimuth.
 *
 * \sa distance_long_path(), azimuth_long_path()
 */

bool RadioWindow::qrb(double lon1, double lat1, double lon2, double lat2, double *distance, double *azimuth) {
	double delta_long, tmp, arc, az;

	/* bail if NULL pointers passed */
	if (!distance || !azimuth)
		return false;

	if ((lat1 > 90.0 || lat1 < -90.0) || (lat2 > 90.0 || lat2 < -90.0))
		return false;

	if ((lon1 > 180.0 || lon1 < -180.0) || (lon2 > 180.0 || lon2 < -180.0))
		return false;

	/* Prevent ACOS() Domain Error */
	if (lat1 == 90.0)
		lat1 = 89.999999999;
	else if (lat1 == -90.0)
		lat1 = -89.999999999;

	if (lat2 == 90.0)
		lat2 = 89.999999999;
	else if (lat2 == -90.0)
		lat2 = -89.999999999;

	/* Convert variables to Radians */
	lat1	/= RADIAN;
	lon1	/= RADIAN;
	lat2	/= RADIAN;
	lon2	/= RADIAN;

	delta_long = lon2 - lon1;

	tmp = sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(delta_long);

	if (tmp > .999999999999999) {
		/* Station points coincide, use an Omni! */
		*distance = 0.0;
		*azimuth = 0.0;
		return true;
	}

	if (tmp < -.999999) {
		/*
		 * points are antipodal, it's straight down.
		 * Station is equal distance in all Azimuths.
		 * So take 180 Degrees of arc times 60 nm,
		 * and you get 10800 nm, or whatever units...
		 */
		*distance = 180.0 * ARC_IN_KM;
		*azimuth = 0.0;
		return true;
	}

	arc = acos(tmp);

	/*
	 * One degree of arc is 60 Nautical miles
	 * at the surface of the earth, 111.2 km, or 69.1 sm
	 * This method is easier than the one in the handbook
	 */


	*distance = ARC_IN_KM * RADIAN * arc;

	/* Short Path */
	/* Change to azimuth computation by Dave Freese, W1HKJ */

	az = RADIAN * atan2(sin(lon2 - lon1) * cos(lat2),
			    (cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2 - lon1)));

	az = fmod(360.0 + az, 360.0);
	if (az < 0.0)
		az += 360.0;
	else if (az >= 360.0)
		az -= 360.0;

	//*azimuth = floor(az + 0.5);
  *azimuth = az;
	return true;
}


// projection conversion functions
// all angles and lat/long in radians
bool RadioWindow::distAz2LongLat(const double distance, const double azimuth,
                                const double long1, const double lat1,
                                double &long2, double &lat2)
{
  if (distance > ARC_IN_KM * 180 || distance < 0) return false;
  if (azimuth < 0 || azimuth >= TWO_PI) return false;
  if (long1 < -M_PI || long1 > M_PI) return false;
  if (lat1 < -M_PI_2  || lat1 > M_PI_2 ) return false;

  auto angularDistance = distance / EARTH_RADIUS;
  lat2 = asin( sin(lat1)*cos(angularDistance)
                    + cos(lat1)*sin(angularDistance)*cos(azimuth) );

  auto long2tmp = long1 + atan2(sin(azimuth)*sin(angularDistance)*cos(lat1),
                             cos(angularDistance)-sin(lat1)*sin(lat2) );
  long2tmp = fmod((long2tmp + 3.0 * M_PI), TWO_PI) - M_PI; // normalize to range -M_PI to +M_PI
  long2 = long2tmp;

  return true;
}

// translate equirectangular (Plate Carree) projection pixels to lat,long
bool RadioWindow::pixelRect2longlat(const int x, const int y,
                                   const int sizeX, const int sizeY,
                                   double &longitude, double &latitude)
{
  if (x < 0 || x > sizeX) return false;
  if (y < 0 || y > sizeY) return false;

  longitude = (x - sizeX / 2.0) * (TWO_PI / sizeX);
  latitude = (y - sizeY / 2.0) * (-M_PI / sizeY);
  return true;
}

bool RadioWindow::longlat2PixelRect(const double longitude, const double latitude,
                                   const int sizeX, const int sizeY,
                                   int &x, int &y)
{
  if (longitude < -M_PI || longitude > M_PI) return false;
  if (latitude < -M_PI_2 || latitude > M_PI_2) return false;

  x = (longitude * sizeX / TWO_PI) + (sizeX / 2.0);
  y = (sizeY / 2.0) - (latitude * sizeY / M_PI);
  return true;
}

bool RadioWindow::pixelAz2DistAz(const int x, const int y,
                                 const int sizeXY, const int kmRadius,
                                 double &distance, double &azimuth)
{
  if (x < 0 || x >= sizeXY) return false;
  if (y < 0 || y >= sizeXY) return false;

  auto xx = double(x) - (sizeXY/2.0); /// shift axis
  auto yy = double(y) - (sizeXY/2.0); // shift axis
  auto angle = std::atan2(xx, -yy);
  if (angle < 0.0 ) angle += TWO_PI;
  azimuth = angle;
  distance = sqrt(xx*xx + yy*yy) * (kmRadius * 2.0 / sizeXY);
  return true;
}
