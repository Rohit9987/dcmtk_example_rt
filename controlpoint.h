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


    // itk contours
    using PixelType = float;
    constexpr static unsigned int Dimension = 3;
    using MeshType = itk::Mesh<PixelType, Dimension>;

    MeshType::Pointer bodyContour;

    void calculateGantryPoint()
    {
        double pi = 22./7.;
        double gantryAngleInRadian = cp_gantryAngle * M_PI / 180.0;
        cp_gantryPoint[0] = _isocenter[0] + 1000 * std::sin(gantryAngleInRadian);
        cp_gantryPoint[1] = _isocenter[1] + 1000 * std::cos(gantryAngleInRadian);
        cp_gantryPoint[2] = _isocenter[2];
    }

    void calculateSSD()             // exclude non Coplanar plans
    {
        // need the nearest point to the contour;
        // need the contour
        // need the gantry point
        // need isocenter
        cp_SSD = 100 ;

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

    void constructVolumeMesh(const OFFilename& rtStructFilename)
    {

        bodyContour = MeshType::New();             // part of the class

        std::cout << "Hello from the mesh world!\n";
    }
    
    void constructPlan(const OFFilename& rtPlanFilename)
    {
        std::cout << "Hello from the plan world!\n";
        // extract the isocenter to update the isocenter
        DcmFileFormat fileformat;
        OFCondition status = fileformat.loadFile(rtPlanFilename);
        if(status.good())
        {
            DRTPlanIOD rtplan;
            status = rtplan.read(*fileformat.getDataset());
            if(status.good())
            {
                OFString patientName;
                status  = rtplan.getPatientName(patientName);
                if (status.good())
                {
                    std::cout << "Patient's Name: " << patientName << std::endl;
                } 
                else
                    std::cerr << "Error: cannot access Patient's Name (" << status.text() << ")" << std::endl;

                /* TODO: write code to extract the following data and update in CPInformation
                 * 1. Monitor unit
                 * 2. depth for tpr/pdd
                 * 3. SSD
                 * 4. field size
                 * 5. MLC
                 *
                 */
                try
                {

                    auto beam = rtplan.getBeamSequence().getItem(0);        // 0 and 1 work for this example -  will use do while for iteration
                    OFString beamType;
                    beam.getBeamType(beamType);
                    std::cout << "Beam Type: " << beamType << '\n';

                    double mu; 
                    beam.getFinalCumulativeMetersetWeight(mu);
                    std::cout << "MU: " << mu << '\n';
                    // TODO: need to get the actual meterset

                    int numcp;
                    beam.getNumberOfControlPoints(numcp);
                    std::cout << "Number of control points: " << numcp << '\n';
                    auto cp = beam.getControlPointSequence();

                    // only use controlpoint 0 for now
                    cp[0].getGantryAngle(cp_gantryAngle);
                    cp[0].getBeamLimitingDeviceAngle(cp_collimatorAngle);
                    cp[0].getNominalBeamEnergy(_energy);


                    std::cout << "Gantry Angle: " << cp_gantryAngle
                              << "\nCollimator Angle: " << cp_collimatorAngle 
                              << "\nBeam Energy: " << _energy << '\n';


                    OFVector<double> isocenter;
                    cp[0].getIsocenterPosition(isocenter);
                    setIsocenter(isocenter);        
                    calculateGantryPoint(); 
                    std::cout << "(" << cp_gantryPoint[0] << "," << cp_gantryPoint[1] << "," << cp_gantryPoint[2] << ")\n";
                }
                catch(...)
                {
                    std::cerr << "EXception caught!\n";
                }

            }

            else
                std::cerr << "Error: cannot read RT Dose object (" << status.text() << ")" << std::endl;
        } 
        else
        {
            std::cerr << "Error: cannot load DICOM file (" << status.text() << ")" << std::endl;
        }
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
    

    // we do only for simple plans first, so only 1 or limited control points
    ControlPoint(const OFFilename& rtPlanFilename, const OFFilename& rtStructFilename)
    {
        constructVolumeMesh(rtStructFilename);
        constructPlan(rtPlanFilename);
        
    }
    
     
};
