/*
 * Diffraction at sub-nucleon scale
 * Dipole amplitude for dipole-smooth nucleus scattering for testing/comparisons
 * Heikki Mäntysaari <mantysaari@bnl.gov>, 2015
 */

#ifndef smooth_ws_nuke_hpp
#define smooth_ws_nuke_hpp

#include "dipole.hpp"
#include "gdist_dglap.hpp"
#include "mz_ipsat/dipoleamplitude.hpp"
#include <tools/interpolation.hpp>
#include <string>
#include <sstream>
#include "dipole.hpp"

class Smooth_ws_nuke : public DipoleAmplitude
{
public:
    Smooth_ws_nuke(int A=197, Ipsat_version version = MZSAT);
    ~Smooth_ws_nuke();
    double Amplitude(double xpom, double q1[2], double q2[2] );
    std::string InfoStr();
    
    double Amplitude_bint(double xpom, double r);
    double Amplitude_sqr_bint(double xpom, double r);
    
    
private:
    int A;
    DGLAPDist gdist;
    Interpolator *T_A_interpolator;
    MZ_ipsat::DipoleAmplitude *mzipsat;
    Ipsat_version ipsat_version;
    bool saturation;
};

#endif /* smooth_ws_nuke_hpp */
