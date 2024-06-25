#pragma once

#include "controlpoint.h"

class Beam
{

    std::vector<ControlPoint> m_controlPoints;

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
};
