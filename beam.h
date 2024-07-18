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

	Contour* m_body;
	Contour* getBodyContour() 
	{ 
		if(m_body == nullptr)
			std::cout << "Error: nullptr in beam!\n";
		return m_body;
	}

    Beam() {}

    Beam(BeamType beamType, double mu, int numcp, double energy, std::vector<double> isocenter)
	: _beamType(beamType), _mu(mu), _numCP(numcp), _beamEnergy(energy), _isocenter(isocenter)
	{

	} 
	
    ~Beam() {}


	void setContourPointerToBeam(std::vector<Contour>* contours)
	{ 
		m_contours = contours;

		// TODO: remove after execution
		std::cout << "Structs available\n";
		for(auto& contour: *m_contours)
        {
            std::cout << contour.getContourName() << '\n';
			if(std::strcmp(contour.getContourName(), "BODY") == 0)
			{
				std::cout << "Found Body\n";
				m_body = &contour;

			}
        }
	}



	void addControlPoint(ControlPoint& cp1)
	{
		m_controlPoints.push_back(cp1);
		//TODO: remove later
		std::cout << "__________________________________________________________\n";
		std::cout << "Total CPs: " << m_controlPoints.size() << '\n';
	}
};
