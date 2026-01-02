#ifndef FIRE_CURVES_H
#define FIRE_CURVES_H

#include <cmath>
#include "config.h"

double iso_fire_curve(double time);
double hc180_fire_curve(double time);
double parametric_fire(double time);

double fire_temperature(double time, const parameters& cfg);

#endif