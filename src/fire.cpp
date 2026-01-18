#include "fire.h"

double iso_fire_curve(double time) {
    return 20 + 345 * log10(8 * time / 60 + 1);
}

double hc180_fire_curve(double time) {
    return 20 + 1080 * (1 - 0.325 * exp(-0.167 * time) - 0.675 * exp(-2.5 * time));
}


double parametric_fire(double time, const parameters& cfg) {
    double Av = cfg.Av;
    double At = cfg.At;
    double heq = cfg.heq;
    double b = cfg.b;
    double q_td = cfg.q_td;
    double growth = cfg.growth_rate;
    double t_lim = growth / 60;

    double opening_factor = Av*std::pow(heq, 0.5)/At;
    double gamma = (std::pow(opening_factor/b, 2))/std::pow(0.04/1160, 2);

    double time_h = time / 3600;
    double t_star = time_h * gamma;

    double t_max_vent = (0.2e-3 * q_td) / opening_factor;
    double t_max;
    if (t_max_vent > t_lim) {
        t_max = t_max_vent;
    }
    else t_max = t_lim;

    double O_lim = 0.1e-3 * q_td / t_lim;
    double gamma_lim = std::pow(O_lim / b, 2) / std::pow(0.04/1160, 2);

    if (opening_factor > 0.04 && q_td < 75 && b < 1160) {
        double k = 1 + ((opening_factor - 0.04)/0.04)*((q_td-75)/75)*((1160-b)/1160);
        gamma_lim *= k;
    }
    if (t_max == t_lim) { 
        t_star = time_h * gamma_lim; 
    }

    double t_star_max = t_max * gamma;

    double T_max = 20 + 1325 * ( 1 - 0.324 * std::exp(-0.2 * t_star_max) - 0.204 * std::exp(-1.7 * t_star_max) - 0.472 * std::exp(-19 * t_star_max) );

    double T = 20;

    if (time_h <= t_max) {
         T = 20 + 1325 * ( 1 - 0.324 * std::exp(-0.2 * t_star) - 0.204 * std::exp(-1.7 * t_star) - 
         0.472 * std::exp(-19 * t_star) );
        }
    
    
    else {
        double x; 
        if (t_max > t_lim) { 
            x = 1; 
        } 
        
        else {
            x = t_lim * gamma / t_star_max;
        }

        if (t_star_max <= 0.5) { 
            T = T_max - 625 * (t_star - t_star_max * x);
        } 

        else if (t_star_max < 2) { 
            T = T_max - 250 * (3 - t_star_max) * (t_star - t_star_max * x);
        } 
        
        else { 
            T = T_max - 250 * (t_star - t_star_max * x);
        } 
    }

    if ( T < 20) {
        return 20;
    }

    return T;

}

double fire_temperature(double time, const parameters& cfg){
    if (cfg.fire_curve_exposed == 1)
        return iso_fire_curve(time);

    if (cfg.fire_curve_exposed == 2)
        return hc180_fire_curve(time);
    
    if (cfg.fire_curve_exposed == 3)
        return parametric_fire(time, cfg);

    if (cfg.fire_curve_exposed == 4)
        return cfg.constant_surface_temp;

    return iso_fire_curve(time);
}
