#ifndef ROCHE_H
#define ROCHE_H

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include "Gravity.h" 

// Calculate Roche radius for a fluid/self-gravitating moon
inline double get_roche_radius(const Body& planet, const Body& moon, double planet_radius, double moon_radius)
{
    double dens_planet = planet.mass / ((4.0 / 3.0) * M_PI * pow(planet_radius, 3));
    double dens_moon = moon.mass / ((4.0 / 3.0) * M_PI * pow(moon_radius, 3));

    return 2.44 * planet_radius * pow(dens_planet / dens_moon, 1.0 / 3.0);
}

// Check if the moon is inside the Roche limit
inline bool update_roche_status(const Body& planet, const Body& moon, double planet_radius, double moon_radius)
{
    double roche_radius = get_roche_radius(planet, moon, planet_radius, moon_radius);
    double distance = glm::length(planet.position - moon.position);

    return distance <= roche_radius;
}

#endif
