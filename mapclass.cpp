#include "mapclass.hpp"

MapClass::MapClass(QSettings &s, int width, int height) : settings(s)
{
  mapWidth  = width;
  mapHeight = height;

  renderingMaps=false;
}

MapClass::~MapClass()
{
}

bool MapClass::isRenderingMaps()
{
  return renderingMaps;
}

void MapClass::generateMaps()
{
  while (renderingMaps); // wait until other rendering process is complete
  renderingMaps=true;

  //QElapsedTimer timer;
  //timer.start();

  double centerLatitude = 0.0;
  double centerLongitude = 0.0;
  locator2longlat(&centerLongitude,
                  &centerLatitude,
                  settings.value(s_gridlocator, s_gridlocator_def).toString().toLatin1().data());

  if (settings.value(s_mapFile, s_mapFile_def).toString().isNull()
      || settings.value(s_mapFile, s_mapFile_def).toString().isEmpty() ) {
    return;
  }

  QImage rectMap(settings.value(s_mapFile, s_mapFile_def).toString());
  QImage azimuthalMap(rectMap.height(),rectMap.height(),QImage::Format_ARGB32);

  double centerLatitudeRadians = centerLatitude * M_PI / 180.0;
  double centerLongitudeRadians = centerLongitude * M_PI / 180.0;

  for (int azimY=0; azimY < azimuthalMap.height(); ++azimY) {
    for (int azimX =0; azimX < azimuthalMap.width(); ++azimX) {
      double distance;
      double bearing;
      if (pixelAz2DistAz(azimX,azimY,
                         azimuthalMap.width(),MAX_KM,
                         distance, bearing)) {

        double longitude;
        double latitude;
        if (distAz2LongLat(distance, bearing,
                           centerLongitudeRadians, centerLatitudeRadians,
                           longitude, latitude)) {

          int rectX;
          int rectY;
          if (longlat2PixelRect(longitude, latitude,
                                rectMap.width(), rectMap.height(),
                                rectX, rectY)) {

            azimuthalMap.setPixelColor(azimX, azimY, rectMap.pixelColor(rectX, rectY));
            //rectMap.pixelColor(rectX, rectY); // test
          }
        }
      }
    }
  }
  //qDebug() << timer.restart() << "ms";
  QList<QPixmap> maps;
  int steps = sqrt(azimuthalMap.width() - mapWidth) / 2;
  for (int i=steps; i>0; --i) {
    QImage image(azimuthalMap.scaled(
                 mapWidth + i * i * 2,
                 mapHeight + i * i * 2,
                 Qt::KeepAspectRatio,
                 Qt::SmoothTransformation
               ));
    image = image.copy(QRect(image.width()/2 - mapWidth/2,
                             image.height()/2 - mapHeight/2,
                             mapWidth,
                             mapHeight) );
    QImage clipped(mapWidth,mapHeight,QImage::Format_ARGB32);
    clipped.fill(Qt::transparent);
    QBrush brush(image); // textured brush
    QPainter p(&clipped);
    p.setBrush(brush);
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    //p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.drawEllipse(0,0,mapWidth,mapHeight);
    p.end();

    maps << QPixmap::fromImage(clipped);
  }
  //qDebug() << "generateMaps " << timer.elapsed() << "ms";
  emit mapsReady(maps);
  renderingMaps=false;
}

