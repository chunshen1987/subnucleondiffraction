/*
 * Diffraction at sub-nucleon scale
 * Dipole amplitude for a IPglasma nucleus
 * Heikki Mäntysaari <mantysaari@bnl.gov>, 2015
 */


#include "ipglasma.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <tools/config.hpp>
#include <tools/tools.hpp>

using namespace Amplitude;

using std::cout;
using std::endl;

const int NC=3;

/*
 * Calculate dipole amplitude form Wilson lines
 * Search the closest grid point that corresponds to the given quark/antiquark coordinate
 * Then calcualte 1 - 1/Nc Tr U(quark) U^dagger(antiquark)
 */
double IPGlasma::Amplitude(double xpom, double q1[2], double q2[2] )
{
    // First find corresponding grid indeces
    WilsonLine quark = GetWilsonLine(q1[0], q1[1]);
    WilsonLine antiquark = GetWilsonLine(q2[0], q2[1]);
    antiquark = antiquark.HermitianConjugate();
    WilsonLine prod = quark*antiquark;
    std::complex<double > amp =  1.0 - 1.0/NC * prod.Trace();
    
    return amp.real();
}


WilsonLine& IPGlasma::GetWilsonLine(double x, double y)
{
    int xind = FindIndex(x, xcoords);
    int yind = FindIndex(y, ycoords);
    
    //cout << "Coordinates " << x << ", "  << y << " indeces " << xind << ", " << yind << endl;
    
    return wilsonlines[ xind*xcoords.size() + yind];
    
}

IPGlasma::IPGlasma(std::string file)
{
    // Load data
    // Syntax: x y [fm] matrix elements Re Im for elements (0,0), (0,1), (0,2), (1,0), ...
    std::ifstream f(file.c_str());
    
    if (!f.is_open())
    {
        std::cerr << "Could not open file " << file << std::endl ;
        exit(1);
        return;
    }
    std::string line;
    
    while(!f.eof() )
    {
        std::getline(f, line);
        if (line[0]=='#' or line.length() < 10)   // Comment line, or empty
        {
            continue;
        }
        
        // Parse
        std::stringstream ss(line);
        // Get coordinates
        double x,y;
        ss >> x;
        ss >> y;
        
        // Datafile is in fm, but we want to use GeVs in this code
        //x*= FMGEV;
        //y *= FMGEV;
        // DEBUG TODO: easier to test without changing units!
        
        // Read rows and columns
        std::vector< std::vector< std::complex<double> > > matrix;
        for (int row=0; row < 3; row++)
        {
            std::vector< std::complex<double> > tmprow;
            for (int col=0; col<3; col++)
            {
                double real, imag;
                ss >> real;
                ss >> imag;
                std::complex<double> element (real, imag);
                tmprow.push_back(element);
            }
            matrix.push_back(tmprow);
        }
        
        // Save Wilson line
        WilsonLine w(matrix);
        wilsonlines.push_back(w);
        
        // We assume that grid is symmetric, so dont save same valeus multiple times
        // In the datafile the coordinates are increasing
        if (xcoords.size() == 0 or x > xcoords[xcoords.size()-1])
            xcoords.push_back(x);
        if (ycoords.size() == 0 or y > ycoords[ycoords.size()-1])
            ycoords.push_back(y);
        
    }
    
    f.close();
    

    
    // Now, given that we have a point (x,y), we can find the index xind such that
    // xcoords[xind] is closest to x, and similarly ind
    // Then, the corresponding Wilson line is
    // wilsonlines[ xcoords.size()*xind + yind]
    // Of course this is symmetric and we could just as well swap xind and yind

    
    std::cout <<"# Loaded " << wilsonlines.size() << " Wilson lines from file " << file << ", grid size " << xcoords.size() << " x " << ycoords.size() << std::endl;

        
    
}

