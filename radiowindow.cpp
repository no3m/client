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
  NightGraphicsPixmap = nullptr;
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
  for (int i=50; i<=150; i+=10) {
    settingsdialog->cbNightTransparency->addItem(QString::number(i));
  }
  settingsdialog->cbNightTransparency->setCurrentIndex(settings->
                value(s_nighttransparency,s_nighttransparency_def).toInt());
  settingsdialog->cbNightTransparency->setMaxVisibleItems(8);
  settingsdialog->hide();

  restoreGeometry(settings->value("geometry").toByteArray());
  mapZoom = settings->value(s_mapZoom, s_mapZoom_def).toInt();

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

  mapThread = new QThread;
  mapHandler = new MapClass(*settings, kMapWidth, kMapHeight);
  qRegisterMetaType<QList<QPixmap> >("QList<QPixmap>");
  connect(mapHandler, &MapClass::nightMapsReady,
    this, &RadioWindow::loadNightMaps);
  connect(mapHandler, &MapClass::mapsReady,
    this, &RadioWindow::loadMaps);
  mapHandler->moveToThread(mapThread);
  mapThread->start();
  connect(this, &RadioWindow::generateNightMaps,
    mapHandler, &MapClass::generateNightMaps);
  connect(this, &RadioWindow::generateMaps,
    mapHandler, &MapClass::generateMaps);

  emit generateMaps();

  GraphicsLines.clear();
  GraphicsText.clear();
  GraphicsEllipses.clear();

  // kDispNone
  blankWidget = new QWidget;
  layout->addWidget(blankWidget);
  layout->setCurrentIndex(kDispNone);

  m_timer = new QTimer();
  openWebSocket();

  timer = new QTimer();
  connect(timer, &QTimer::timeout, this, &RadioWindow::timerTimeout);
  timer->start(timerPeriod);

  // test
  //
}

RadioWindow::~RadioWindow()
{

}

