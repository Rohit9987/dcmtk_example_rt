#pragma once

#include "controlpoint.h"
#include "contour.h"

class Beam
{

    std::vector<ControlPoint> m_controlPoints;
	std::vector<Contour> *m_contours;

    //TODO: use 6X, 6FFF, etc, from packt website
    double energy; 

	// TODO: move isocenter from controlpoint to beam
	std::vector<double> _isocenter {0,0,0};

    /*other details
     * MU
     *
     */
    


public:
    Beam() {};
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
