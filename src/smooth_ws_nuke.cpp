/*
 * Diffraction at sub-nucleon scale
 * Dipole amplitude for dipole-smooth nucleus scattering for testing/comparisons
 * Heikki Mäntysaari <mantysaari@bnl.gov>, 2015
 */

#include "smooth_ws_nuke.hpp"
#include "mz_ipsat/dipoleamplitude.hpp"
#include "dipole.hpp"
#include "vector.hpp"
#include <tools/tools.hpp>
#include <tools/interpolation.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>

// IPsat 2012
extern "C" {
    double dipole_amplitude_(double* xBj, double* r, double* b, int* param);
};

int IPSAT12_NUKE_PAR = 2;    // m_c=1.4 GeV

using Amplitude::SQR;

const double NC=3.0;

using namespace std;

Smooth_ws_nuke::Smooth_ws_nuke(int A_, Ipsat_version ipsatv)
{
    ipsat_version = ipsatv;
    
    if (ipsat_version == MZSAT)
    {
        double C=2.2894; double mu0 = std::sqrt(1.1); double lambdag=0.08289; double Ag=2.1953; double mc=1.3528;
        mzipsat = new MZ_ipsat::DipoleAmplitude(C, mu0, lambdag , Ag , mc );
        mzipsat->SetSaturation(true);
        saturation = true;
    }
    else if (ipsat_version == MZNONSAT)
    {
        double C = 4.2974; double mu0=std::sqrt(1.1); double lambdag = -0.006657; double Ag=3.0391; double mc = 1.3504;
        mzipsat = new MZ_ipsat::DipoleAmplitude(C, mu0, lambdag, Ag, mc);
        mzipsat->SetSaturation(false);
        saturation=false;
    }
    else
    {
        cerr << "Unknown ipsat version in Smooth_ws_nuke" << endl;
        exit(1);
    }
    
    
    A=A_;
    InitializeWSDistribution(A);
    
    // Initialize interpolator
    vector<double> bvals;
    vector<double> tavals;
    for (double b=0; b<100; b+=0.1)
    {
        bvals.push_back(b);
        tavals.push_back(T_A(b, A));
    }
    T_A_interpolator = new Interpolator(bvals, tavals);
    T_A_interpolator->SetOverflow(0);
    T_A_interpolator->SetUnderflow(0);
    T_A_interpolator->SetFreeze(true);
}

Smooth_ws_nuke::~Smooth_ws_nuke()
{
    delete T_A_interpolator;
}
double Smooth_ws_nuke::Amplitude(double xpom, double q1[2], double q2[2] )
{
    if (saturation == false)
    {
        cerr << "Smooth_ws_nuke::Amplitude does not support nonsat yet" << endl;
        exit(1);
    }
    if (A < 100)
    {
        cerr << "Smooth_ws_nuke::Amplitude assumes large A" << endl;
        exit(1);
    }

    // Basically KT hep-ph/0304189
    
    // Total dipole-proton xs
    // \sigma_dip = 2 \int d^2 b N(r,b)
    Vec v1(q1[0], q1[1]); Vec v2(q2[0],q2[1]);
    Vec rvec = v1 - v2;
    Vec bvec = v1+v2;
    bvec = bvec*0.5;
    double b = bvec.Len();
    double r = rvec.Len();
    double sigmap = 2.0*mzipsat->N_bint(r, xpom);
    
    if (sigmap < 0)
    {
        cerr << "ERROR: sigma_p= " << sigmap << ", r=" << r <<", xpom=" << xpom << endl;
    }
    
    double res = 1.0 - std::exp(- 1.0/2.0 * A * T_A_interpolator->Evaluate(b)*sigmap);
    
    if (res < 0 or res > 1)
    {
        cout << "Error: Smooth ws nuke dipole amplitude " << res << ", r=" << r << ", b=" << b <<", T(b)=" << T_A_interpolator->Evaluate(b) << endl;
    }
    
    return res;
    
    // KT (41): Assumes large A and small dipole xs, not really realistic
    /*
    double r = std::sqrt( SQR(q1[0]-q2[0]) + SQR(q1[1]-q2[1]) );
    
    // Take the nucleon density at the geometric center of the two quarks
    double b = std::sqrt( SQR( (q1[0]+q2[0])/2.0 ) + SQR( (q1[1]+q2[1])/2.0) );
    
    return 1.0 - std::exp( -r*r * M_PI*M_PI / (2.0 * NC) * mzipsat->Alphas_xg(xpom, mzipsat->MuSqr(r)) * A * T_A_interpolator->Evaluate(b));
     */
    
    
   
}

std::string Smooth_ws_nuke::InfoStr()
{
    std::stringstream ss;
    ss << "#Optigal Glauber nucleus, A=" << A << endl;;
    return ss.str();
}