void RadioWindow::timerTimeout()
{
  //qDebug() << "timer timeout";
  //qDebug() << "main thread" << QThread::currentThreadId();
  //if (settings->value(s_nightshading, s_nightshading_def).toBool()) {
    emit generateNightMaps();
  //}
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
  if (mapThread->isRunning()) {
    mapThread->quit();
    mapThread->wait();
  }
  //for (const auto &map : qAsConst(AzMaps)) {
  //  delete map;
  //}
  //for (const auto &map : qAsConst(nightAzMaps)) {
  //  delete map;
  //}
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

  mapHandler->deleteLater();
  mapThread->deleteLater();

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

void RadioWindow::updateMapImages()
{
  emit generateMaps();
}

void RadioWindow::updateNightMapImages()
{
  emit generateNightMaps();
}

void RadioWindow::toggleNightMapImage()
{
  updateNightGraphicsPixmap();
}

void RadioWindow::connectSignals()
{
  connect(settingsdialog, &SettingsDialog::settingsUpdated,
          this, &RadioWindow::settingsUpdated);
  connect(settingsdialog, &SettingsDialog::updateMapImages,
          this, &RadioWindow::updateMapImages);
  connect(settingsdialog, &SettingsDialog::toggleNightMapImage,
          this, &RadioWindow::toggleNightMapImage);
  connect(settingsdialog, &SettingsDialog::updateNightMapImages,
          this, &RadioWindow::updateNightMapImages);
  connect(settingsdialog, &SettingsDialog::webSocketReconnect,
          this, &RadioWindow::webSocketReconnect);

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
  GraphicsView->setRenderHints( QPainter::Antialiasing
                               |QPainter::TextAntialiasing
                               |QPainter::SmoothPixmapTransform);
  //GraphicsView->setCacheMode(QGraphicsView::CacheBackground);
  //GraphicsView->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
  //GraphicsView->setGeometry(QRect(0, 0, 400, 400));
  GraphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  layout->addWidget(GraphicsView);

  GraphicsScene = new QGraphicsScene(QRectF(0,
                                            0,
                                            kGraphicsSceneWidth,
                                            kGraphicsSceneHeight),
                                     GraphicsView);

  GraphicsView->setScene(GraphicsScene);
}

void RadioWindow::createGraphicsLegend()
{
  // render oversized and scale down
  float expand = 20.0;
  float scale = 8.0;
  QImage image((kMapWidth+expand)*scale, (kMapHeight+expand)*scale, QImage::Format_ARGB32);
  image.fill(Qt::transparent);
  QPainter p(&image);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);
  p.setFont(QFont(QGuiApplication::font().family(),
                  //(kGraphicsFontSize/3)*scale,
                  kGraphicsFontSize*scale,
                  QFont::Normal,
                  QFont::StyleNormal));
  p.setBrush( Qt::NoBrush );
  p.setPen(QColor(0,0,0,255)); //30,30,30,40
  p.drawEllipse(image.width()/2.0 - (kMapWidth/8.0)*scale,
                image.height()/2.0 - (kMapHeight/8.0)*scale,
                (kMapWidth/4.0)*scale,
                (kMapHeight/4.0)*scale);
  p.drawEllipse(image.width()/2.0 - (kMapWidth/4.0)*scale,
                image.height()/2.0 - (kMapHeight/4.0)*scale,
                (kMapWidth/2.0)*scale,
                (kMapHeight/2.0)*scale);
  p.drawEllipse(image.width()/2.0 - (kMapWidth*3/8)*scale,
                image.height()/2.0 - (kMapHeight*3/8)*scale,
                (kMapWidth*3/4)*scale,
                (kMapHeight*3/4)*scale);
  p.drawEllipse(image.width()/2.0 - (kMapWidth/2.0)*scale,
                image.height()/2.0 - (kMapHeight/2.0)*scale,
                (kMapWidth)*scale,
                (kMapHeight)*scale);
  p.drawEllipse(image.width()/2.0 - (kMapWidth/2.0 + 2.0)*scale,
                image.height()/2.0 - (kMapHeight/2.0 + 2.0)*scale,
                (kMapWidth + 4.0)*scale,
                (kMapHeight + 4.0)*scale);

  for (int i = 0; i< 360; ++i) {
    p.setPen(QColor(0,0,0,255)); //30,30,30,40
    if (i%30 == 0) { // 4-seg line
      float x1 = (kMapWidth/20.0 * cos(qDegreesToRadians(double(i))))*scale + image.width()/2.0;
      float y1 = (kMapHeight/20.0 * sin(qDegreesToRadians(double(i))))*scale + image.height()/2.0;
      float x2 = (kMapWidth/2.0 * cos(qDegreesToRadians(double(i))))*scale + image.width()/2.0;
      float y2 = (kMapHeight/2.0 * sin(qDegreesToRadians(double(i))))*scale + image.height()/2.0;
      p.drawLine(x1,y1,x2,y2);

    } else if (i%10 == 0) { // 3-seg line, labels
      p.setPen(QColor(0,0,0,255));
      float x1 = ((kMapWidth/8.0) * cos(qDegreesToRadians(double(i))))*scale + image.width()/2.0;
      float y1 = ((kMapHeight/8.0) * sin(qDegreesToRadians(double(i))))*scale + image.height()/2.0;
      float x2 = (kMapWidth/2.0 * cos(qDegreesToRadians(double(i))))*scale + image.width()/2.0;
      float y2 = (kMapHeight/2.0 * sin(qDegreesToRadians(double(i))))*scale + image.height()/2.0;
      p.drawLine(x1,y1,x2,y2);

    } else if (i%5 == 0) { // 2-seg line
      p.setPen(QColor(0,0,0,255));
      float x1 = ((kMapWidth/4.0) * cos(qDegreesToRadians(double(i))))*scale + image.width()/2.0;
      float y1 = ((kMapHeight/4.0) * sin(qDegreesToRadians(double(i))))*scale + image.height()/2.0;
      float x2 = (kMapWidth/2.0 * cos(qDegreesToRadians(double(i))))*scale + image.width()/2.0;
      float y2 = (kMapHeight/2.0 * sin(qDegreesToRadians(double(i))))*scale + image.height()/2.0;
      p.drawLine(x1,y1,x2,y2);

    }
    // ticks
    p.setPen(QColor(0,0,0,255));
    float x1 = ((kMapWidth/2.0) * cos(qDegreesToRadians(double(i))))*scale + image.width()/2.0;
    float y1 = ((kMapHeight/2.0) * sin(qDegreesToRadians(double(i))))*scale + image.height()/2.0;
    float x2 = ((kMapWidth/2.0+2.0) * cos(qDegreesToRadians(double(i))))*scale + image.width()/2.0;
    float y2 = ((kMapHeight/2.0+2.0) * sin(qDegreesToRadians(double(i))))*scale + image.height()/2.0;
    p.drawLine(x1,y1,x2,y2);
  }

  p.end();
  image.save("/home/eric/.config/softrx-client/grid.png");
  image = image.scaled(image.width()/scale,
                       image.height()/scale,
                       Qt::KeepAspectRatio,
                       Qt::SmoothTransformation);

  QPixmap *legend = new QPixmap(QPixmap::fromImage(image));
  GraphicsLegend = GraphicsScene->addPixmap(*legend);
  GraphicsLegend->setZValue(15);
  GraphicsLegend->setPos((kGraphicsSceneWidth-kMapWidth)/2 -10,
                           (kGraphicsSceneHeight-kMapHeight)/2 -10);

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
    GraphicsPixmap = GraphicsScene->addPixmap(AzMaps[mapZoom]);
    GraphicsPixmap->setZValue(0);
    GraphicsPixmap->setPos((kGraphicsSceneWidth-kMapWidth)/2,
                           (kGraphicsSceneHeight-kMapHeight)/2 );

  }
}

