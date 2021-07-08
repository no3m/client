#ifndef MAPCLASS_H
#define MAPCLASS_H
#pragma once

#include "defines.h"
#include "vector3.hpp"
#include "helpers.hpp"

class MapClass : public QObject
{
    Q_OBJECT

public:
  MapClass(QSettings &s, int frameWidth, int frameHeight);
  ~MapClass() override;
  bool isRenderingMaps();

signals:
  void nightMapsReady(QList<QPixmap>);
  void mapsReady(QList<QPixmap>);

public slots:
  void generateNightMaps();
  void generateMaps();

private slots:

private:
  QSettings& settings;
  int mapWidth;
  int mapHeight;
  bool renderingMaps;
};

#endif