void MapClass::generateNightMaps()
{
  //qDebug() << "map thread" << QThread::currentThreadId();

  if (renderingMaps) return;
  renderingMaps=true;

  using namespace vector3;

  //QElapsedTimer timer;
  //timer.start();

  // Qt methods
  QDate date = QDateTime::currentDateTimeUtc().date();
  QTime time = QDateTime::currentDateTimeUtc().time();
  double hoursSinceMidnight = time.msecsSinceStartOfDay() / 1000.0 / 3600.0;
  //int june21 = 172;
  //if (date.isLeapYear(date.currentDate().year())) june21 = 173;
  //int daysInYear = date.daysInYear();
  int dayOfYear = date.dayOfYear();
  //int year = date.currentDate().year();

/*
  // c++ std
  auto now = std::chrono::system_clock::now();
  time_t tnow = std::chrono::system_clock::to_time_t(now);
  tm *date_tm = std::gmtime(&tnow);
  double hoursSinceMidnight = date_tm->tm_hour + date_tm->tm_min / 60.0 + date_tm->tm_sec / 3600.0;
  int year = date_tm->tm_year + 1900;
  int dayOfYear = date_tm->tm_yday + 1;
  int daysInYear = 365;
  int june21 = 172;
  //date_tm->tm_hour = 0;
  //date_tm->tm_min = 0;
  //date_tm->tm_sec = 0;
  //auto midnight = std::chrono::system_clock::from_time_t(timegm(date_tm)); // _mkgmtime() windows
  //auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now-midnight);
  //double hoursSinceMidnight = seconds.count() / 3600;
  if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) { // leap year
    daysInYear = 366;
    june21 = 173;
  }
*/

  //double degPerDay = 360.0*M_PI/180.0/365.24;
  //double radPerDay = TWO_PI/365.24;
  //double tilt = 23.44 * cos(TWO_PI / daysInYear * (dayOfYear - june21)); // summer sol ref
  //double tilt = -23.44 * cos(TWO_PI / daysInYear * ((dayOfYear-1) + 10)); // winter sol ref
  //qDebug() << "Observable tilt " << tilt;
  //hoursSinceMidnight += 6.0;  // a
  //double solarNoon = hoursSinceMidnight - 12.0;
  //double solarNoonScaled = solarNoon / 24.0;
  //hoursSinceMidnight += -3.0 + 24.0; // e
  //while (hoursSinceMidnight > 24) hoursSinceMidnight -= 24;
  //hoursSinceMidnight = fmod(hoursSinceMidnight + 6.0, 24.0);
  //hoursSinceMidnight = fmod(hoursSinceMidnight, 24.0);
  double daySinceMidnight = hoursSinceMidnight / 24.0;
  double dayOfYearF = (dayOfYear-1) + daySinceMidnight;
  double tilt = -23.44 * cos(TWO_PI / 365 * (dayOfYearF + 10.0)); // winter sol ref

/*
  qDebug() << "June 21: " << june21;
  qDebug() << "Days in current year " << daysInYear;
  qDebug() << "Day of the year " << dayOfYear;
  qDebug() << "Observable tilt " << tilt;
  //qDebug() << "Current year " << year;
  qDebug() << "hrs since midnight utc " << hoursSinceMidnight;
  qDebug() << "hrs since midnight scaled " << hoursSinceMidnightScaled;
*/
  Vec3d seasonOffset(0, 0, tan(TWO_PI * (tilt/360.0)));
  Vec3d pointingFromEarthToSun(-cos((TWO_PI) * daySinceMidnight),
                               sin((TWO_PI) * daySinceMidnight),
                               0 );
  //pointingFromEarthToSun = pointingFromEarthToSun + seasonOffset;
  pointingFromEarthToSun += seasonOffset;
  pointingFromEarthToSun.normalize();

/*
  // plate carree projection
  QImage nightRectMap(2048,1024,QImage::Format_ARGB32);
  nightRectMap.fill(Qt::transparent);
  for (int u=0; u < nightRectMap.width(); ++u) {
    for (int v=0; v < nightRectMap.height(); ++v) {
      double phi = ((double(v) / (double(nightRectMap.height()) * 2.0)) - 1.0) * TWO_PI;
      double lambda = (double(u) / double(nightRectMap.width())) * TWO_PI;
      double x = sin(phi) * cos(lambda);
      double y = cos(phi);
      double z = sin(phi) * sin(lambda);
      Vec3d earthNormal(x, y, z);
      earthNormal.normalize();

      double angleBetweenSurfaceAndSunlight = dot(pointingFromEarthToSun, earthNormal);
      //qDebug() << "angle " << angleBetweenSurfaceAndSunlight;
      if (angleBetweenSurfaceAndSunlight <= -0.1) { // 6 deg
        nightRectMap.setPixelColor(u, v, QColor(0,0,0,130)); // night

      } else if (angleBetweenSurfaceAndSunlight < 0.0) {

        nightRectMap.setPixelColor(u, v,
          QColor(0,
                 0,
                 0,
                 90 - int(angleBetweenSurfaceAndSunlight * 400.0))); // twilight
                 //130));

      } else {
        // day
      }
    }
  }
  //nightRectMap.save("/home/eric/.config/softrx-client/nightRectMap.png");
*/

  // equidistant azimuthal projection
  //qDebug() << timer.restart() << "ms";
  if (settings.value(s_mapFile, s_mapFile_def).toString().isNull()
      || settings.value(s_mapFile, s_mapFile_def).toString().isEmpty() ) {
    return;
  }
  //qDebug() << "source map: " << settings.value(s_mapFile, s_mapFile_def).toString();
  double centerLatitude = 0.0;
  double centerLongitude = 0.0;
  locator2longlat(&centerLongitude,
                  &centerLatitude,
                  settings.value(s_gridlocator, s_gridlocator_def).toString().toLatin1().data());
  double centerLatitudeRadians = centerLatitude * M_PI / 180.0;
  double centerLongitudeRadians = centerLongitude * M_PI / 180.0;
  QImage rectMap(settings.value(s_mapFile, s_mapFile_def).toString());
  QImage nightAzMap(rectMap.height(),rectMap.height(),QImage::Format_ARGB32);
  nightAzMap.fill(Qt::transparent);
  int transparency = settings.value(s_nighttransparency,s_nighttransparency_def).toInt();
  transparency = transparency * 10 + 50;

  for (int azimY=0; azimY < nightAzMap.height(); ++azimY) {
    //QRgb *line = (QRgb *)nightAzMap.scanLine(azimY);
    for (int azimX =0; azimX < nightAzMap.width(); ++azimX) {
      double distance;
      double bearing;
      if (pixelAz2DistAz(azimX,azimY,
                         nightAzMap.width(),MAX_KM,
                         distance, bearing)) {

        double longitude;
        double latitude;
        if (distAz2LongLat(distance, bearing,
                           centerLongitudeRadians, centerLatitudeRadians,
                           longitude, latitude
        )) {

          Vec3d earthNormal(
            cos(latitude) * cos(longitude),
            cos(latitude) * sin(longitude),
            sin(latitude)
          );
          //earthNormal2.normalize();
          double angleBetweenSurfaceAndSunlight = dot(pointingFromEarthToSun, earthNormal);
          if (angleBetweenSurfaceAndSunlight < -0.1) { // 6 degrees
            nightAzMap.setPixelColor(azimX,
                                     azimY,
                                     QColor(0,0,0,transparency)); // night
            //line[azimX] = qRgba(qRed(0), qGreen(0), qBlue(0), qAlpha(70));
          } else if (angleBetweenSurfaceAndSunlight < 0.0) {
            int alpha = (transparency - 40) - int(angleBetweenSurfaceAndSunlight * 400.0);
            nightAzMap.setPixelColor(azimX, azimY,QColor(0,0,0,alpha)); // twilight
            //line[azimX] = qRgba(qRed(0), qGreen(0), qBlue(0), qAlpha(alpha));
          }
        }
      }
    }
  }


  //qDebug() << timer.restart() << "ms";
  //nightAzMap.save("/home/eric/.config/softrx-client/nightAzMap.png");
  QList<QPixmap> maps;
  int steps = sqrt(nightAzMap.width() - mapWidth) / 2;
  for (int i=steps; i>0; --i) {
    QImage image(nightAzMap.scaled(
                 mapWidth + i * i * 2,
                 mapHeight + i * i * 2,
                 Qt::KeepAspectRatio,
                 Qt::SmoothTransformation
               ));
    image = image.copy(QRect(image.width()/2 - mapWidth/2,
                             image.height()/2 - mapHeight/2,
                             mapWidth,
                             mapHeight) );
    QImage clipped(mapWidth,mapHeight,QImage::Format_ARGB32);
    clipped.fill(Qt::transparent);
    QBrush brush(image); // textured brush
    QPainter p(&clipped);
    p.setBrush(brush);
    p.setPen(Qt::NoPen);
    p.setRenderHint(QPainter::Antialiasing);
    //p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.drawEllipse(0,0,mapWidth,mapHeight);
    p.end();

    maps << QPixmap::fromImage(clipped);
  }
  //qDebug() << "generateNightMaps " << timer.elapsed() << "ms";
  emit nightMapsReady(maps);
  renderingMaps=false;
}
