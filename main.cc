#include <iostream>
#include <vector>
#include <cmath>

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"

// not sure what this is
#include "dcmtk/ofstd/ofconapp.h"

// rt headers
#include "dcmtk/dcmrt/drtdose.h"
#include "dcmtk/dcmrt/drtimage.h"
#include "dcmtk/dcmrt/drtplan.h"
#include "dcmtk/dcmrt/drtstrct.h"
#include "dcmtk/dcmrt/drttreat.h"
#include "dcmtk/dcmrt/drtionpl.h"
#include "dcmtk/dcmrt/drtiontr.h"

// time
#include <time.h>

// itk headers
#include "itkMesh.h"


// local header
#include "plan.h"

// operator overloading
std::ostream& operator<<(std::ostream& os, OFVector<double> data)
{
    os << "[";
    for(int i =0; i < data.size(); i++)
    {
        if ((i+1)%3==1)
            os << ((i+1)/3) << ":(x";
        else if ((i+1)%3==2)
            os << "y:";
        else
            os << "z:";

        os << data[i];
        if ((i+1)%3==0)
        {

            os << ")";
        }

        if (i != data.size() - 1)
            os << ',';
    }
    os << "]\n";
    return os;
}
void printVector(OFVector<double> data)
{
    if(data[2] < 555 && data[2] > 554)           
    {
        std::cout << "[";
        double x, y, z;
        for(int i =0; i < data.size(); i++)
        {
            if ((i+1)%3==1)
                x = data[i]; 
            else if ((i+1)%3==2)
                y = data[i];
            else
                z = data[i]; 

            if(z < 555 && z > 554)
                std::cout << "(" << x << ',' << y << ',' << z << "), "; 

        }
    std::cout << "]\n";
    }
}


/*
 * o1
TODO:
    1. open a rt-plan
    2. extract the information needed to calculate the dose                     - in progress, need data - rtplan, rtdose, rtstruct
        a. MU
        b. SSD
        c. field size
            - X and Y jaws
            - MLCs
        d. Energy
        e. depth inside the patient
        f.

    3. create the appropriate classes/files required
        - create files with PDD, OF and OAR data

    4. calculate the dose
        a. use simple 2d calculation
        b. use clarkson method
        c. pencil beam 
    5. verify the dose from the rt-dose file

    6. make UI



*/

OFVector<double> isocenter;

bool readPlan(const OFFilename& planFilename)
{
    // open the rtplan file
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(planFilename);
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

            /* TODO: write code to extract the following data
             * and store to a class
             * 1. Monitor unit
             * 2. depth for tpr/pdd
             * 3. SSD
             * 4. field size
             * 5. MLC
             *
             */
            try
            {
                auto beam = rtplan.getBeamSequence().getItem(1);        // 0 and 1 work for this example -  will use do while for iteration
                OFString beamType;
                beam.getBeamType(beamType);
                std::cout << "Beam Type: " << beamType << '\n';


                double mu; 
                beam.getFinalCumulativeMetersetWeight(mu);
                std::cout << "MU: " << mu << '\n';

                int numcp;
                beam.getNumberOfControlPoints(numcp);
                std::cout << "Number of control points: " << numcp << '\n';
                auto cp = beam.getControlPointSequence();
                
                for(int i = 0; i < numcp - 1; i++)
                {
                    double angle;
                    cp[i].getGantryAngle(angle);
                    std::cout << "Gantry Angle: " << angle << '\n';

                    double colAngle;
                    cp[i].getBeamLimitingDeviceAngle(colAngle);
                    std::cout << "Col Angle: " << colAngle << '\n'; 


                    double energy;
                    cp[i].getNominalBeamEnergy(energy);
                    std::cout << "Beam Energy: " << energy << '\n';

                    double ssd;
                    cp[i].getSourceToSurfaceDistance(ssd);
                    std::cout << "SSD: " << ssd << '\n';

                    float ssd1;
                    cp[i].getSourceToExternalContourDistance(ssd1);
                    std::cout << "SSD1: " << ssd1 << '\n';
                    
                    OFString entryPoint;
                    cp[i].getSurfaceEntryPoint(entryPoint);
                    std::cout << "EntryPoint" << entryPoint << '\n';

                    OFVector<double> isocenter;
                    cp[i].getIsocenterPosition(isocenter);
                    std::cout << "Isocenter:"<< isocenter[0] << '\n';
                    std::cout << "Isocenter:"<< isocenter[1] << '\n';
                    std::cout << "Isocenter:"<< isocenter[2] << '\n';

                    OFVector<double> surface;
                    cp[i].getSurfaceEntryPoint(surface);
                    std::cout << "Surface: " << surface[0] << '\n';
                    std::cout << "Surface: " << surface[1] << '\n';
                    std::cout << "Surface: " << surface[2] << '\n';

                    auto blds = cp[i].getBeamLimitingDevicePositionSequence();
                    for(int j = 0; j < blds.getNumberOfItems(); j++)
                    {
                        OFString bldType;
                        blds[j].getRTBeamLimitingDeviceType(bldType);
                        std::cout << "BLD Type: " << bldType << '\n';;
                        OFVector<double> positions;
                        blds[j].getLeafJawPositions(positions);
                        for(auto position: positions)
                            std::cout << j << ": " << position << "\n";
                    }

                }

                /* TODO
                 * SSD ????                     - haven't been able to find ssd
                 * (300A, 0130) dicom tag
                 *
                 * Jaw positions            - yes,   need to calculate the eqsq, and clarkson
                 * leaf positions           - ask nikhil to help record video online
                 */

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
        return EXIT_FAILURE;
    }
    return 0;
}

