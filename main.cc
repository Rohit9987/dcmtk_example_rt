#include <iostream>

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

using namespace std;

/*
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

int readPlan(const OFFilename& planFilename)
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
                cout << "Patient's Name: " << patientName << endl;
            } 
            else
                cerr << "Error: cannot access Patient's Name (" << status.text() << ")" << endl;

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
                 * Jaw positions            - yes, need to understand which jaw, mlc is which
                 * leaf positions
                 */

                
                


            




            }
            catch(...)
            {
                std::cerr << "EXception caught!\n";
            }

        }

        else
            cerr << "Error: cannot read RT Dose object (" << status.text() << ")" << endl;
    } 
    else
    {
        cerr << "Error: cannot load DICOM file (" << status.text() << ")" << endl;
        return EXIT_FAILURE;
    }
    return 0;
}

int readDose(const OFFilename& doseFilename)
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
                cout << "Patient's Name: " << patientName << endl;
            } 
            else
                cerr << "Error: cannot access Patient's Name (" << status.text() << ")" << endl;
        } 
        else
            cerr << "Error: cannot read RT Dose object (" << status.text() << ")" << endl;
    } 
    else
    {
        cerr << "Error: cannot load DICOM file (" << status.text() << ")" << endl;
        return EXIT_FAILURE;
    }
    return 0;
}

int readStruct(const OFFilename& structFilename)
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
                cout << "Patient's Name: " << patientName << endl;
                rtstruct.getStructureSetName(patientName);
                std::cout << "Struct name: " << patientName << std::endl;
                // extract the body contour
                auto contourSequence = rtstruct.getStructureSetROISequence();
                for(int i = 0; i < contourSequence.getNumberOfItems(); i++)
                {
                    auto ROI = contourSequence[i];
                    OFString name;
                    ROI.getROIName(name);
                    std::cout << "name : " << name << '\n';
                }

            } 
            else
                cerr << "Error: cannot access Patient's Name (" << status.text() << ")" << endl;
        } 
        else
            cerr << "Error: cannot read RT struct object (" << status.text() << ")" << endl;
    } 
    else
    {
        cerr << "Error: cannot load DICOM file (" << status.text() << ")" << endl;
        return EXIT_FAILURE;
    }
    return 0;
}

int main(int argc, char** argv)
{
    using namespace std::literals;

    OFFilename planFilename, doseFilename, structFilename;         // TODO: use char array to reduce memory
    if(argc < 2)
    {
        planFilename = "data/rtplan.dcm";
        doseFilename = "data/rtdose.dcm";
        structFilename = "data/rtstruct.dcm";
    }
    if (argc>1) 
    {
        planFilename = argv[1];
        doseFilename = argv[2];
        structFilename = argv[3];
    }


    readPlan(planFilename);
    std::cout << "--------------------------------------\n";
    readDose(doseFilename);
    std::cout << "--------------------------------------\n";
    readStruct(structFilename);

    
    return 0;
}
