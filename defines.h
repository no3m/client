#ifndef DEFINES_H
#define DEFINES_H
#pragma once

/*!
    Software RX Switching E. Tichansky NO3M 2021
    v0.1
 */

#include <QApplication>
#include <QBrush>
#include <QByteArray>
#include <QCloseEvent>
#include <QColor>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDialog>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFlags>
#include <QFont>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QMainWindow>
#include <QMetaType>
#include <QObject>
#include <QPalette>
#include <QPen>
#include <QPixmap>
#include <QPoint>
#include <QSettings>
#include <QSpinBox>
#include <QStackedLayout>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QStyle>
#include <QtGlobal>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QtMath>
#include <QtWebSockets>
#include <QtWidgets>

#include <cstdio>
#include <cmath>
#include <cstring>

//#include <time.h>
//#ifdef Q_OS_WIN
//#define timegm _mkgmtime
//#endif

const int timerPeriod = 5*60*1000; // 5 minutes

const double TWO_PI = 2.0 * M_PI;
// locator2longlat
const static int loc_char_range[] = { 18, 10, 24, 10, 24, 10 };
#define MAX_LOCATOR_PAIRS       6
#define MIN_LOCATOR_PAIRS       1
// qrb
#define RADIAN  (180.0 / M_PI)
/* arc length for 1 degree, 60 Nautical Miles */
#define ARC_IN_KM 111.2
#define EARTH_RADIUS 6371.009
#define MAX_KM (ARC_IN_KM * 180.0)

const int kDispList=0;
const int kDispCompass=1;
const int kDispNone=2;

const int kAvailable=0;
const int kSelected=1;
const int kUnavailable=2;

const QString s_serverIp                = "serverip";
const QString s_serverIp_def            = "127.0.0.1";
const QString s_serverPort              = "serverport";
const QString s_serverPort_def          = "7300";
const QString s_serverAutoConnect       = "serverauto";
const bool    s_serverAutoConnect_def   = false;
const QString s_mapFile                 = "mapdirectory";
const QString s_mapFile_def             = "";
const QString s_mapZoom                 = "mapZoom";
const int     s_mapZoom_def             = 0;
const QString s_mapShadeUnavailable     = "mapshadeunavailable";
const bool    s_mapShadeUnavailable_def = false;
const QString s_mapShadeColor           = "mapshadecolor";
const QColor  s_mapShadeColor_def       = QColor(255, 102, 102, 100); // trans red
const QString s_darkPalette             = "darkpalette";
const bool    s_darkPalette_def         = false;
const QString s_gridlocator             = "gridlocator";
const QString s_gridlocator_def         = "";
const QString s_nightshading            = "nightshading";
const bool    s_nightshading_def        = false;
const QString s_nighttransparency       = "nighttransparency";
const int     s_nighttransparency_def   = (90-50)/10; // 90

#endif
