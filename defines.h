/*!
    Software RX Switching E. Tichansky NO3M 2021
    v0.1
 */

#pragma once

#include <QAbstractSocket>
#include <QApplication>
#include <QBrush>
#include <QByteArray>
#include <QChar>
#include <QCloseEvent>
#include <QColor>
#include <QCollator>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QElapsedTimer>
#include <QErrorMessage>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFlags>
#include <QFont>
#include <QFontMetricsF>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QHeaderView>
#include <QHostAddress>
#include <QItemDelegate>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QKeyEvent>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QMutex>
#include <QObject>
#include <QPalette>

#ifdef Q_OS_LINUX
 //#include <QtPdf>
 #include <QPdfDocument>
 //#include <QPdfDocumentRenderOptions>
#endif
#ifdef Q_OS_WIN
 #include <poppler-qt5.h>
 static void dummy_error_function(const QString &, const QVariant &) { } // poppler
#endif

#include <QPen>
#include <QPixmap>
#include <QProgressDialog>
#include <QQueue>
#include <QReadWriteLock>
#include <QScreen>
#include <QSize>
//#include <QSqlDatabase>
//#include <QSqlError>
//#include <QSqlQuery>
//#include <QSqlQueryModel>
//#include <QSqlTableModel>
//#include <QSqlRecord>
#include <QSettings>
//#include <QSerialPortInfo>
//#include <QSerialPort>
#include <QSpinBox>
#include <QStackedLayout>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QStyle>
//#include <QStyledItemDelegate>
//#include <QTableView>
#include <QTcpSocket>
#include <QtGlobal>
//#include <QThread>
#include <QTime>
#include <QTimer>
#include <QtMath>
//#include <QtSql>
#include <QtWebSockets>
#include <QtWidgets>
#include <QUrl>
#include <QVariant>

#include <QSpinBox>
//#include <QStyledItemDelegate>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QSize>

//#include <stdio.h>
#include <cstdio>
#include <cmath>

const double TWO_PI = 2 * M_PI;

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
const QString s_mapDirectory            = "mapdirectory";
const QString s_mapDirectory_def        = "";
const QString s_mapZoom                 = "mapZoom";
const int     s_mapZoom_def             = 0;
const QString s_mapShadeUnavailable     = "mapshadeunavailable";
const bool    s_mapShadeUnavailable_def = false;
const QString s_mapShadeColor           = "mapshadecolor";
const QColor  s_mapShadeColor_def       = QColor(255, 102, 102, 100); // trans red
const QString s_darkPalette             = "darkpalette";
const bool    s_darkPalette_def         = false;
