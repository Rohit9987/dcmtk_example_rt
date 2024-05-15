#pragma once

#include "controlpoint.h"

class Beam
{

    std::vector<ControlPoint> m_controlPoints;

    //TODO: use 6X, 6FFF, etc, from packt website
    double energy; 

    /*other details
     * MU
     *
     */
    


public:
    Beam() {};
    ~Beam() {};
};
