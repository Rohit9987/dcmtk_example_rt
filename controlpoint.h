#pragma once

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/ofstd/ofconapp.h"

#include "dcmtk/dcmrt/drtdose.h"
#include "dcmtk/dcmrt/drtimage.h"
#include "dcmtk/dcmrt/drtplan.h"
#include "dcmtk/dcmrt/drtstrct.h"
#include "dcmtk/dcmrt/drttreat.h"
#include "dcmtk/dcmrt/drtionpl.h"
#include "dcmtk/dcmrt/drtiontr.h"

#include "contour.h"

#include <cmath>

class ControlPoint
{
private:
    double cp_gantryAngle;
    double cp_collimatorAngle;
    double cp_couchAngle;

    double cp_MU;               // TODO: need to find out from DCMTK
    double cp_SSD;              // TODO: calculate from the SS
    double _energy;

	
    std::vector<double> _isocenter {0,0,0};
    std::vector<double> cp_gantryPoint {0,0,0};


	// jaws and mlc;
	double x1, x2, y1, y2;
	double _mlc[120];


    // itk contours
	// TODO: move this to the beam
	// just send the gantry point for calculation
	// or to the plan itself

	Contour* m_body;

    void calculateGantryPoint()
    {
        double pi = 22./7.;
        double gantryAngleInRadian = cp_gantryAngle * M_PI / 180.0;
        cp_gantryPoint[0] = std::round(_isocenter[0] + 1000 * std::sin(gantryAngleInRadian)*10)/10;
        cp_gantryPoint[1] = std::round(_isocenter[1] + 1000 * std::cos(gantryAngleInRadian)*10)/10;
        cp_gantryPoint[2] = _isocenter[2];

    }

    void calculateSSD()             // exclude non Coplanar plans
    {
        // need the nearest point to the contour;
        // need the contour
        // need the gantry point
        // need isocenter
        cp_SSD = 100 ;
		std::cout << "Gantry Angle : " << cp_gantryAngle;


    }

    void setIsocenter(OFVector<double> isocenter)
    {
        _isocenter[0] = isocenter[0];
        _isocenter[1] = isocenter[1];
        _isocenter[2] = isocenter[2];
        // TODO: to be removed
        std::cout << "Isocenter: ("
                  << _isocenter[0] << ", "
                  << _isocenter[1] << ", "
                  << _isocenter[2] << "\n";
    }


public:
    ~ControlPoint()
    {}

    // list of constructors, remove the ones not needed
    ControlPoint(double gantryAngle): cp_gantryAngle(gantryAngle), _isocenter({0,0,0})
    {
        calculateGantryPoint(); 
        std::cout << "(" << cp_gantryPoint[0] << ", " << cp_gantryPoint[1] << "," << cp_gantryPoint[2] << ")\n";
    }
    
	// colangle and couch angle can be beam properties
    ControlPoint(double gantryAngle, double x1, double x2, double y1, double y2, double  mlc[]): 
	cp_gantryAngle(gantryAngle), 
	_isocenter({0,0,0}),
	x1(x1),
	x2(x2),
	y1(y1),
	y2(y2)
    {
		//TODO:  remove later
		// lets print out the control Points
		std::cout << "gantryAngle : " << gantryAngle
				  << "(" << x1 << ","
				  <<		x2 << ","
				  <<		y1 << ","
				  <<		y2 << ")"
				  << '\n';

		for(int i = 0; i < 120; i++ )
			_mlc[i] = mlc[i];
		
        calculateGantryPoint(); 
		calculateSSD();
    }

	void setBodyContour(Contour* body)
	{
		m_body = body;
		if(m_body == nullptr)
		{
			std::cout << "Error null pointer\n";
			return;
		}
		std::cout << "from controlPoint: \n";
		std::cout << m_body->getContourName();

		cp_SSD = 1000.0 - m_body->calculateDepth();


		std::cout << "SSD: " << cp_SSD << '\n';
	}
    // we do only for simple plans first, so only 1 or limited control points
};