bool readDose(const OFFilename& doseFilename)
{
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(doseFilename);
    if (status.good())
    {
        DRTDoseIOD rtdose;
        status = rtdose.read(*fileformat.getDataset());
        if (status.good())
        {
            OFString patientName;
            status = rtdose.getPatientName(patientName);
            if (status.good())
            {
                std::cout << "Patient's Name: " << patientName << std::endl;
            } 
            else
                std::cerr << "Error: cannot access Patient's Name (" << status.text() << ")" << std::endl;
        } 
        else
            std::cerr << "Error: cannot read RT Dose object (" << status.text() << ")" << std::endl;
    } 
    else
    {
        std::cerr << "Error: cannot load DICOM file (" << status.text() << ")" << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}

bool readStruct(const OFFilename& structFilename)
{
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(structFilename);
    if (status.good())
    {
        DRTStructureSetIOD rtstruct;
        status = rtstruct.read(*fileformat.getDataset());
        if (status.good())
        {
            OFString patientName;
            status = rtstruct.getPatientName(patientName);
            if (status.good())
            {
                std::cout << "Patient's Name: " << patientName << std::endl;
                rtstruct.getStructureSetName(patientName);
                std::cout << "Struct name: " << patientName << std::endl;
                // extract the body contour
                auto contourSequence = rtstruct.getStructureSetROISequence();       // rtstruct -> getStructureSetROISequence();
                auto ROIcontourSequence = rtstruct.getROIContourSequence();

                for(int i = 0; i < contourSequence.getNumberOfItems(); i++)
                {
                    auto ROI = contourSequence[i];                      //  Item 
                    OFString name;
                    ROI.getROIName(name);                               // item->GetName();

                    if(name == "BODY")
                    {
                        std::cout << "name : " << name << '\n';
                        ROI.getROINumber(name);                                 // ROINumber = ReferenceNumber
                        std::cout << "ROINumber : " << name << '\n';        

                        std::cout << "Execute the logic\n";

                        auto ROI1 = ROIcontourSequence[i];
                        int refNumber;
                        ROI1.getReferencedROINumber(refNumber);
                        std::cout << "Reference number: " << refNumber << '\n';

                        auto contour = ROI1.getContourSequence();
                         
                        int contourPlane = 0;
                        while(contour[contourPlane++].isValid())
                        {
                            //std::cout << "Contour plane: " << (contourPlane-1) << '\n';
                            //int val;
                            //contour[contourPlane].getNumberOfContourPoints(val);
                            //std::cout << "contour points: " << val << '\n';

                            OFVector<double> data;
                            contour[contourPlane].getContourData(data);                            // this funcition gives out (x,y,z) or so;
                            printVector(data);
                            //std::cout << data;                                          // need to extract the x, y, z to get the contour points
                        }

                    }

                }

            } 
            else
                std::cerr << "Error: cannot access Patient's Name (" << status.text() << ")" << std::endl;
        } 
        else
            std::cerr << "Error: cannot read RT struct object (" << status.text() << ")" << std::endl;
    } 
    else
    {
        std::cerr << "Error: cannot load DICOM file (" << status.text() << ")" << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}

int main(int argc, char** argv)
{
    clock_t clkStart, clkEnd;
    clkStart = clock();
        

    using namespace std::literals;

    OFFilename planFilename, doseFilename, structFilename;         // TODO: use char array to reduce memory
    if(argc < 2)
    {
        planFilename = "data/rtplan.dcm";
        structFilename = "data/rtstruct.dcm";
        doseFilename = "data/rtdose.dcm";
    }
    if (argc>3) 
    {
        planFilename = argv[1];
        structFilename = argv[2];
        doseFilename = argv[3];
    }

    Plan plan(planFilename, structFilename, doseFilename);


    clkEnd = clock();
    std::cout << (float)(clkEnd - clkStart)/CLOCKS_PER_SEC << "SECONDS\n";
    
    return 0;
}
