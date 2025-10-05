#ifndef MOONMAKER_H
#define MOONMAKER_H
#include<iostream>
#include<math.h>
#include<vector>
#include<numbers>
#include<omp.h>
using namespace std;


double density_function(double radius, double Moon_Radius){
    //scale 0.1 unit = 579.13 kilometers
    //Moon_Radius in kilometres
    // double R = Moon_Radius;
    // double rho_c = 5.5000;   // Core density kg/m³
    // double rho_s = 3.0000;   // Surface density kg/m³

    
    // double x = radius / R;
    // return rho_s + (rho_c - rho_s) * pow(1 - x, 2);
    return 100.0f/(4/3* M_PI*pow(radius,3));
}



vector<pair<vector<double>,double>> serial_calculate_centres_and_mass_serial(vector<double> Moon_center, double Moon_Radius, double fragment_radius)
{
    vector<pair<vector<double>, double>> fragments_result;
    
    for(double x  = Moon_center[0]- Moon_Radius; x <= Moon_center[0] + Moon_Radius; x+=2*fragment_radius){
        for(double y  = Moon_center[1]- Moon_Radius; y <= Moon_center[1] + Moon_Radius; y+=2*fragment_radius){
            for(double z  = Moon_center[2]- Moon_Radius; z <= Moon_center[2] + Moon_Radius; z+=2*fragment_radius){
                double distToMoonCenter = sqrt(pow(x - Moon_center[0], 2) + pow(y - Moon_center[1], 2) + pow(z - Moon_center[2], 2));
                if (distToMoonCenter + fragment_radius <= Moon_Radius) {
                    double mass_fragment = density_function(distToMoonCenter, Moon_Radius)* 4/3*M_PI*pow(fragment_radius, 3);
                    vector<double> coordinates = {x, y, z};
                    fragments_result.push_back(make_pair(coordinates,mass_fragment));

                }
            }
        }
    }

    return fragments_result;
}

vector<pair<vector<double>,double>> parallel_calculate_centres_and_mass_serial(vector<double> Moon_center, double Moon_Radius, double fragment_radius)
{
    vector<pair<vector<double>, double>> fragments_result;
    
    #pragma omp parallel for collapse(3)
    for(double x  = Moon_center[0]- Moon_Radius; x <= Moon_center[0] + Moon_Radius; x+=2*fragment_radius){
        for(double y  = Moon_center[1]- Moon_Radius; y <= Moon_center[1] + Moon_Radius; y+=2*fragment_radius){
            for(double z  = Moon_center[2]- Moon_Radius; z <= Moon_center[2] + Moon_Radius; z+=2*fragment_radius){
                double distToMoonCenter = sqrt(pow(x - Moon_center[0], 2) + pow(y - Moon_center[1], 2) + pow(z - Moon_center[2], 2));
                if (distToMoonCenter + fragment_radius <= Moon_Radius) {
                    double mass_fragment = density_function(distToMoonCenter, Moon_Radius)* 4/3*M_PI*pow(fragment_radius, 3);
                    vector<double> coordinates = {x, y, z};

                    #pragma omp critical
                    {
                        fragments_result.push_back(make_pair(coordinates, mass_fragment));
                    }

                }
            }
        }
    }

    return fragments_result;
}




#endif