void RadioWindow::updateNightGraphicsPixmap()
{
  if (NightGraphicsPixmap != nullptr) {
    GraphicsScene->removeItem(NightGraphicsPixmap);
    delete NightGraphicsPixmap;
    NightGraphicsPixmap = nullptr;
  }
  if (settings->value(s_nightshading, s_nightshading_def).toBool() ) {
    if (AzMaps.size() > mapZoom) {
      if (nightAzMaps.size() > mapZoom) {
        NightGraphicsPixmap = GraphicsScene->addPixmap(nightAzMaps[mapZoom]);
        NightGraphicsPixmap->setZValue(5);
        NightGraphicsPixmap->setPos((kGraphicsSceneWidth-kMapWidth)/2,
                               (kGraphicsSceneHeight-kMapHeight)/2 );
      }
    }
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
  QGraphicsEllipseItem *ellipseitem = GraphicsScene->addEllipse(rect,
                                                                Qt::NoPen,
                                                                QBrush(brushColor));
  GraphicsEllipses << ellipseitem;
  ellipseitem->setStartAngle(stop*16);
  ellipseitem->setSpanAngle(span*16);
  ellipseitem->setZValue(10);
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
    lineitem->setZValue(20);
  }
}


void RadioWindow::addGraphicsLabel(int angle, QString label, int state)
{
  if (GraphicsScene != nullptr) {

    int margin;
    if (angle > 335 || angle < 25 || (angle > 155 && angle < 205)) {
      margin = kMapWidth / 15; // smaller margin on top and bottom
    } else {
      margin = kGraphicsLabelMargin;
    }

    angle = (angle - 90) % 360; // transform to Qt angles

    // translate angle to position, given compass radius of ~175
    double x = ((kMapWidth/2)+margin) * qCos(qDegreesToRadians(double(angle)))
                    + (kGraphicsSceneWidth/2);
    double y = ((kMapHeight/2)+margin) * qSin(qDegreesToRadians(double(angle)))
                    + (kGraphicsSceneHeight/2);

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
  updateNightGraphicsPixmap();
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

void RadioWindow::loadMaps(QList<QPixmap> maps)
{
  //qDebug() << "loadMaps(main): Maps ready " << maps.size();
  AzMaps.clear();
  AzMaps = maps;
  updateGraphicsPixmap();

  emit generateNightMaps();
}

void RadioWindow::loadNightMaps(QList<QPixmap> maps)
{
  //qDebug() << "loadNightMaps(main): Night maps ready " << maps.size();
  nightAzMaps.clear();
  nightAzMaps = maps;
  updateNightGraphicsPixmap();
}
