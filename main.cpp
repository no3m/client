#include "radiowindow.h"
#include <QApplication>
#include <QStringList>
#include <QDebug>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QStringList args=app.arguments();
  app.setApplicationName ("softrx-client");
  // explicitely set Fusion style
  // on Pi platform,  Qt5CTProxyStyle is default and messes up QPushButton
  app.setStyle(QStyleFactory::create("Fusion"));

  RadioWindow w(args);
  w.show();
  return app.exec();
}
