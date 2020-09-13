#include <math.h>
#include "../include/utility-math.h"

double degreeToRadian(double degree) {
    return degree * ((double) M_PI / 180);
}

double getDistance(double latitude, double longitude, double previousLatitude, double previousLongitude){
    double differenceLatitude = degreeToRadian(latitude - previousLatitude);
    double differenceLongitude = degreeToRadian(longitude - previousLongitude);
    latitude = degreeToRadian(latitude);
    previousLatitude = degreeToRadian(previousLatitude);

    double a = sin(differenceLatitude / 2) * sin(differenceLatitude / 2) +
               sin(differenceLongitude / 2) * sin(differenceLongitude / 2) * cos(previousLatitude) * cos(latitude);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return RAGGIO_TERRA_METRI * c;
}

double getVelocity(double space, int time){
    /* Spazio in metri, time in secondi m/s */
    return space / time;
}
