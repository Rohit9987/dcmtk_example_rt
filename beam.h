#pragma once

#include "controlpoint.h"
#include "contour.h"

class Beam
{

	int _numCP;
    std::vector<ControlPoint> m_controlPoints;

	std::vector<Contour> *m_contours;

    //TODO: use 6X, 6FFF, etc, from packt website
    double energy; 

	// TODO: move isocenter from controlpoint to beam
	std::vector<double> _isocenter {0,0,0};

	// TODO: convert these into string_literals
	double _mu;

    /*other details
     * MU
     *
     */
    
	int _beamEnergy;


public:
	enum BeamType { STATIC, IMRT, VMAT, ELECTRON};
	enum BeamType _beamType;

    Beam() {};

    Beam(BeamType beamType, double mu, int numcp, double energy, std::vector<double> isocenter)
	: _beamType(beamType), _mu(mu), _numCP(numcp), _beamEnergy(energy), _isocenter(isocenter)
	{
		std::cout
			<< "BeamType : " << beamType
			<< "\nmu : " << mu
			<< "\nnumcp: " << numcp
			<< "\nEnergy: " << energy
			<< "\niso (" << isocenter[0] << ',' << isocenter[1] << ',' << isocenter[2] << ')'
			<< '\n';
	};
	
    ~Beam() {};


	void setContourPointerToBeam(std::vector<Contour>* contours)
	{ 
		m_contours = contours;

		// TODO: remove after execution
		std::cout << "Structs available\n";
        for(const auto& contour: *m_contours)
        {
            std::cout << contour.getContourName() << '\n';
        }


	}
};
