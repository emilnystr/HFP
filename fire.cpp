#include "fire.h"

double iso_fire_curve(double time) {
    return 20.0 + 345.0 * log10(8.0 * time / 60.0 + 1.0);
}

double hc180_fire_curve(double time) {
    return 20.0 + 1080.0 * (1.0 - 0.325 * exp(-0.167 * time) - 0.675 * exp(-2.5 * time));
}


double parametric_fire(double time, parameters& cfg) {
    double Av = cfg.Av;
    double At = cfg.At;
    double heq = cfg.heq;
    double b = cfg.b;
    double q_td = cfg.q_td;
    double growth = cfg.growth_rate;

    
}


double fire_temperature(double time, const parameters& cfg)
{
    if (cfg.fire_curve_exposed == 1)
        return iso_fire_curve(time);

    if (cfg.fire_curve_exposed == 2)
        return hc180_fire_curve(time);
    
    if (cfg.fire_curve_exposed == 3)
        return parametric_fire(time);

    if (cfg.fire_curve_exposed == 4)
        return cfg.constant_surface_temp;

    return iso_fire_curve(time);
}
