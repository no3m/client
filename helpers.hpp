#ifndef HELPERS_H
#define HELPERS_H
#pragma once

#include "defines.h"


bool pixelAz2DistAz (const int, const int,
                     const int, const int,
                     double&, double&);
bool longlat2PixelRect(const double, const double,
                       const int, const int,
                       int&, int&);
bool pixelRect2longlat(const int, const int,
                       const int, const int,
                       double&, double&);
bool distAz2LongLat(const double, const double,
                    const double, const double,
                    double&, double&);

bool qrb(double, double, double, double, double*, double*);
bool locator2longlat(double*, double*, const char*);
bool isValidAngle(int);


// projection conversion functions
// all angles and lat/long in radians
inline
bool distAz2LongLat(const double distance, const double azimuth,
                                const double long1, const double lat1,
                                double &long2, double &lat2)
{
  if (distance > MAX_KM || distance < 0) return false;
  //if (long1 < -M_PI || long1 > M_PI) return false;
  //if (lat1 < -M_PI_2  || lat1 > M_PI_2 ) return false;

  auto angularDistance = distance / EARTH_RADIUS;
  lat2 = asin( sin(lat1)*cos(angularDistance)
                    + cos(lat1)*sin(angularDistance)*cos(azimuth) );

  auto long2tmp = long1 + atan2(sin(azimuth)*sin(angularDistance)*cos(lat1),
                             cos(angularDistance)-sin(lat1)*sin(lat2) );
	if (long2tmp < -M_PI || long2tmp > M_PI) {
  	long2tmp = fmod((long2tmp + 3.0 * M_PI), TWO_PI) - M_PI; // normalize to range -M_PI to +M_PI
	}
  long2 = long2tmp;

  return true;
}

// translate equirectangular (Plate Carree) projection pixels to lat,long
inline
bool pixelRect2longlat(const int x, const int y,
                                   const int sizeX, const int sizeY,
                                   double &longitude, double &latitude)
{
  //if (x < 0 || x > sizeX) return false;
  //if (y < 0 || y > sizeY) return false;

  longitude = (x - sizeX / 2.0) * (TWO_PI / sizeX);
  latitude = (y - sizeY / 2.0) * (-M_PI / sizeY);
  return true;
}

inline
bool longlat2PixelRect(const double longitude, const double latitude,
                                   const int sizeX, const int sizeY,
                                   int &x, int &y)
{
  //if (longitude < -M_PI || longitude > M_PI) return false;
  //if (latitude < -M_PI_2 || latitude > M_PI_2) return false;

  x = (longitude * sizeX / TWO_PI) + (sizeX / 2.0);
  y = (sizeY / 2.0) - (latitude * sizeY / M_PI);
  return true;
}

inline
bool pixelAz2DistAz(const int x, const int y,
                                 const int sizeXY, const int kmRadius,
                                 double &distance, double &azimuth)
{
	double xx = x - (sizeXY/2.0); // shift origin
	double yy = y - (sizeXY/2.0);
  azimuth = std::atan2(xx, -yy);
  distance = sqrt(xx*xx + yy*yy) * (kmRadius * 2.0 / sizeXY);
  return true;
}


#endif
