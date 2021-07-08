#include "helpers.hpp"

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
bool locator2longlat(double *longitude, double *latitude, const char *locator)
{
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
bool qrb(double lon1, double lat1, double lon2, double lat2, double *distance, double *azimuth)
{
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

bool isValidAngle(int value) {

  if (value >= 0 && value < 360) {
    return true;
  } else {
    return false;
  }
}
