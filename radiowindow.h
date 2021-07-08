#ifndef RADIOWINDOW_H
#define RADIOWINDOW_H
#pragma once

#include "defines.h"
#include "ui_radio.h"
#include "ui_settingsdialog.h"
#include "settingsdialog.h"
#include "mapclass.hpp"

struct AntennaButton {
  QPushButton *button;
  QString label;
  int antenna;
  AntennaButton(QPushButton *b, QString l, int a) : button(b), label(l), antenna(a) {};
};


class CustomGraphicsView : public QGraphicsView
{
    Q_OBJECT

protected:
    void wheelEvent(QWheelEvent *event) override
    {
      QPoint numDegrees = event->angleDelta() / 8;
      //QPointF position = event->position();
      //qDebug() << "position " << position << " deg " << numDegrees;

      if (!numDegrees.isNull()) {
        int numSteps = numDegrees.y() / 15;
        emit mousewheel(numSteps);
      }

      event->accept();
    }
    void mousePressEvent(QMouseEvent * event) override
    {
      if (event->button() == Qt::LeftButton) {
        emit mousePressLeft(event->pos().x(), event->pos().y());
      }

      event->accept();
    }
    void mouseMoveEvent(QMouseEvent *event) override
    {
      if (event->buttons() == Qt::LeftButton) {
        //qDebug() << "mouseMoveEvent x " << event->pos().x() << " y " << event->pos().y();
        emit mousePressLeft(event->pos().x(), event->pos().y());
        event->accept();
      } else {
        event->ignore();
      }
    }
signals:
    void mousewheel(int);
    void mousePressLeft(int, int);
};

class RadioWindow : public QWidget, public Ui::RadioWindow
{
Q_OBJECT

public:
    explicit RadioWindow(QStringList args, QWidget *parent = nullptr);
    ~RadioWindow() override;

signals:
  void closed();
  void generateNightMaps();
  void generateMaps();

protected:
  void closeEvent(QCloseEvent *event) override;

public slots:
private slots:
private:
  QSettings *settings;
  SettingsDialog *settingsdialog;
  QList<QPixmap> AzMaps;
  QList<QPixmap> nightAzMaps;
  int radioNr;
  QString radioNrStr;
  int mapZoom;
  int currentBearing;
  QPalette defaultPalette;
  QTimer *timer;

  void connectionLED(bool);
  void setPalette();

  void connectSignals();
  void openSettingsDialog();
  void settingsUpdated();
  void updateMapImages();
  void updateNightMapImages();
  void toggleNightMapImage();
  void cbBandChanged();
  void cbGroupChanged();
  void toggleAntennaLock();
  void toggleAntennaScan();
  void toggleScanEnabled();
  void cbScanDelayChanged();
  void toggleAntennaTracking();
  void cbLinkedChanged();
  void swapAntennas();

  QWebSocket *webSocket;
  QTimer *m_timer;
  void openWebSocket();
  void webSocketConnected();
  void webSocketReconnect();
  void webSocketClosed();
  void messageReceived(const QByteArray &message);
  void sendData(const QJsonObject &);
  void setComboBoxItemEnabled(QComboBox*, int, bool);

  // frame
  QStackedLayout *layout;
  QList<QGraphicsItem*> GraphicsLines;
  QList<QGraphicsItem*> GraphicsText;
  QWidget *blankWidget;
  QWidget *buttonWidget;
  QGridLayout *gridLayout;
  CustomGraphicsView *GraphicsView;
  QGraphicsScene *GraphicsScene;
  QList<QGraphicsItem*> GraphicsEllipses;
  QGraphicsPixmapItem *GraphicsPixmap;
  QGraphicsPixmapItem *NightGraphicsPixmap;
  QList<AntennaButton> antennaButtons;
  QGraphicsPixmapItem *GraphicsLegend;

  void createGraphicsView();
  void updateGraphicsEllipse(int, int, int);
  void updateGraphicsPixmap();
  void updateNightGraphicsPixmap();
  void addGraphicsLine(int);
  void addGraphicsLabel(int, QString, int);
  void createGraphicsLegend();
  void loadMaps(QList<QPixmap>);
  void loadNightMaps(QList<QPixmap>);
  void timerTimeout();

  void createButtons();
  void antennaButtonClicked(int);
  void updateAntennaButton(int,int);

  void zoomChangedMouse(int);
  void zoomIn();
  void zoomOut();
  void bearingChangedMouse(int,int);
  void cleanup();

  int kMapWidth;
  int kMapHeight;
  int kGraphicsSceneHeight;
  int kGraphicsSceneWidth;
  int kGraphicsEllipseShrink;
  int kGraphicsLineLength;
  int kGraphicsLabelMargin;
  int kGraphicsFontSize;

  QThread *mapThread;
  MapClass *mapHandler;

};

#endif
