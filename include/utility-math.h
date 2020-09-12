#ifndef utility_math_h
#define utility_math_h

#define RAGGIO_TERRA_METRI 6367863

double getDistance(double latitude, double longitude, double previousLatitude, double previousLongitude);
double degreeToRadian(double degree);
double getVelocity(double space, int time);

#endif
